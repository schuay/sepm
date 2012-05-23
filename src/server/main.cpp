
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
    QString dbDriver,
            dbDatabase,
            dbHost,
            dbUser,
            dbPassword,
            certsDir,
            certPath,
            keyPath,
            caCertPath;
};

static void initSettings();
static void usage();
static void parseArgs(int argc, char **argv, Args &args);
static void finalizeArgs(Args &args);
static QString getSetting(sdc::Settings::SettingsKey key, bool &configured);
static void initSettings();

static void usage()
{
    fprintf(stderr,
            "-h             print this message and exit\n"
            "-d driver      set database driver (http://qt-project.org/doc/qt-4.8/sql-driver.html)\n"
            "-b database    set database name\n"
            "-o host        set database host\n"
            "-u user        set database user\n"
            "-p password    set database password\n"
            "-r cert dir    path to ssl server directory\n"
            "-c cert path   path to ssl server certificate\n"
            "-k key path    path to ssl server key\n"
            "-a ca path     path to ca certificate\n"
            "-i init        init settings file and exit\n");
}

static void parseArgs(int argc, char **argv, Args &args)
{
    int opt;
    while ((opt = getopt(argc, argv, "r:d:b:o:u:p:c:k:aih")) != -1) {
        switch (opt) {
        case 'd':
            args.dbDriver = optarg;
            break;
        case 'b':
            args.dbDatabase = optarg;
            break;
        case 'o':
            args.dbHost = optarg;
            break;
        case 'u':
            args.dbUser = optarg;
            break;
        case 'p':
            args.dbPassword = optarg;
            break;
        case 'c':
            args.certPath = optarg;
            break;
        case 'k':
            args.keyPath = optarg;
            break;
        case 'r':
            args.certsDir = optarg;
            break;
        case 'a':
            args.caCertPath = optarg;
        case 'i':
            initSettings();
            exit(0);
        case 'h':
            usage();
            exit(0);
        default:
            break;
        }
    }
}

static void finalizeArgs(Args &args)
{
    bool configured, allConfigured = true;

    if (args.dbDriver == "") {
        args.dbDriver = getSetting(sdc::Settings::SDbDriver, configured);
        allConfigured &= configured;
    }

    if (args.dbDatabase == "") {
        args.dbDatabase = getSetting(sdc::Settings::SDbDatabase, configured);
        allConfigured &= configured;
    }

    if (args.dbHost == "") {
        args.dbHost = getSetting(sdc::Settings::SDbHost, configured);
        allConfigured &= configured;
    }

    if (args.dbUser == "") {
        args.dbUser = getSetting(sdc::Settings::SDbUser, configured);
        allConfigured &= configured;
    }

    if (args.dbPassword == "") {
        args.dbPassword = getSetting(sdc::Settings::SDbPassword, configured);
        allConfigured &= configured;
    }

    if (args.certsDir == "") {
        args.certsDir = getSetting(sdc::Settings::SCertsDir, configured);
        // this is not mandatory, so no influence on allConfigured
    }

    if (args.certPath == "") {
        args.certPath = getSetting(sdc::Settings::SCertPath, configured);
        allConfigured &= configured;
    }

    if (args.keyPath == "") {
        args.keyPath = getSetting(sdc::Settings::SKeyPath, configured);
        allConfigured &= configured;
    }

    if (args.caCertPath == "") {
        args.caCertPath = getSetting(sdc::Settings::SCertAuthPath, configured);
        allConfigured &= configured;
    }

    if (!allConfigured) {
        fprintf(stderr,
                "Could not find all the necessary settings.\n"
                "Consider using the -i option to initialize settings\n"
                "or use command line options.\n");
        exit(1);
    }

    QLOG_TRACE() << "Settings used:";
    QLOG_TRACE() << "  driver: " << args.dbDriver;
    QLOG_TRACE() << "  dbHost: " << args.dbHost;
    QLOG_TRACE() << "  db: " << args.dbDatabase;
    QLOG_TRACE() << "  user: " << args.dbUser;
    QLOG_TRACE() << "  password: " << args.dbPassword;
    QLOG_TRACE() << "  certsDefaultDir: " << args.certsDir;
    QLOG_TRACE() << "  certPath: " << args.certPath;
    QLOG_TRACE() << "  keyPath: " << args.keyPath;
    QLOG_TRACE() << "  caCertPath: " << args.caCertPath;
}

static QString getSetting(sdc::Settings::SettingsKey key, bool &configured)
{
    sdc::Settings settings;

    if (!settings.isConfigured(key)) {
        configured = false;
        return "";
    } else {
        configured = true;
        QVariant settingVal = settings.getValue(key);
        return settingVal.toString();
    }
}

static void initSettings()
{
    sdc::Settings settings;
    const int numSettings = 9;
    int numConfigured = 0;

    if (!settings.isConfigured(settings.SCertsDir)) {
        numConfigured++;
        settings.setValue(settings.SCertsDir, "misc");
    }

    if (!settings.isConfigured(settings.SCertPath)) {
        numConfigured++;
        settings.setValue(settings.SCertPath, "localhost.crt");
    }

    if (!settings.isConfigured(settings.SKeyPath)) {
        numConfigured++;
        settings.setValue(settings.SKeyPath, "localhost.key");
    }

    if (!settings.isConfigured(settings.SCertAuthPath)) {
        numConfigured++;
        settings.setValue(settings.SCertAuthPath, "ca.crt");
    }

    if (!settings.isConfigured(settings.SDbDriver)) {
        numConfigured++;
        settings.setValue(settings.SDbDriver, "QPSQL");
    }

    if (!settings.isConfigured(settings.SDbHost)) {
        numConfigured++;
        settings.setValue(settings.SDbHost, "127.0.0.1");
    }

    if (!settings.isConfigured(settings.SDbDatabase)) {
        numConfigured++;
        settings.setValue(settings.SDbDatabase, "grp22");
    }

    if (!settings.isConfigured(settings.SDbUser)) {
        numConfigured++;
        settings.setValue(settings.SDbUser, "grp22");
    }

    if (!settings.isConfigured(settings.SDbPassword)) {
        numConfigured++;
        settings.setValue(settings.SDbPassword, "se20linux901");
    }

    fprintf(stderr, "Configured %d out of %d settings with default values.\n"
            "The configuration file is stored in %s.\n",
            numConfigured,
            numSettings,
            settings.fileName().toStdString().c_str());
}

struct Application : virtual public Ice::Application {

    /**
     * Run the server
     * Returns only when the server is being exited.
     */
    virtual int run(int , char **) {
        QLOG_TRACE() << __PRETTY_FUNCTION__;

        /* The server handles adapter setup and wraps needed interfaces. */
        sdcs::Server server(communicator());
        Q_UNUSED(server);

        communicator()->waitForShutdown();

        return 0;
    }
};

int main(int argc, char **argv)
{
    /* Init QSettings. Must be done before option parsing because of -i. */
    QCoreApplication::setOrganizationName("group22");
    QCoreApplication::setApplicationName("sdcs");

    /* Parse command line arguments. */
    Args args;
    parseArgs(argc, argv, args);

    /* Create our Qt core application. */
    QCoreApplication a(argc, argv);

    /* Init logger */
    QsLogging::Logger &logger = QsLogging::Logger::instance();
    logger.setLoggingLevel(QsLogging::TraceLevel);

    QsLogging::DestinationPtr debugDestination(
        QsLogging::DestinationFactory::MakeDebugOutputDestination());
    logger.addDestination(debugDestination.get());

    /* Get remaining config values from init file */
    finalizeArgs(args);

    /* Set DB options */
    sdcs::UserDbProxy::setDriver(args.dbDriver);
    sdcs::UserDbProxy::setHost(args.dbHost);
    sdcs::UserDbProxy::setDatabase(args.dbDatabase);
    sdcs::UserDbProxy::setUser(args.dbUser);
    sdcs::UserDbProxy::setPassword(args.dbPassword);

    /* Init Ice properties. */
    Ice::PropertiesPtr props = Ice::createProperties();
    props->setProperty("Ice.Plugin.IceSSL", "IceSSL:createIceSSL");
    props->setProperty("Ice.Override.Timeout", "5000");

    if (args.certsDir != "")
        props->setProperty("IceSSL.DefaultDir", args.certsDir.toStdString());

    props->setProperty("IceSSL.CertFile", args.certPath.toStdString());
    props->setProperty("IceSSL.KeyFile", args.keyPath.toStdString());
    props->setProperty("IceSSL.CertAuthFile", args.caCertPath.toStdString());

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
