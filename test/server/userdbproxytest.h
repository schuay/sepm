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


#include <QObject>
#include <QSqlQuery>

#include "SecureDistributedChat.h"

class UserDbProxyTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testConnection();

    void testRetrieveUser();
    void testRetrieveNonexistentUser();

    void testRetrieveContactList();
    void testRetrieveNonexistentContactList();
    void testSaveContactList();
    void testSaveContactListAgain();

    void testRetrieveLoglist();
    void testRetrieveLoglistEmpty();
    void testRetrieveLog();
    void testRetrieveLogNonexistent();
    void testSaveLog();
    void testSaveLogDuplicate();

    void testDeleteExistentUser();
    void testDeleteNonexistentUser();

    void testCreateUser();
    void testCreateExistingUser();

    void cleanupTestCase();

private:
    sdc::User user1;
    QByteArray hash1;
    QByteArray salt1;

    QSqlDatabase db;
};
