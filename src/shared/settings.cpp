#include "settings.h"

#include <assert.h>

namespace sdc
{

/* Define the static instance. */
QSharedPointer<QSettings> Settings::settings;

void Settings::init()
{
    settings = QSharedPointer<QSettings>(new QSettings());
}

void Settings::init(const QString &filename)
{
    settings = QSharedPointer<QSettings>(new QSettings(filename, QSettings::NativeFormat));
}

QString Settings::getKey(SettingsKey key)
{
    switch (key) {
    case CCertPath:
        return "client/certPath";
    case CPublicKeyPath:
        return "client/publicKeyPath";
    case CPrivateKeyPath:
        return "client/privateKeyPath";
    case CUsername:
        return "client/username";
    case CServer:
        return "client/server";
    case SCertsDir:
        return "server/certsDir";
    case SCertPath:
        return "server/certPath";
    case SKeyPath:
        return "server/keyPath";
    case SCertAuthPath:
        return "server/certAuthPath";
    case SHostname:
        return "server/hostname";
    case SDbHost:
        return "server/dbHost";
    case SDbUser:
        return "server/dbUser";
    case SDbDatabase:
        return "server/dbDatabase";
    case SDbPassword:
        return "server/dbPassword";
    case SDbDriver:
        return "server/dbDriver";

    default:
        assert(0);
    }
}

void Settings::setValue(SettingsKey key, QVariant value)
{
    settings->setValue(getKey(key), value);
    settings->sync();
}

QVariant Settings::getValue(SettingsKey key)
{
    return settings->value(getKey(key));
}

bool Settings::isConfigured(SettingsKey key)
{
    return settings->contains(getKey(key));
}

QString Settings::fileName()
{
    return settings->fileName();
}

}
