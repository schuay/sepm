#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QSettings>
#include <QSharedPointer>

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
     * Initializes the instance from QApplication settings.
     */
    static void init();

    /**
     * Initializes the instance from filename;
     */
    static void init(const QString &filename);

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
    struct Setting {
        Setting() { }
        Setting(const QString &key, const QVariant &defaultValue) : key(key), defaultValue(defaultValue) { }
        QString key;
        QVariant defaultValue;
    };

    static void fillConfiguration();

    /**
     * The settings object used for all other methods.
     * Must be initialized by calling init() before usage.
     */
    static QSharedPointer<QSettings> settings;

    /**
     * The configuration stores mappings between the public SettingsKey,
     * and the associated key and default value.
     */
    static QMap<SettingsKey, Setting> configuration;
};

}

#endif // SETTINGS_H
