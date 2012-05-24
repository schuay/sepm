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
        CPrivateKeyPath,
        CUsername,
        CServer,
        CLIENT_END,
        SCertsDir,
        SCertPath,
        SKeyPath,
        SCertAuthPath,
        SHostname,
        SDbHost,
        SDbUser,
        SDbDatabase,
        SDbPassword,
        SDbDriver,
        SERVER_END
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

    /**
     * Returns true if the specified key is found in the settings.
     */
    static bool isConfigured(SettingsKey key);

    /**
     * Returns the configuration file name and path.
     */
    static QString fileName();

private:
    static QString getKey(SettingsKey key);
};

}

#endif // SETTINGS_H
