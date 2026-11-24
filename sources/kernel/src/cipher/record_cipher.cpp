#include <cipher/record_cipher.h>

namespace {
    // Pozice dat v hlavicce
    constexpr uint32_t Suite_Header_Pos = 0;
    constexpr uint32_t Lower_Size_Header_Pos = 2;
    constexpr uint32_t Upper_Size_Header_Pos = 3;
}

NCipher_Result IRecord_Cipher::Parse_Common_Header(const uint8_t *hdr, uint32_t hdr_len, uint32_t &total_len) const {
    if (hdr == nullptr || hdr_len < mInfo.header_len) {
        return NCipher_Result::Bad_Argument;
    }

    if (hdr[Suite_Header_Pos] != static_cast<uint32_t>(mInfo.suite_id)) {
        return NCipher_Result::Bad_Format;
    }

    // vypocet velikosti samotnych dat z hlavicky (Little Endian zapis)
    uint32_t data_len = static_cast<uint32_t>(hdr[Lower_Size_Header_Pos]) | 
                         static_cast<uint32_t>(hdr[Upper_Size_Header_Pos] << 8);

    if (data_len > mInfo.max_plain) {
        return NCipher_Result::Bad_Format;
    }

    total_len = mInfo.header_len + mInfo.nonce_len +
                 data_len + mInfo.tag_len;

    return NCipher_Result::Ok;
}