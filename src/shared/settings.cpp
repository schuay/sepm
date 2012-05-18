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
    case SCertPath:
        return "server/certPath";
    case SCertPrivPath:
        return "server/certPrivPath";
    case SHostname:
        return "server/hostname";
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

}
