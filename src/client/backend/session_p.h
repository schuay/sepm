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


#ifndef _SESSION_P_H
#define _SESSION_P_H

#include "session.h"

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <QMutexLocker>

#include "QsLog.h"

#include "Security.h"

namespace sdcc
{

class SessionPrivate : public QObject, public sdc::ChatClientCallbackI
{
    Q_OBJECT

    SessionPrivate(Session *q, Ice::CommunicatorPtr c)
        : communicator(c), q_ptr(q) { }

    void initChat(const sdc::StringSeq &cUsers, const std::string &chatID,
                  const sdc::ByteSeq &sessionKeyEnc, const Ice::Current &)
    throw(sdc::ChatClientCallbackException);

    void addChatParticipant(const sdc::User &participant,
                            const std::string &chatID, const Ice::Current &)
    throw(sdc::ParticipationException);

    void removeChatParticipant(const sdc::User &participant,
                               const std::string &chatID, const Ice::Current &)
    throw(sdc::ParticipationException);

    void appendMessageToChat(const sdc::ByteSeq &message, const std::string &chatID,
                             const sdc::User &participant, const Ice::Current &)
    throw(sdc::MessageCallbackException);

    std::string echo(const std::string &message, const Ice::Current &)
    throw(sdc::SDCException);

    void runRetrieveUser(const QString &username, const QObject *id);

    void runInitChat();

    void runLogout();

    void runDeleteUser(QSharedPointer<const User> user);

    void runRetrieveContactList();

    void runRetrieveLoglist();

    void runRetrieveLog(const QDateTime &time, const QString &chat);

    void runSaveContactList(const QStringList &contactlist);

    ~SessionPrivate();

    QMutex chatsMutex;
    QMutex usersMutex;
    QMap<QString, QSharedPointer<Chat> > chats;
    QMap<QString, QSharedPointer<const User> > users;

    Ice::CommunicatorPtr communicator;
    sdc::SessionIPrx session;

    bool valid;

    QSharedPointer<const LoginUser> user;

private:
    Session *q_ptr;
    Q_DECLARE_PUBLIC(Session)

    QSharedPointer<const User> retrieveUser(const QString &username);
    QSharedPointer<const User> getUser(const QString &username);
    void leaveAllChats(void);

private slots:
    void leaveChatCompletedSlot(const QString &id);
};

}

#endif /* _SESSION_P_H */
