
#include <QCoreApplication>
#include <QtCrypto>
#include <unistd.h>
#include <Ice/Ice.h>

#include "QsLog.h"
#include "QsLogDest.h"
#include "authentication.h"
#include "userdbproxy.h"
#include "server.h"
#include "settings.h"

struct Args {
    Args() : updateSettings(false) { }

    QString settingsFile;
    bool updateSettings;
};

static void usage();
static void parseArgs(int argc, char **argv, Args &args);

static void usage()
{
    fprintf(stderr,
            "-h             print this message and exit\n"
            "-f file        read settings from file\n"
            "-i init        init settings file and exit\n");
}

static void parseArgs(int argc, char **argv, Args &args)
{
    int opt;
    while ((opt = getopt(argc, argv, "ihf:")) != -1) {
        switch (opt) {
        case 'f':
            args.settingsFile = optarg;
            break;
        case 'i':
            args.updateSettings = true;
            break;
        case 'h':
            usage();
            exit(0);
        default:
            break;
        }
    }
}

struct Application : virtual public Ice::Application {

    /**
     * Run the server
     * Returns only when the server is being exited.
     */
    virtual int run(int , char **) {
        QLOG_TRACE() << __PRETTY_FUNCTION__;

        /* The server handles adapter setup and wraps needed interfaces. */
        sdcs::Server::create(communicator());

        communicator()->waitForShutdown();

        return 0;
    }
};

#define GET_STDSTRING_SETTING(key) sdc::Settings::getValue(key).toString().toStdString()

int main(int argc, char **argv)
{
    /* Parse command line arguments. */
    Args args;
    parseArgs(argc, argv, args);

    /* Init logger */
    QsLogging::Logger &logger = QsLogging::Logger::instance();
    logger.setLoggingLevel(QsLogging::TraceLevel);

    QsLogging::DestinationPtr debugDestination(
        QsLogging::DestinationFactory::MakeDebugOutputDestination());
    logger.addDestination(debugDestination.get());

    /* Create our Qt core application. */
    QCoreApplication a(argc, argv);

    /* Init Settings. */
    QCoreApplication::setOrganizationName("group22");
    QCoreApplication::setApplicationName("sdcs");

    if (args.settingsFile.isNull())
        sdc::Settings::init();
    else
        sdc::Settings::init(args.settingsFile);

    if (args.updateSettings) {
        sdc::Settings::create();
        return 0;
    }

    /* Init Ice properties. */
    Ice::PropertiesPtr props = Ice::createProperties();
    props->setProperty("Ice.Plugin.IceSSL", "IceSSL:createIceSSL");
    props->setProperty("Ice.Override.Timeout", "5000");
    props->setProperty("IceSSL.DefaultDir", GET_STDSTRING_SETTING(sdc::Settings::SCertsDir));
    props->setProperty("IceSSL.CertFile", GET_STDSTRING_SETTING(sdc::Settings::SCertPath));
    props->setProperty("IceSSL.KeyFile", GET_STDSTRING_SETTING(sdc::Settings::SKeyPath));
    props->setProperty("IceSSL.CertAuthFile", GET_STDSTRING_SETTING(sdc::Settings::SCertAuthPath));

    /* This is necessary so that the client doesn't need to send a certificate, e.g.
     * for testing server availability.
     */
    props->setProperty("IceSSL.VerifyPeer", "0");

    /* Again, nested calls lead to deadlock issues. This time, the problem
     * was caused by the echo() call in Authentication::login(). */
    props->setProperty("Ice.ThreadPool.Client.SizeMax", "5");
    props->setProperty("Ice.ThreadPool.Server.SizeMax", "5");

    Ice::InitializationData id;
    id.properties = props;

    /* Handles initialization and cleanup of QCA resources. */
    QCA::Initializer qca;
    Q_UNUSED(qca);

    /* Our application handles the main loop. */
    Application srv;

    return srv.main(argc, argv, id);
}
