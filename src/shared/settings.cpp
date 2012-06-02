#include "settings.h"

#include <assert.h>

#include "QsLog.h"

namespace sdc
{

/* Define the static instance. */
QSharedPointer<QSettings> Settings::settings;
QMap<Settings::SettingsKey, Settings::Setting> Settings::configuration;

void Settings::init()
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    settings = QSharedPointer<QSettings>(new QSettings());
    fillConfiguration();
}

void Settings::init(const QString &filename)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    QLOG_TRACE() << "Reading settings from" << filename;

    settings = QSharedPointer<QSettings>(new QSettings(filename, QSettings::NativeFormat));
    fillConfiguration();
}

void Settings::fillConfiguration()
{
    configuration.clear();

    configuration[CCertPath] = Setting("client/certPath", "");
    configuration[CPublicKeyPath] = Setting("client/publicKeyPath", "");
    configuration[CPrivateKeyPath] = Setting("client/privateKeyPath", "");
    configuration[CUsername] = Setting("client/username", "");
    configuration[CServer] = Setting("client/server", "");
    configuration[SCertsDir] = Setting("server/certsDir", "misc");
    configuration[SCertPath] = Setting("server/certPath", "localhost.crt");
    configuration[SKeyPath] = Setting("server/keyPath", "localhost.key");
    configuration[SCertAuthPath] = Setting("server/certAuthPath", "ca.crt");
    configuration[SListenAddress] = Setting("server/listenAddress", "0.0.0.0");
    configuration[SHostname] = Setting("server/hostname", "localhost");
    configuration[SDbHost] = Setting("server/dbHost", "localhost");
    configuration[SDbUser] = Setting("server/dbUser", "grp22");
    configuration[SDbDatabase] = Setting("server/dbDatabase", "grp22");
    configuration[SDbPassword] = Setting("server/dbPassword", "se20linux901");
    configuration[SDbDriver] = Setting("server/dbDriver", "QPSQL");
}

void Settings::create()
{
    for (int i = 0; i < static_cast<int>(SettingsKeyEnd); i++) {
        SettingsKey key = static_cast<SettingsKey>(i);
        setValue(key, getValue(key));
    }
}

void Settings::setValue(SettingsKey key, QVariant value)
{
    settings->setValue(configuration[key].key, value);
    settings->sync();
}

QVariant Settings::getValue(SettingsKey key)
{
    return settings->value(configuration[key].key, configuration[key].defaultValue);
}

}
