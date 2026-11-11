#include <cipher/cipher.h>
#include <cipher/suites/chacha20_poly1305.h>
#include <cipher/suites/xor_cipher.h>

CCipher_Manager sCipher_Manager;

// instance sifer
static const CXor_Cipher sXor_Cipher;
static const CChaCha20_Poly1305 sChaCha20_Poly1305;

const IRecord_Cipher* CCipher_Manager::Get_Cipher(NCipher_Suite suite) const {
    switch (suite) {
        case NCipher_Suite::Xor:               return &sXor_Cipher;
        case NCipher_Suite::ChaCha20_Poly1305: return &sChaCha20_Poly1305;
        default: return nullptr;
    }
}