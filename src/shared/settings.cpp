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
