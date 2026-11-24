#include <cipher/suites/xor_cipher.h>

namespace {
    // Metadata XOR sifry
    constexpr uint32_t Key_Len      = 32;   // delka klice
    constexpr uint32_t Prefix_Len   = 8;    // delka prefixu nonce
    constexpr uint32_t Counter_Len  = 4;    // delka citace v nonce
    constexpr uint32_t Nonce_Len    = Prefix_Len + Counter_Len;   // 12
    constexpr uint32_t Tag_Len      = 4;    // delka tagu
    constexpr uint32_t Record_Overhead = Record_Header_Len + Nonce_Len + Tag_Len;   // 20

    // Stav jednoho kanalu
    struct TXor_Context {
        uint8_t key[Key_Len];
        uint8_t nonce_prefix[Prefix_Len];
        uint32_t send_counter;      // citac pro dalsi odeslany zaznam
        uint32_t last_counter;      // posledni prijaty citac
        bool has_last;              // overeni prijeti nejakeho zaznamu
    };
}

CXor_Cipher::CXor_Cipher()
    : IRecord_Cipher({
        NCipher_Suite::Xor,
        Key_Len, Nonce_Len, Tag_Len, Record_Header_Len,
        Record_Max_Plain,
        static_cast<uint32_t>(sizeof(TXor_Context))
    }) {}

NCipher_Result CXor_Cipher::Init_Context(void *ctx, const uint8_t *key, const uint8_t *nonce_prefix) const {
    // TODO: need to be implemented
    return NCipher_Result::Bad_Format;
}

NCipher_Result CXor_Cipher::Encrypt(void *ctx, const uint8_t *aad, uint32_t aad_len,
                                       const uint8_t *plain, uint32_t plain_len,
                                       uint8_t *out, uint32_t &out_len) const {
    // TODO: need to be implemented
    return NCipher_Result::Bad_Format;
}

NCipher_Result CXor_Cipher::Decrypt(void *ctx, const uint8_t *aad, uint32_t aad_len,
            const uint8_t *rec, uint32_t rec_len,
                                       uint8_t *plain, uint32_t &plain_len) const {
    // TODO: need to be implemented
    return NCipher_Result::Bad_Format;
}

NCipher_Result CXor_Cipher::Parse_Header(const uint8_t *hdr, uint32_t hdr_len, uint32_t &total_len) const {
    // TODO: need to be implemented
    return NCipher_Result::Bad_Format;
}

void CXor_Cipher::Destroy_Context(void *ctx) const {
    // TODO: need to be implemented
}