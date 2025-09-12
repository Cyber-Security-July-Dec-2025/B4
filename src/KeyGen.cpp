// KeyGen.cpp - generate RSA keypair (DER encoded) using Crypto++
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <cryptopp/files.h>
#include <iostream>
#include <cstdlib>

using namespace CryptoPP;

int main(int argc, char* argv[]) {
    unsigned int keyBits = 2048;
    if (argc > 1) keyBits = static_cast<unsigned int>(std::atoi(argv[1]));

    AutoSeededRandomPool rng;
    RSA::PrivateKey priv;
    priv.GenerateRandomWithKeySize(rng, keyBits);
    RSA::PublicKey pub(priv);

    const char* privPath = "keys/my_private.key";
    const char* pubPath  = "keys/my_public.key";

    // write DER encoded keys
    try {
        FileSink privSink(privPath);
        priv.DEREncode(privSink);
        privSink.MessageEnd();

        FileSink pubSink(pubPath);
        pub.DEREncode(pubSink);
        pubSink.MessageEnd();
    } catch(const Exception& e) {
        std::cerr << "Crypto++ error: " << e.what() << std::endl;
        return 2;
    } catch(...) {
        std::cerr << "Unknown error writing keys\n";
        return 3;
    }

    std::cout << "Generated RSA-" << keyBits << " keypair: " << privPath << " , " << pubPath << "\n";
    return 0;
}
