#pragma once

#include <cipher/record_cipher.h>

class CChaCha20_Poly1305 final : public IRecord_Cipher {
    public:
        CChaCha20_Poly1305();

        virtual NCipher_Result Init_Context(void *ctx, const uint8_t *key, const uint8_t *nonce_prefix) const override;

        virtual NCipher_Result Encrypt(void *ctx, const uint8_t *aad, uint32_t aad_len,
                                       const uint8_t *plain, uint32_t plain_len,
                                       uint8_t *out, uint32_t &out_len) const override;

        virtual NCipher_Result Decrypt(void *ctx, const uint8_t *aad, uint32_t aad_len,
                                       const uint8_t *rec, uint32_t rec_len,
                                       uint8_t *plain, uint32_t &plain_len) const override;

        virtual NCipher_Result Parse_Header(const uint8_t *hdr, uint32_t hdr_len, uint32_t &total_len) const override;

        virtual void Destroy_Context(void *ctx) const override;
};