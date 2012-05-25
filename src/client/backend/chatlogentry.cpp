#include "chatlogentry.h"

namespace sdcc
{

ChatlogEntry::ChatlogEntry(const QString &sender, long timestamp, const QString &message)
    : sender(sender), timestamp(QDateTime::fromTime_t(timestamp)),
      message(message) {}

const QString &ChatlogEntry::getSender() const
{
    return sender;
}

const QDateTime &ChatlogEntry::getTimestamp() const
{
    return timestamp;
}

const QString &ChatlogEntry::getMessage() const
{
    return message;
}

}
