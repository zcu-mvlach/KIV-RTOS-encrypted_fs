#pragma once

#include <hal/intdef.h>

// Vysledek opearce sifroavni
enum class NCipher_Result : uint8_t {
    Ok = 0,
    Bad_Argument,    // spatny klic, buffer nebo delka zpravy
    Bad_Format,      // kratky zaznam, jine suite_id, spatne len
    Auth_Failed,     // tag nesedi (spatny klic, zmenena data)
    Replay,          // nonce uz videna nebo starsi nez posledni
    Nonce_Exhausted, // citac nonce vycerpan, nutne otevrit znovu
};

// Popis sifry (konstantni a nezavisle na kanalu)
struct TCipher_Info {
    uint8_t suite_id;      // identifikator sifry (1 = ChaCha20-Poly1305, 2 = XOR)
    uint32_t key_len;      // delka klice v bajtech
    uint32_t nonce_len;    // delka nonce (prefix + citac)
    uint32_t tag_len;      // delka autentizacniho tagu
    uint32_t header_len;   // delka hlavicky zaznamu
    uint32_t max_plain;    // maximalni delka plaintextu v jednom zaznamu
    uint32_t context_size; // kolik bajtu musi volajici alokovat pro kontext
};

// Identifikatory sifer
enum class NCipher_Suite : uint8_t {
    ChaCha20_Poly1305 = 1,
    Xor = 2,
};

/**
 * @brief Rozhrani sifry pracujici po zaznamech
 * @note
 * Sifra je bezestavova - veskere promenne veci (klic, citac, nonce...)
 * je uchovan v kontextu ctx, ktery alokuje volajici podle Info().context_size
 */
class IRecord_Cipher {
    private:
        const TCipher_Info mInfo;

    protected:
        /**
         * @brief Vrati popis sifry (suite, delky klice, nonce, tagu, hlavicky, velikost kontextu).
         * @return Konstantni reference na popis
         */
        IRecord_Cipher(const TCipher_Info &info) : mInfo(info) {}

    public:
        virtual ~IRecord_Cipher() = default;

        // Metoda vracejici info o dane sifre
        const TCipher_Info &Info() const { return mInfo; }

        /**
         * @brief Pripravi kontext sifry z klice a prefixu nonce
         *
         * Kontext musi alokovat volajici (alespon Info().context_size bajtu).
         * Citac nonce se nastavi na pocatecni hodnotu a pamet posledniho prijateho citace se vynuluje.
         *
         * @param   ctx           Pamet pro kontext, po navratu inicializovana
         * @param   key           Klic o delce Info().key_len bajtu
         * @param   nonce_prefix  Prefix nonce (8 B); ma byt ruzny pro kazdeho zapisovatele
         * @return Ok, nebo Bad_Argument pri nullptr nebo nevyhovujicim klici
         */
        virtual NCipher_Result Init_Context(void *ctx, const uint8_t *key, const uint8_t *nonce_prefix) const = 0;

        /**
         * @brief Zasifruje plaintext do jednoho zaznamu (hlavicka, nonce, data, tag)
         *
         * Po uspechu zvysi citac nonce v kontextu. Hlavicku zaznamu si sifra chrani sama
         * (je soucasti autentizace), parametr aad slouzi jen pro pripadna dalsi chranena data.
         *
         * @param   ctx        Inicializovany kontext
         * @param   aad        Dalsi chranena data (nepovinne, nullptr)
         * @param   aad_len    Delka aad v bajtech (0, pokud je aad nullptr)
         * @param   plain      Plaintext k zasifrovani
         * @param   plain_len  Delka plaintextu, nesmi presahnout Info().max_plain
         * @param   out        Vystupni buffer pro cely zaznam, musi mit alespon
         *                     plain_len + header_len + nonce_len + tag_len bajtu
         * @param   out_len    Skutecna delka vytvoreneho zaznamu v bajtech
         * @return Ok; Bad_Argument pri spatnych ukazatelich nebo delce; Nonce_Exhausted,
         *         pokud je citac vycerpan (kanal je nutne otevrit znovu)
         */
        virtual NCipher_Result Encrypt(void *ctx, const uint8_t *aad, uint32_t aad_len,
                                    const uint8_t *plain, uint32_t plain_len,
                                    uint8_t *out, uint32_t &out_len) const = 0;

        /**
         * @brief Overi a desifruje cely zaznam
         *
         * Kontroly probihaji v poradi: format, tag, citac nonce. Citac se v kontextu
         * posune az po uspesnem overeni tagu. Pri jakekoliv chybe se vystupni buffer
         * vynuluje a plain_len je 0, aby volajici nedostal zadna data.
         *
         * @param   ctx        Inicializovany kontext (drzi posledni prijaty citac)
         * @param   aad        Dalsi chranena data, musi byt stejna jako pri sifrovani
         * @param   aad_len    Delka aad v bajtech
         * @param   rec        Cely zaznam (hlavicka, nonce, data, tag)
         * @param   rec_len    Delka zaznamu v bajtech
         * @param   plain      Vystupni buffer pro plaintext, musi mit alespon
         *                     rec_len - header_len - nonce_len - tag_len bajtu
         * @param   plain_len  Delka desifrovaneho plaintextu
         * @return Ok; Bad_Format (kratky zaznam, jine suite_id, nesedi len); Auth_Failed
         *         (tag nesedi); Replay (nonce uz videna nebo starsi); Bad_Argument
         */
        virtual NCipher_Result Decrypt(void *ctx, const uint8_t *aad, uint32_t aad_len,
                                    const uint8_t *rec, uint32_t rec_len,
                                    uint8_t *plain, uint32_t &plain_len) const = 0;

        /**
         * @brief Z hlavicky zaznamu zjisti celkovou delku zaznamu
         *
         * Pouziva se u proudu (roura, UART), kde neni znama hranice zaznamu: volajici
         * nacte alespon Info().header_len bajtu a podle vysledku docte zbytek.
         *
         * @param   hdr        Zacatek zaznamu (hlavicka)
         * @param   hdr_len    Kolik bajtu hlavicky je k dispozici (alespon Info().header_len)
         * @param   total_len  Celkova delka zaznamu (hlavicka, nonce, data, tag)
         * @return Ok; Bad_Format, pokud je hlavicka neplatna (jine suite_id, nesmyslne len);
         *         Bad_Argument, pokud je hdr_len mensi nez Info().header_len
         */
        virtual NCipher_Result Parse_Header(const uint8_t *hdr, uint32_t hdr_len, uint32_t &total_len) const = 0;

        /**
         * @brief Vynuluje citlive udaje v kontextu (klic, citace)
         *
         * Pamet kontextu nevraci, tu uvolnuje volajici. Po zavolani se kontext nesmi dale pouzit.
         *
         * @param   ctx  Kontext k vymazani
         */
        virtual void Destroy_Context(void *ctx) const = 0;
};