#include "settings.h"

#include <assert.h>

namespace sdc
{

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
    QSettings settings;
    settings.setValue(getKey(key), value);
    settings.sync();
}

QVariant Settings::getValue(SettingsKey key)
{
    QSettings settings;
    return settings.value(getKey(key));
}

bool Settings::isConfigured(SettingsKey key)
{
    QSettings settings;
    return settings.contains(getKey(key));
}

QString Settings::fileName()
{
    QSettings settings;
    return settings.fileName();
}

}
