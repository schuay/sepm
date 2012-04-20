#include "Security.h"

#include <QByteArray>

#include "sdcHelper.h"

using namespace sdc;

Security::Security()
{
    m_rsa = false;
    m_aes = false;

    if ( QCA::isSupported( "pkey" ) &&
            QCA::PKey::supportedIOTypes().contains( QCA::PKey::RSA ) )
        m_rsa = true;
    if ( QCA::isSupported( "aes256-ofb" ) )
    {
        m_aes = true;
        cipher = new QCA::Cipher( "aes256", QCA::Cipher::OFB );
    }
}

/* ########## RSA encryption ########## */

ByteSeq Security::encryptRSA( QCA::PublicKey& key, const ByteSeq& data ) throw( SecurityException )
{
    if( !( m_rsa && m_aes ) )
        throw SecurityException( "RSA not supported" );
    if( !key.canEncrypt() )
        throw SecurityException( "supplied key can not encrypt" );

    ByteSeq symkey = genAESKey( key.maximumEncryptSize( QCA::EME_PKCS1_OAEP ) );
    ByteSeq encdata = encryptAES( symkey, data );

    QCA::SecureArray secdata = sdcHelper::byteSeqToByteArray( symkey );
    QCA::SecureArray outdata = key.encrypt( secdata, QCA::EME_PKCS1_OAEP );

    outdata.append( sdcHelper::byteSeqToByteArray( encdata ) );

    return sdcHelper::bStringToByteSeq( outdata.constData(), outdata.size() );
}

ByteSeq Security::encryptRSA( const QString& pubkeyfile, const ByteSeq& data ) throw( SecurityException )
{
    QCA::PublicKey key = QCA::PublicKey::fromPEMFile( pubkeyfile );
    return encryptRSA( key, data );
}

ByteSeq Security::encryptRSA( const ByteSeq& pubkey, const ByteSeq& data ) throw( SecurityException )
{
    QCA::PublicKey key = QCA::PublicKey::fromPEM( QString::fromStdString( sdcHelper::getBinaryString( pubkey ) ) );
    return encryptRSA( key, data );
}

/* ########## RSA decryption ########## */

ByteSeq Security::decryptRSA( QCA::PrivateKey& key, const ByteSeq& data ) throw( SecurityException )
{
    if( !( m_rsa && m_aes ) )
        throw SecurityException( "RSA not supported" );
    if( !key.canDecrypt() )
        throw SecurityException( "supplied key can not decrypt" );

    QByteArray encdata = sdcHelper::byteSeqToByteArray( data );
    QCA::SecureArray secdata = encdata.left( key.bitSize() / 8 );
    QCA::SecureArray symkey;
    ByteSeq outdata;

    if( key.decrypt( secdata, &symkey, QCA::EME_PKCS1_OAEP ) == false )
        throw SecurityException( "decryption failed" );

    outdata = decryptAES( sdcHelper::byteArraytoByteSeq( symkey.toByteArray() ),
            sdcHelper::byteArraytoByteSeq( encdata.mid( key.bitSize() / 8 ) ) );

    return outdata;
}

ByteSeq Security::decryptRSA( const QString& privkeyfile, const ByteSeq& data ) throw( SecurityException )
{
    QCA::PrivateKey key = QCA::PrivateKey::fromPEMFile( privkeyfile );
    return decryptRSA( key, data );
}

ByteSeq Security::decryptRSA( const QString& privkeyfile, const QString& passphrase, const ByteSeq& data ) throw( SecurityException )
{
    QCA::PrivateKey key = QCA::PrivateKey::fromPEMFile( privkeyfile, passphrase.toStdString().c_str() );
    return decryptRSA( key, data );
}

ByteSeq Security::decryptRSA( const ByteSeq& privkey, const ByteSeq& data ) throw( SecurityException )
{
    QCA::PrivateKey key = QCA::PrivateKey::fromPEM( QString::fromStdString( sdcHelper::getBinaryString( privkey ) ) );
    return decryptRSA( key, data );
}

/* ########## RSA sign ########## */

ByteSeq Security::signRSA( QCA::PrivateKey& key, const ByteSeq& data ) throw( SecurityException )
{
    if ( !m_rsa )
        throw SecurityException( "RSA not supported" );
    if ( !key.canSign() )
        throw SecurityException( "supplied key can not sign" );
    QCA::SecureArray secdata = sdcHelper::byteSeqToByteArray( data );
    QByteArray signature = key.signMessage( secdata, QCA::EMSA3_SHA1 );

    return sdcHelper::bStringToByteSeq( signature.constData(), signature.size() );
}

ByteSeq Security::signRSA( const QString& privkeyfile, const ByteSeq& data ) throw( SecurityException )
{
    QCA::PrivateKey key = QCA::PrivateKey::fromPEMFile( privkeyfile );
    return signRSA( key, data );
}

ByteSeq Security::signRSA( const QString& privkeyfile, const QString& passphrase, const ByteSeq& data ) throw( SecurityException )
{
    QCA::PrivateKey key = QCA::PrivateKey::fromPEMFile( privkeyfile, passphrase.toStdString().c_str() );
    return signRSA( key, data );
}

ByteSeq Security::signRSA( const ByteSeq& privkey, const ByteSeq& data ) throw( SecurityException )
{
    QCA::PrivateKey key = QCA::PrivateKey::fromPEM( QString::fromStdString( sdcHelper::getBinaryString( privkey ) ) );
    return signRSA( key, data );
}

/* ########## RSA verify ########## */

bool Security::verifyRSA( QCA::PublicKey& key, const ByteSeq& data, const ByteSeq& signature ) throw( SecurityException )
{
    if ( !m_rsa )
        throw SecurityException( "RSA not supported" );
    if ( !key.canVerify() )
        throw SecurityException( "supplied key can not verify" );
    QCA::SecureArray secdata = sdcHelper::byteSeqToByteArray( data );
    QByteArray qsignature = sdcHelper::byteSeqToByteArray( signature );

    return key.verifyMessage( secdata, qsignature, QCA::EMSA3_SHA1 );
}

bool Security::verifyRSA( const QString& pubkeyfile, const ByteSeq& data, const ByteSeq& signature ) throw( SecurityException )
{
    QCA::PublicKey key = QCA::PublicKey::fromPEMFile( pubkeyfile );
    return verifyRSA( key, data, signature );
}

bool Security::verifyRSA( const ByteSeq& pubkey, const ByteSeq& data, const ByteSeq& signature ) throw( SecurityException )
{
    QCA::PublicKey key = QCA::PublicKey::fromPEM( QString::fromStdString( sdcHelper::getBinaryString( pubkey ) ) );
    return verifyRSA( key, data, signature );
}

/* ########## RSA keys ########## */

void Security::genRSAKey( ByteSeq& pubkey, ByteSeq& privkey, int bits ) throw( SecurityException )
{
    if( !m_rsa )
        throw SecurityException( "RSA not supported" );

    QCA::PrivateKey key = QCA::KeyGenerator().createRSA( bits );
    std::string pubkey_ = key.toPublicKey().toPEM().toStdString();
    std::string privkey_ = key.toPEM().toStdString();

    pubkey = sdcHelper::bStringToByteSeq( pubkey_.data(), pubkey_.size() );
    privkey = sdcHelper::bStringToByteSeq( privkey_.data(), privkey_.size() );
}

void Security::savePrivKey( const ByteSeq& privkey, const QString& privkeyfile, const QString& passphrase ) throw( SecurityException )
{
    if ( !m_rsa )
        throw SecurityException( "RSA not supported" );

    QCA::ConvertResult r;
    QCA::PrivateKey key = QCA::PrivateKey::fromPEM( sdcHelper::getBinaryString( privkey ).data(), QCA::SecureArray(), &r );
    if( QCA::ConvertGood != r )
        throw SecurityException( "failed to read private key from input" );
    if( ! key.toPEMFile( privkeyfile, passphrase.toStdString().c_str() ) )
        throw SecurityException( "failed to save key" );
}

void Security::savePubKey( const ByteSeq& pubkey, const QString& pubkeyfile ) throw( SecurityException )
{
    if ( !m_rsa )
        throw SecurityException( "RSA not supported" );

    QCA::ConvertResult r;
    QCA::PublicKey key = QCA::PublicKey::fromPEM( sdcHelper::getBinaryString( pubkey ).data(), &r );
    if( QCA::ConvertGood != r )
        throw SecurityException( "failed to read public key from input" );
    if( ! key.toPEMFile( pubkeyfile ) )
        throw SecurityException( "failed to save key" );
}

ByteSeq Security::readPrivKey( const QString& privkeyfile, const QString& passphrase ) throw( SecurityException )
{
    if ( !m_rsa )
        throw SecurityException( "RSA not supported" );

    QCA::ConvertResult r;
    QCA::PrivateKey key = QCA::PrivateKey::fromPEMFile( privkeyfile, passphrase.toStdString().c_str(), &r );
    if( QCA::ConvertGood != r )
    {
        if( r == QCA::ErrorFile )
            throw SecurityException( "failed to read private key from file" );
        else if( r == QCA::ErrorPassphrase )
            throw SecurityException( "wrong passphrase" );
        else
            throw SecurityException( "decoding failed" );
    }
    QString data = key.toPEM();
    return sdcHelper::bStringToByteSeq( data.toStdString().c_str(), data.length() );
}

ByteSeq Security::readPubKey( const QString& pubkeyfile ) throw( SecurityException )
{
    if ( !m_rsa )
        throw SecurityException( "RSA not supported" );

    QCA::ConvertResult r;
    QCA::PublicKey key = QCA::PublicKey::fromPEMFile( pubkeyfile, &r );
    if( QCA::ConvertGood != r )
    {
        if( r == QCA::ErrorFile )
            throw SecurityException( "failed to read public key from file" );
        else
            throw SecurityException( "decoding failed" );
    }
    QString data = key.toPEM();
    return sdcHelper::bStringToByteSeq( data.toStdString().c_str(), data.length() );
}

/* ########## AES ########## */

ByteSeq Security::processAES( const ByteSeq& key, const ByteSeq& data, QCA::Direction dir ) throw( SecurityException )
{
    if( !m_aes )
        throw SecurityException( "AES not supported" );

    QByteArray outdata;
    QCA::SymmetricKey symkey( sdcHelper::byteSeqToByteArray( key ) );
    if( symkey.size() == 0 )
        throw SecurityException( "empty key supplied" );

    if ( dir == QCA::Encode )
    {
        QCA::SecureArray secdata = sdcHelper::byteSeqToByteArray( data );
        QCA::InitializationVector iv( symkey.size() );
        cipher->setup( dir, symkey, iv );
        outdata = cipher->process( secdata ).toByteArray();
        outdata = outdata.prepend( iv.toByteArray() );
    }
    else
    {
        QByteArray data_ = sdcHelper::byteSeqToByteArray( data );
        QCA::SecureArray secdata = data_.mid( symkey.size() );
        QCA::InitializationVector iv( data_.left( symkey.size() ) );
        cipher->setup( dir, symkey, iv );
        outdata = cipher->process( secdata ).toByteArray();
    }


    return sdcHelper::byteArraytoByteSeq( outdata );
}

ByteSeq Security::encryptAES( const ByteSeq& key, const ByteSeq& data ) throw( SecurityException )
{
    return processAES( key, data, QCA::Encode );
}

ByteSeq Security::decryptAES( const ByteSeq& key, const ByteSeq& data ) throw( SecurityException )
{
    return processAES( key, data, QCA::Decode );
}

/* ########## AES keys ########## */

ByteSeq Security::genAESKey( int size )
{
    if ( !m_aes )
        throw SecurityException( "AES not supported" );

    QCA::SymmetricKey key( size );
    return sdcHelper::byteArraytoByteSeq( key.toByteArray() );
}

