#pragma once
#include <string>

class CryptoHelper {
public:
    static void GenerateRSAKeys(const std::string& privPath, const std::string& pubPath, unsigned int bits = 2048);
    static std::string EncryptForPeer(const std::string& plaintext, const std::string& peerPublicKeyPath);
    static std::string DecryptWithMyPrivate(const std::string& envelope, const std::string& myPrivateKeyPath);
};
