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
