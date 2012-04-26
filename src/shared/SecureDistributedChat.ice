#ifndef SDC_ICE
#define SDC_ICE

// This is important for gcc 4.6
[["cpp:include:cstddef"]]

#include <Ice/Identity.ice>

module sdc {
    // ------------------- CONSTANTS    -------------------
    const int port = 1337;   /**< The constant port used by all servers. */

    // ------------------- DATATYPES    -------------------
    sequence<byte> ByteSeq;     /**< A collection of bytes. Most often used for encapsulating encrypted traffic. */
    sequence<string> StringSeq; /**< A collection of strings. */

    // ------------------- EXCEPTIONS   -------------------
    // Parent exception of all exceptions.
    exception SDCException {
        string what;
    };

    //Authentication Exceptions
    exception AuthenticationException extends SDCException{ };

    //Session Exceptions
    exception SessionException extends SDCException{ };
    exception UserHandlingException extends SessionException{ };
    exception MessageException extends SessionException{ };
    exception LogException extends SessionException{ };
    exception ContactException extends SessionException{ };

    // Callback Exceptions
    exception ChatClientCallbackException extends SDCException{ };
    exception ChatException extends ChatClientCallbackException { };
    exception ParticipationException extends ChatClientCallbackException { };
    exception MessageCallbackException extends ChatClientCallbackException { };

    // InterServer Exceptions
    exception InterServerException extends SDCException{ };

    // ------------------- FORWARD DECLARATION  -----------
    interface ChatClientCallbackI;

    // ------------------- STRUCTS      -------------------
    // A capsulated user.
    struct User{
        string ID;         /**< The ID of the user. Most likely in the format of username@server */
        ByteSeq publicKey; /**< The user public key. We provided a script for generating one. */
    };

    sequence<User> ContactList; /**< Collection for the contact list. */

    // A capsulated log message.
    struct LogMessage{
        string senderID;     /**< The ID of the user who sent the message. */
        long timestamp;      /**< The timestamp of message creation. */
        string message;      /**< The message that was sent. */
    };

    sequence<LogMessage> Chatlog; /**< Collection for messages of a chat. */

    // A capsulated entry in a chat log.
    struct ChatlogEntry{
        string chatID;       /**< The ID of the chat the log belongs to. */
        long timestamp;      /**< The timestamp of the moment the chatlog was saved. */
    };

    sequence<ChatlogEntry> Loglist; /**< A collection of logs. */

    // A capsulated signed encrypted content.
    struct SecureContainer{
        ByteSeq data;        /**< Encrypted content. */
        ByteSeq signature;   /**< The signature for the _unencrypted_ content. */
    };

    // ------------------- INTERFACES   -------------------
    /** A session is generated by AuthenticationI and represents the main
     *  outgoing communication channel from a client to its home server. */
    interface SessionI{
        // Logout the user this session belongs to.
        void logout() throws UserHandlingException;

        /** Retrieve the User object of the user with ID userID.
         *
         *  If the requested user is not at the clients home server,
         *  the request is forwarded to the home server
         *  of that client.
        *  The returned User object contains the key necessary for
        	  *  sending that user encrypted content. */
        User retrieveUser(
            string userID
        ) throws UserHandlingException, InterServerException;

        /** Initialize a new chat and return the according chat ID.
         *  The client will very likely want to generate a session key
         *  for the new chat when calling initChat(). See invite(). */
        string initChat() throws SessionException;

        /** Leave the chat with ID chatID.
         *
         *  If the chat is not at the clients home server,
         *  the request is forwarded to the home server of the chat.
        *  The receiving server will want to forward the news
        	  *  to all participants of that chat. */
        void leaveChat(
            string chatID
        ) throws SessionException, InterServerException;

        /** Invite the user participant to the chat chatID.
         *  All communication in that chat is encrypted using
         *  sessionKey.
         *
         *  If the user being invited is not at the clients
         *  home server, the invite is forwarded to the home server
         *  of that respective client. */
        void invite(
            User participant, /**< User being invited to the chat. */
            string chatID,
            /** The session key must be generated by the client that
             *  initialized the chat in the first place. That key is
             *  shared among all participants of the chat and is used
             *  to encrypt and decrypt messages in this chat
             *  accordingly. */
            ByteSeq sessionKey
        ) throws UserHandlingException, InterServerException;

        /** Send a message to a chat.
         *
         *  If the message is sent to a chat that is not managed
         *  by the clients home server, the message is forwarded. */
        void sendMessage(
            ByteSeq message,
            string chatID
        ) throws MessageException, InterServerException;

        /** Delete a user. This will most likely be restricted to
         *  the use of clients with administrative privileges. */
        void deleteUser(
            User participant
        ) throws UserHandlingException;

        /** Save a log of a chat. The log is encrypted and signed,
         *  see the documentation of SecureContainer for
         *  more information.
         *  Keep in mind that the user the log belongs to
         *  is identified by the session itself. */
        void saveLog(
            string chatID,
            long timestamp,
            SecureContainer log
        ) throws LogException;

        // Retrieve all logs of the user the session belongs to.
        Loglist retrieveLoglist( )
        throws LogException;

        // Retrieve a signed and encrypted log of a chat.
        SecureContainer retrieveLog(
            string chatID,
            long timestamp
        ) throws LogException;

        /** Save the contact list of a user. The contact list is
         *  encrypted and signed. See the documentation of
         *  SecureContainer for more information.
         *  Keep in mind that the user the contact list belongs to
         *  is identified by the session itself. */
        void saveContactList(
            SecureContainer contactList
        ) throws ContactException;

        //Retrieve the signed and encrypted contact list of a user.
        SecureContainer retrieveContactList() throws ContactException;
    };

    /** AuthenticationI is a session factory. It handles authentication and
     *  generated sessions for valid users. Furthermore it is the first
     *  object a client will retrieve from a server. */
    interface AuthenticationI{
        // Register a new user.
        void registerUser(
            User participant,
            /** The password is sent in cleartext, but over a
             *  communication channel using SSL!
             *  The server should not save the cleartext password
             *  to the database, instead save it hashed & salted.
             */
            string pwd
        ) throws AuthenticationException;

        /** Login a registered user and retrieve a session for
         *  communicating with the server.
         *  The server checks the password before testing
        *  the ClientCallback the client sent by calling
        	  *  the echo() method there.
        	  *  This means that a login with an invalid callback
        	  *  object will be rejected.
                      */
        SessionI* login(
            User participant, /**< User logging in. */
            /** The password is sent in cleartext, but over a
             *  communication channel using SSL!
             *  The server should hash & salt it and compare it
             *  with the entry saved in the database during
             *  registration.
             */
            string pwd,
            /** This identifies the client for using a callback,
             *  which is used by the server to communicate with its
             *  clients. */
            Ice::Identity ident
        ) throws AuthenticationException;

        /** Returns the received message without modification.
         *  Can be used for checking availability of a server. */
        string echo(
            string message
        ) throws SDCException;
    };

    /** This interface is used by the server to communicate with its
     *  clients. The server will likely save callback objects of its clients
     *  for continued communication. */
    interface ChatClientCallbackI{
        /** Initialize a new chat at the client. This might be
         *  called by the server if it receives an invitation for
         *  the client. */
        void initChat(
            StringSeq users, /**< Participants of the chat. */
            string chatID,
            ByteSeq key /**< Session Key that is used by the chat. */
        ) throws ChatException;

        /** Add a user to a chat. This is called when a
         *  participant joins a chat. */
        void addChatParticipant(
            User participant,
            string chatID
        ) throws ParticipationException;

        /** Remove a user from a chat. This is called if a user of
         *  a chat in which another user is participating leaves.
         *  For example: User A participates in chat X, Y, Z.
         *  If user A leaves, all participants of chat X, Y and Z
         *  must be notified. */
        void removeChatParticipant(
            User participant,
            string chatID
        ) throws ParticipationException;

        /** Append a message to a chat.
        *  The distinction between sendMessage and this method
        	  *  is, that sendMessage is the action of sending a message,
        	  *  and appendMessageToChat the call to the delegates
        	  *  (participants of the chat, callback objects) notifying
        	  *  them of that action. */
        void appendMessageToChat(
            ByteSeq message,
            string chatID,
            User participant /**< Sender of the message. */
        ) throws MessageCallbackException;


        /** Returns the received message without modification.
        *  Is used by the server to check if the client sent
        	  *  a valid callback object. */
        string echo(
            string message
        ) throws SDCException;
    };

    /** InterServer is used to communicate between servers. Most often
     *  this is used when clients with different home servers communicate
     *  with each other. */
    interface InterServerI{
        // Retrieve the User object of a user on another server.
        User retrieveUser(
            string userID
        ) throws InterServerException;

        // Invite a user on another server to a chat (forward invite).
        void invite(
            User participant,
            string chatID,
            ByteSeq key /**< Shared session key of the chat. */
        ) throws InterServerException;

        /** Send a message to a chat which was initialized on
         *  another server. It is important to notice the distinction
         *  on forwarding calls to another server because the chat
         *  is on another server or because the user is on another
         *  server. */
        void sendMessage(
            User sender,
            ByteSeq message,
            string chatID
        ) throws InterServerException;

        // The participant leaves a chat on another server.
        void leaveChat(
            User participant,
            string chatID
        ) throws InterServerException;

        // Command a server to relay an initChat request to a client.
        void clientInitChat(
            User client, /**< Client to be notified. */
            StringSeq users, /**< Participants of the chat. */
            string chatID,
            ByteSeq key /**< Session key of the chat. */
        ) throws ChatException;

        // Command a server to relay an addChatParticipant request to a client.
        void clientAddChatParticipant(
            User client, /**< Client to be notified. */
            User participant, /**< Participant to be removed. */
            string chatID
        ) throws ParticipationException;

        // Command a server to relay an removeChatParticipant request to a client.
        void clientRemoveChatParticipant(
            User client, /**< Client to be notified. */
            User participant, /**< Participant to be removed. */
            string chatID
        ) throws ParticipationException;

        // Command a server to relay an appendMessage request to a client.
        void clientAppendMessageToChat(
            User client, /**< Client to be notified. */
            ByteSeq message,
            string chatID,
            User participant /**< Participant that sent the message. */
        ) throws MessageCallbackException;
    };
};

#endif
