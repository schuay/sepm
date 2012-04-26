/**
 * @file sessionmanager.h
 *
 * Guidelines:
 *
 * Every function in the backend is to be called asynchronous,
 * that is it returns immediately and the result will be communicated
 * using a signal. Make sure you're subscribed to the signal if you
 * are interested in it.
 * Functions will execute in their own threads, so the caller does not
 * need to care about blocking. One has to pay attention not to confuse
 * several result signals, as they don't always carry information on which
 * function call they belonged to. If there is potential for confusion
 * then the caller is not allowed to call the same function again before
 * the result is received.
 *
 * Usually the signal will be named functionNameCompleted, as in
 *   function:  SessionManager.login()
 *   signal:    SessionManager.loginCompleted()
 *
 * Functions return void. Result signals carry first their data and then
 * two standardized parameters indicating success and an error message (if
 * there is one).
 *   example:
 *      void loginCompleted(QSharedPointer<Session> session,
 *                          bool success,
 *                          const QString &msg);
 *
 * If success is false, then msg will always carry an error message. If success
 * is true then the content of msg is undefined.
 */

#include <QSharedPointer>

namespace sdcc
{

class User;
class Session;

/**
 * Top level object for the client.
 * Normally, there will only be one session manager.
 * Maybe it should be a singleton?
 */
class SessionManager : public QObject
{
    Q_OBJECT

public:

    /**
     * Calls echo on the server to check its availability.
     * serverCertPath is the local path to the server's public key.
     */
    static void testConnection(const QString &serverName,
                               const QString &serverCertPath);

    /**
     * Login a user.
     * serverCertPath is the local path to the server's public key.
     */
    static void login(const QString &serverName,
                      const QString &serverCertPath, const User &usr,
                      const QString &pwd);

    /**
     * Register a new user on the server.
     */
    static void registerUser(const QString &serverName,
                             const QString &serverCertPath, const User &usr,
                             const QString &pwd);

signals:

    void testConnectionCompleted(bool success, const QString &msg);

    /**
     * Login has completed.
     * session handles all further communication with the server.
     */
    void loginCompleted(QSharedPointer<Session> session, bool success,
                        const QString &msg);

    void registerCompleted(bool success, const QString &msg);
};

}
