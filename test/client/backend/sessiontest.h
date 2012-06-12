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
#include <QSharedPointer>
#include <QProcess>

#include "session.h"

using namespace sdcc;

class SessionTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();
    void retrieveEmptyLoglist();
    void testInitChat();
    void retrieveNonEmptyLoglist();
    void testLogout();
    void retrieveUser();
    void retrieveUserNonexistent();
    void retrieveContactListNonexistent();
    void saveRetrieveContactList();
    void saveRetrieveEmptyContactList();
    void testDeleteUserNonexistent();
    void testDeleteUserUnauthorized();
    void testDeleteUser(); /* This test must be last since it deletes our user. */

    /* This test is commented intentionally to show that this
     * behavior should actually be implemented. Currently, a repeated
     * logout just leaves the caller hanging without a reply.
     *
     * void testLogoutRepeated();
     */

private:
    QSharedPointer<Session> session;
    QProcess server;
};
