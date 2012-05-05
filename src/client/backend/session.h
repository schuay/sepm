#ifndef _SESSION_H
#define _SESSION_H

#include <QSharedPointer>

#include "SecureDistributedChat.h"
#include "chat.h"
#include "user.h"

namespace sdcc
{

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

    virtual ~Session();

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
    void retrieveUser(const QString &username);

signals:

    /**
     * Received an invitiation to a chat from the server.
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
    void retrieveUserCompleted(const User &user, bool success,
                               const QString &msg);

private:
    /* Prevent unintended construction of instances by user. */
    Session(const User &user, const QString &pwd, sdc::AuthenticationIPrx auth);
    Session(const Session &);

    SessionPrivate *d_ptr;
    Q_DECLARE_PRIVATE(Session)
};

}

#endif /* _SESSION_H */
