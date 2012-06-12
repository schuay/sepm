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


#ifndef INTERSERVER_H
#define INTERSERVER_H

#include "SecureDistributedChat.h"

namespace sdcs
{

class InterServer : public sdc::InterServerI
{
public:
    /**
     * Retrieve the User object of a user on another server.
     */
    sdc::User retrieveUser(const std::string &userID, const Ice::Current &)
    throw(sdc::InterServerException);

    /**
     * Invite a user on another server to a chat (forward invite).
     * This is called when the (remote) caller is inviting someone to a callee-local chat.
     */
    void invite(const sdc::User &participant,
                const std::string &chatID,
                const sdc::ByteSeq &key,
                const Ice::Current &)
    throw(sdc::InterServerException);

    /**
     * Send a message to a chat which was initialized on
     * another server. It is important to notice the distinction
     * on forwarding calls to another server because the chat
     * is on another server or because the user is on another
     * server.
     * Caller-local user, callee-local chat.
     */
    void sendMessage(const sdc::User &sender,
                     const sdc::ByteSeq &message,
                     const std::string &chatID,
                     const Ice::Current &)
    throw(sdc::InterServerException);

    /**
     * The participant leaves a chat on another server.
     * Caller-local user, callee-local chat.
     */
    void leaveChat(const sdc::User &participant,
                   const std::string &chatID,
                   const Ice::Current &)
    throw(sdc::InterServerException);

    /**
     * Command a server to relay an initChat request to a client.
     * This is called when the (remote) caller is inviting a callee-local user to a chat.
     */
    void clientInitChat(const sdc::User &client,
                        const sdc::StringSeq &users,
                        const std::string &chatID,
                        const sdc::ByteSeq &key,
                        const Ice::Current &)
    throw(sdc::ChatException);

    /**
     * Command a server to relay an addChatParticipant request to a client.
     * Caller-local chat, callee-local user.
     */
    void clientAddChatParticipant(const sdc::User &client,
                                  const sdc::User &participant,
                                  const std::string &chatID,
                                  const Ice::Current &)
    throw(sdc::ParticipationException);

    /**
     * Command a server to relay an removeChatParticipant request to a client.
     * Caller-local chat, callee-local user.
     */
    void clientRemoveChatParticipant(const sdc::User &client,
                                     const sdc::User &participant,
                                     const std::string &chatID,
                                     const Ice::Current &)
    throw(sdc::ParticipationException);

    /**
     * Command a server to relay an appendMessage request to a client.
     * Caller-local chat, callee-local user.
     */
    void clientAppendMessageToChat(const sdc::User &client,
                                   const sdc::ByteSeq &message,
                                   const std::string &chatID,
                                   const sdc::User &participant,
                                   const Ice::Current &)
    throw(sdc::MessageCallbackException);
};
}

#endif
