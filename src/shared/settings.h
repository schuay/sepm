#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QSettings>

namespace sdc
{

/**
 * Manages storage and retrieval of settings.
 */
class Settings
{
public:

    /**
     * All settings values are accessed through values of this enum.
     * Client settings are prefixed by a 'C', server settings by an 'S'.
     */
    enum SettingsKey {
        CCertPath,
        CPublicKeyPath,
        CPrivateKeyPath
    };

    /**
     * Reads the settings file and returns the requested value.
     */
    static QVariant getValue(SettingsKey key);

    /**
     * Stores the value to the settings file with the given key.
     * When this function completes successfully, the file has been saved.
     */
    static void setValue(SettingsKey key, QVariant value);

private:
    static QString getKey(SettingsKey key);
};

}

#endif // SETTINGS_H
