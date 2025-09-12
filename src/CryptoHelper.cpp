#include "CryptoHelper.h"
#include <cryptopp/osrng.h>
#include <cryptopp/secblock.h>
#include <cryptopp/files.h>
#include <cryptopp/rsa.h>
#include <cryptopp/filters.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/oaep.h>
#include <cryptopp/gcm.h>
#include <cryptopp/aes.h>
#include <stdexcept>
#include <string>

using namespace CryptoPP;

static void appendUint32BE(std::string &out, uint32_t v) {
    char b[4];
    b[0] = static_cast<char>((v >> 24) & 0xff);
    b[1] = static_cast<char>((v >> 16) & 0xff);
    b[2] = static_cast<char>((v >> 8) & 0xff);
    b[3] = static_cast<char>(v & 0xff);
    out.append(b, 4);
}
static uint32_t readUint32BE(const unsigned char* p) {
    return (uint32_t(p[0])<<24) | (uint32_t(p[1])<<16) | (uint32_t(p[2])<<8) | uint32_t(p[3]);
}

void CryptoHelper::GenerateRSAKeys(const std::string& privPath, const std::string& pubPath, unsigned int bits) {
    AutoSeededRandomPool rng;
    RSA::PrivateKey priv;
    priv.GenerateRandomWithKeySize(rng, bits);
    RSA::PublicKey pub(priv);

    FileSink fs1(privPath.c_str());
    priv.DEREncode(fs1);
    fs1.MessageEnd();

    FileSink fs2(pubPath.c_str());
    pub.DEREncode(fs2);
    fs2.MessageEnd();
}

std::string CryptoHelper::EncryptForPeer(const std::string& plaintext, const std::string& peerPublicKeyPath) {
    AutoSeededRandomPool prng;

    RSA::PublicKey pub;
    {
        FileSource fs(peerPublicKeyPath.c_str(), true);
        pub.BERDecode(fs);
    }

    const size_t keyLen = 32; // AES-256
    const size_t ivLen  = 12; // AES-GCM recommended IV size
    SecByteBlock sessionKey(keyLen);
    prng.GenerateBlock(sessionKey, sessionKey.size());
    SecByteBlock iv(ivLen);
    prng.GenerateBlock(iv, iv.size());

    std::string cipherWithTag;
    {
        GCM<AES>::Encryption enc;
        enc.SetKeyWithIV(sessionKey, sessionKey.size(), iv, iv.size());
        AuthenticatedEncryptionFilter ef(enc, new StringSink(cipherWithTag));
        ef.ChannelPut(DEFAULT_CHANNEL, reinterpret_cast<const byte*>(plaintext.data()), plaintext.size());
        ef.ChannelMessageEnd(DEFAULT_CHANNEL);
    }

    std::string encSessionKey;
    {
        RSAES_OAEP_SHA_Encryptor encryptor(pub);
        StringSource ss(sessionKey, sessionKey.size(), true,
                        new PK_EncryptorFilter(prng, encryptor, new StringSink(encSessionKey)));
    }

    std::string envelope;
    appendUint32BE(envelope, static_cast<uint32_t>(encSessionKey.size()));
    envelope.append(encSessionKey);
    envelope.push_back(static_cast<char>(iv.size()));
    envelope.append(reinterpret_cast<const char*>(iv.BytePtr()), iv.size());
    appendUint32BE(envelope, static_cast<uint32_t>(cipherWithTag.size()));
    envelope.append(cipherWithTag);
    return envelope;
}

std::string CryptoHelper::DecryptWithMyPrivate(const std::string& envelope, const std::string& myPrivateKeyPath) {
    AutoSeededRandomPool prng;

    const unsigned char* data = reinterpret_cast<const unsigned char*>(envelope.data());
    size_t pos = 0;
    if (envelope.size() < 4) throw std::runtime_error("envelope too small");
    uint32_t eskLen = readUint32BE(data + pos); pos += 4;
    if (pos + eskLen + 1 + 4 > envelope.size()) throw std::runtime_error("envelope truncated");
    std::string encSessionKey(reinterpret_cast<const char*>(data + pos), eskLen); pos += eskLen;
    uint8_t ivLen = data[pos]; pos += 1;
    if (pos + ivLen + 4 > envelope.size()) throw std::runtime_error("envelope truncated (iv)");
    std::string iv(reinterpret_cast<const char*>(data + pos), ivLen); pos += ivLen;
    uint32_t cipherLen = readUint32BE(data + pos); pos += 4;
    if (pos + cipherLen > envelope.size()) throw std::runtime_error("envelope truncated (cipher)");
    std::string cipherWithTag(reinterpret_cast<const char*>(data + pos), cipherLen);

    RSA::PrivateKey priv;
    {
        FileSource fs(myPrivateKeyPath.c_str(), true);
        priv.BERDecode(fs);
    }

    std::string recovered;
    {
        RSAES_OAEP_SHA_Decryptor decryptor(priv);
        StringSource ss(encSessionKey, true,
                        new PK_DecryptorFilter(prng, decryptor, new StringSink(recovered)));
    }
    SecByteBlock sessionKey(reinterpret_cast<const byte*>(recovered.data()), recovered.size());

    std::string clear;
    {
        GCM<AES>::Decryption dec;
        dec.SetKeyWithIV(sessionKey, sessionKey.size(), reinterpret_cast<const byte*>(iv.data()), iv.size());
        AuthenticatedDecryptionFilter df(dec, new StringSink(clear),
                                         AuthenticatedDecryptionFilter::DEFAULT_FLAGS);
        StringSource ss(cipherWithTag, true, new Redirector(df));
        if (!df.GetLastResult()) throw std::runtime_error("message authentication failed");
    }
    return clear;
}
