#include "sdcHelper.h"

using namespace sdc;

ByteSeq sdcHelper::bStringToByteSeq( const char* arr, long len )
{
    std::vector<Ice::Byte> vec;
    for( int i = 0; i < len; i++ )
    {
        vec.push_back( arr[i] );
    }
    return vec;
}

QByteArray sdcHelper::byteSeqToByteArray( const sdc::ByteSeq& bytes )
{
    QByteArray arr;
    arr.resize( bytes.size() );
    for( unsigned int i = 0; i < bytes.size(); i++ )
        arr[i] = bytes.at( i );
    return arr;
}

std::string sdcHelper::getBinaryString( const sdc::ByteSeq& bytes )
{
    std::string ret;
    std::vector<Ice::Byte>::const_iterator it;
    for( it = bytes.begin(); it < bytes.end(); it++ )
    {
        ret.append( 1, *it );
    }
    return ret;
}

ByteSeq sdcHelper::byteArraytoByteSeq( const QByteArray& arr )
{
    return bStringToByteSeq( arr.constData(), arr.size() );
}

std::string sdcHelper::getServerFromID( const std::string& id )
{
    size_t seperator = id.find( "@" );
    if( seperator != std::string::npos )
    {
        return id.substr( seperator + 1 );
    }
    return "";
}

std::string sdcHelper::getNameFromID( const std::string& id )
{
    size_t seperator = id.find( "@" );
    if( seperator != std::string::npos )
    {
        return id.substr( 0, seperator );
    }
    return "";
}

bool sdcHelper::isValidID( const std::string& id )
{
    return ( sdcHelper::getNameFromID( id ) == "" || sdcHelper::getServerFromID( id ) == "" ) ? false : true;
}
