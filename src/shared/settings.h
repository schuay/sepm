/* *************************************************************************
 *  Copyright 2012 TU VIENNA SEPM GROUP 22                                 *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ************************************************************************* */


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
        SCertsDir,
        SCertPath,
        SKeyPath,
        SCertAuthPath,
        SHostname,
        SListenAddress,
        SDbHost,
        SDbUser,
        SDbDatabase,
        SDbPassword,
        SDbDriver,
        SettingsKeyEnd
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
     * Creates the settings file and initializes it with default values.
     * If the file already exists, adds keys that don't exist in the file
     * with default values.
     */
    static void create();

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
