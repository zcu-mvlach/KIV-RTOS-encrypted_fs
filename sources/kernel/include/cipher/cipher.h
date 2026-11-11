// cipher/ciphers.h
#pragma once

#include <cipher/record_cipher.h>

/**
 * @brief Sprava dostupnych sifer
 */
class CCipher_Manager {
    public:
        /**
         * @brief Vrati sifru podle identifikatoru
         * @param   suite  Identifikator sifry
         * @return Ukazatel na sdilenou instanci sifry, nebo nullptr, pokud je sifra neznama
         */
        const IRecord_Cipher* Get_Cipher(NCipher_Suite suite) const;
};

extern CCipher_Manager sCipher_Manager;