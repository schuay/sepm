#ifndef SDCHELPER_H
#define SDCHELPER_H

#include <cstddef>

#include "SecureDistributedChat.h"

#include <QByteArray>

namespace sdc
{

class sdcHelper
{
public:
    static std::string getBinaryString( const ByteSeq& bytes );
    static ByteSeq bStringToByteSeq( const char* arr, long len );
    static QByteArray byteSeqToByteArray( const ByteSeq& bytes );
    static ByteSeq byteArraytoByteSeq( const QByteArray& arr );
    static std::string getServerFromID( const std::string& id );
    static std::string getNameFromID( const std::string& id );
    static bool isValidID( const std::string& id );
};

}

#endif // SDCHELPER_H
