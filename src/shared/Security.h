#ifndef _SECURITY_H_
#define _SECURITY_H_

#include <exception>

#include <QString>
#include <QtCrypto>

#include "SecureDistributedChat.h"

namespace sdc
{

class SecurityException : public std::exception
{
public:
    SecurityException() { };
    SecurityException(const std::string& msg): m_msg(msg) { };
    ~SecurityException() throw() { };
    const char* what() const throw() { return m_msg.c_str(); };
private:
    std::string m_msg;
};

class Security
{
public:
    Security();


    /**
     * True if RSA is supported.
     */
    bool supportsRSA() const { return m_rsa; };

    /**
     * True if AES is supported.
     */
    bool supportsAES() const { return m_aes; };
    
    /**
     * Encrypt a ByteSeq with RSA.
     *
     * @param pubkeyfile The file with the public RSA key.
     * @param data The data to encrypt.
     * @return The encrypted ByteSeq.
     */
    ByteSeq encryptRSA( const QString& pubkeyfile, const ByteSeq& data ) throw( SecurityException );

    /**
     * Encrypt a ByteSeq with RSA.
     *
     * @param pubkey The public RSA key.
     * @param data The data to encrypt.
     * @return The encrypted ByteSeq.
     */
    ByteSeq encryptRSA( const ByteSeq& pubkey, const ByteSeq& data ) throw( SecurityException );


    /**
     * Decrypt a ByteSeq with RSA.
     *
     * @param privkeyfile The file with the private RSA key.
     * @param data The encrypted data.
     * @return The decrypted data.
     */
    ByteSeq decryptRSA( const QString& privkeyfile, const ByteSeq& data ) throw( SecurityException );

    /**
     * Decrypt a ByteSeq with RSA.
     *
     * @param privkeyfile The file with the private RSA key.
     * @param passphrase The passphrase for the key file.
     * @param data The encrypted data.
     * @return The decrypted data.
     */
    ByteSeq decryptRSA( const QString& privkeyfile, const QString& passphrase, const ByteSeq& data ) throw( SecurityException );

    /**
     * Decrypt a ByteSeq with RSA.
     *
     * @param privkey The private RSA key.
     * @param data The encrypted data.
     * @return The decrypted data.
     */
    ByteSeq decryptRSA( const ByteSeq& privkey, const ByteSeq& data ) throw( SecurityException );


    /**
     * Sign a ByteSeq with RSA.
     *
     * @param privkeyfile The file with the private RSA key.
     * @param data The data to sign.
     * @return The signature.
     */
    ByteSeq signRSA( const QString& privkeyfile, const ByteSeq& data ) throw( SecurityException );

    /**
     * Sign a ByteSeq with RSA.
     *
     * @param privkeyfile The file with the private RSA key.
     * @param passphrase The passphrase for the key file.
     * @param data The data to sign.
     * @return The signature.
     */
    ByteSeq signRSA( const QString& privkeyfile, const QString& passphrase, const ByteSeq& data ) throw( SecurityException );

    /**
     * Sign a ByteSeq with RSA.
     *
     * @param privkey The private RSA key.
     * @param data The data to sign.
     * @return The signature.
     */
    ByteSeq signRSA( const ByteSeq& privkey, const ByteSeq& data ) throw( SecurityException );


    /**
     * Verify the RSA signature of a ByteSeq.
     * 
     * @param pubkeyfile The file with the public RSA key.
     * @param data The data for which to verify the signature.
     * @param signature The signature.
     * @return True or false.
     */
    bool verifyRSA( const QString& pubkeyfile, const ByteSeq& data, const ByteSeq& signature ) throw( SecurityException );

    /**
     * Verify the RSA signature of a ByteSeq.
     * 
     * @param pubkey The public RSA key.
     * @param data The data for which to verify the signature.
     * @param signature The signature.
     * @return True or false.
     */
    bool verifyRSA( const ByteSeq& pubkey, const ByteSeq& data, const ByteSeq& signature ) throw( SecurityException );


    /**
     * Encrypt a ByteSeq with AES.
     *
     * @param key The key.
     * @param data The data to encrypt.
     * @return The encrypted data.
     */
    ByteSeq encryptAES( const ByteSeq& key, const ByteSeq& data ) throw( SecurityException );

    /**
     * Decrypt a ByteSeq with AES.
     *
     * @param key The key.
     * @param data The encrypted data.
     * @return The decrypted data.
     */
    ByteSeq decryptAES( const ByteSeq& key, const ByteSeq& data ) throw( SecurityException );


    /**
     * Generate a RSA key pair.
     *
     * @param pubkey The ByteSeq to save the public key in.
     * @param privkey The ByteSeq to save the private key in.
     * @param bits The key size to use.
     */
    void genRSAKey( ByteSeq& pubkey, ByteSeq& privkey, int bits = 2048 ) throw( SecurityException );

    /**
     * Save a private key in a file.
     *
     * @param privkey The private key.
     * @param privkeyfile The file to save the key in.
     * @param passphrase The optional passphrase for the key file.
     */
    void savePrivKey( const ByteSeq& privkey, const QString& privkeyfile, const QString& passphrase = QString() ) throw( SecurityException );

    /**
     * Save a public key in a file.
     *
     * @param pubkey The public key.
     * @param pubkeyfile The file to save the key in.
     */
    void savePubKey( const ByteSeq& pubkey, const QString& pubkeyfile ) throw( SecurityException );

    /**
     * Read a private key from a file.
     *
     * @param privkeyfile The file to read the key from.
     * @passphrase The optional passphrase for the key file.
     */
    ByteSeq readPrivKey( const QString& privkeyfile, const QString& passphrase = QString() ) throw( SecurityException );

    /**
     * Read a public key from a file.
     *
     * @param pubkeyfile The file to save the key in.
     */
    ByteSeq readPubKey( const QString& pubkeyfile ) throw( SecurityException );


    /**
     * Generate a random AES key.
     *
     * @param size The key size.
     */
    ByteSeq genAESKey( int size );

protected:
    /**
     * Encrypt a ByteSeq with RSA.
     *
     * @param key The public RSA key.
     * @param data The data to encrypt.
     * @return The encrypted ByteSeq.
     */
    ByteSeq encryptRSA( QCA::PublicKey& key, const ByteSeq& data ) throw( SecurityException );

    /**
     * Decrypt a ByteSeq with RSA.
     *
     * @param key The private RSA key.
     * @param data The encrypted data.
     * @return The decrypted data.
     */
    ByteSeq decryptRSA( QCA::PrivateKey& key, const ByteSeq& data ) throw( SecurityException );

    /**
     * Sign a ByteSeq with RSA.
     *
     * @param key The private RSA key.
     * @param data The data to sign.
     * @return The signature.
     */
    ByteSeq signRSA( QCA::PrivateKey& key, const ByteSeq& data ) throw( SecurityException );

    /**
     * Verify the RSA signature of a ByteSeq.
     *
     * @param key The public RSA key.
     * @param data The data for which to verify the signature.
     * @param signature The signature.
     * @return True or false.
     */
    bool verifyRSA( QCA::PublicKey& key, const ByteSeq& data, const ByteSeq& signature ) throw( SecurityException );


    /**
     * En-/Decrypt data with AES.
     *
     * @param key The key.
     * @param data The data to process.
     * @param dir The direction whether to en- or decrypt.
     * @return The processed data.
     */
    ByteSeq processAES( const ByteSeq& key, const ByteSeq& data, QCA::Direction dir ) throw( SecurityException );

private:
    QCA::Initializer init;
    bool m_rsa;
    bool m_aes;
    QCA::Cipher* cipher;
};

}

#endif  // _SECURITY_H_

