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


#ifndef SDCHELPER_H
#define SDCHELPER_H

#include <cstddef>
#include <QByteArray>
#include <QStringList>

#include "SecureDistributedChat.h"

namespace sdc
{

class sdcHelper
{
public:
    static std::string getBinaryString(const ByteSeq &bytes);
    static ByteSeq bStringToByteSeq(const char *arr, long len);
    static QByteArray byteSeqToByteArray(const ByteSeq &bytes);
    static ByteSeq byteArraytoByteSeq(const QByteArray &arr);
    static std::string getServerFromID(const std::string &id);
    static std::string getNameFromID(const std::string &id);
    static bool isValidID(const std::string &id);
    static StringSeq qStringListToStringSeq(const QStringList &s);
};

}

#endif // SDCHELPER_H
