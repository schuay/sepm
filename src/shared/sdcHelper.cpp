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


#include "sdcHelper.h"

using namespace sdc;

ByteSeq sdcHelper::bStringToByteSeq(const char *arr, long len)
{
    std::vector<Ice::Byte> vec;
    for (int i = 0; i < len; i++) {
        vec.push_back(arr[i]);
    }
    return vec;
}

QByteArray sdcHelper::byteSeqToByteArray(const ByteSeq &bytes)
{
    QByteArray arr;
    arr.resize(bytes.size());
    for (unsigned int i = 0; i < bytes.size(); i++)
        arr[i] = bytes.at(i);
    return arr;
}

std::string sdcHelper::getBinaryString(const ByteSeq &bytes)
{
    std::string ret;
    std::vector<Ice::Byte>::const_iterator it;
    for (it = bytes.begin(); it < bytes.end(); it++) {
        ret.append(1, *it);
    }
    return ret;
}

ByteSeq sdcHelper::byteArraytoByteSeq(const QByteArray &arr)
{
    return bStringToByteSeq(arr.constData(), arr.size());
}

std::string sdcHelper::getServerFromID(const std::string &id)
{
    size_t separator = id.find("@");
    if (separator != std::string::npos) {
        return id.substr(separator + 1);
    }
    return "";
}

std::string sdcHelper::getNameFromID(const std::string &id)
{
    size_t separator = id.find("@");
    if (separator != std::string::npos) {
        return id.substr(0, separator);
    }
    return "";
}

bool sdcHelper::isValidID(const std::string &id)
{
    return (sdcHelper::getNameFromID(id) == "" || sdcHelper::getServerFromID(id) == "") ? false : true;
}

StringSeq sdcHelper::qStringListToStringSeq(const QStringList &s)
{
    sdc::StringSeq seq;
    for (int i = 0; i < s.size(); i++) {
        seq.push_back(s[i].toStdString());
    }
    return seq;
}
