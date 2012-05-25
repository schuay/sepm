#ifndef _CHATLOGENTRY_H
#define _CHATLOGENTRY_H

#include <QString>
#include <QDateTime>

namespace sdcc
{

/**
 * A simple object to represent an entry in a chatlog.
 */
class ChatlogEntry
{

    friend class SessionPrivate;

public:
    const QString &getSender() const;
    const QDateTime &getTimestamp() const;
    const QString &getMessage() const;

private:
    /**
     * The id if the user, who sent the message.
     */
    QString sender;

    /**
     * The time at which the message was received.
     */
    QDateTime timestamp;

    /**
     * The message's contents.
     */
    QString message;

    ChatlogEntry(const QString &sender, long timestamp, const QString &message);
};

}

#endif /* _CHATLOGENTRY_H */
