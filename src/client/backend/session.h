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


#ifndef _SESSION_H
#define _SESSION_H

#include <QSharedPointer>

#include "SecureDistributedChat.h"
#include "chat.h"
#include "user.h"
#include "loginuser.h"
#include "sdcHelper.h"
#include "chatlogentry.h"

namespace sdcc
{

static const int SESSION_KEY_SIZE = 256;

class SessionManager;
class SessionPrivate;

/**
 * A session is the context of a user logged into a server.
 * In principle, one may have as many sessions as one likes,
 * obtain them by calling SessionManager.login().
 * All method calls are asynchronous.
 */
class Session : public QObject
{
    Q_OBJECT

    /* Allow SessionManager to call the private constructor. */
    friend class SessionManager;

public:

    /**
     * Initialize a new chat with no other participants but the user the
     * session belongs to. New users can then be added using Chat.invite()
     * once the chat has been created successfully.
     */
    void initChat();

    /**
     * Logout from current session. All chats will be left and all sessions
     * closed. Chats created by the user will continue to exist if there is
     * at least one user left in them.
     */
    void logout();

    /**
     * Retrieve another user from the current or another server. The returned
     * User object will contain the user's public key.
     * username shall be in the form "user@server".
     */
    void retrieveUser(const QString &username, const QObject *id);

    /**
     * Delete the given user from the server.
     */
    void deleteUser(QSharedPointer<const User> user);

    /**
     * Retrieves the contact list from the server.
     */
    void retrieveContactList();

    /**
     * Retrieves the chatlog list from the server.
     */
    void retrieveLoglist();

    /**
     * Retrieves the log of the specified chat from the server.
     */
    void retrieveLog(const QPair<QDateTime, QString> &spec);
    void retrieveLog(const QDateTime &time, const QString &chat);

    /**
     * Saves the contact list on the server
     */
    void saveContactList(const QStringList &contactlist);

    /**
     * Retrieve the user of this session.
     */
    const QSharedPointer<const LoginUser> getUser() const;

    /**
     * True if neither logout(), nor deleteUser() with the sessions user has
     * been called, false otherwise.
     */
    bool isValid() const;

signals:

    /**
     * Received an invitation to a chat from the server.
     */
    void invitationReceived(QSharedPointer<Chat> chat);

    /**
     * A chat has been initialized.
     */
    void initChatCompleted(QSharedPointer<Chat> chat, bool success,
                           const QString &msg);

    /**
     * This object is invalid once logoutCompleted() has completed.
     */
    void logoutCompleted(bool success, const QString &msg);

    /**
     * A user has been retrieved from the server.
     */
    void retrieveUserCompleted(QSharedPointer<const User> user, const QObject *id,
                               bool success, const QString &msg);

    /**
     * A user has been deleted from the server.
     */
    void deleteUserCompleted(bool success, const QString &msg);

    /**
     * The contact list has been retrieved from the server.
     */
    void retrieveContactListCompleted(const QStringList &list,
                                      bool success, const QString &msg);

    /**
     * The chatlog list has been retrieved from the server.
     */
    void retrieveLoglistCompleted(const QList<QPair<QDateTime, QString> > &list,
                                  bool success, const QString &msg);

    /**
     * The requested chatlog has been retrieved from the server.
     */
    void retrieveLogCompleted(const QList<ChatlogEntry> &list,
                              bool success, const QString &msg);

    /**
     * The contact list has been saved on the server
     */
    void saveContactListCompleted(bool success, const QString &msg);



private:
    /* Prevent unintended construction of instances by user. */
    Session(QSharedPointer<const LoginUser> user, const QString &pwd,
            sdc::AuthenticationIPrx auth);
    Session(const Session &);

    SessionPrivate *d_ptr;
    Q_DECLARE_PRIVATE(Session)
};

}

#endif /* _SESSION_H */
