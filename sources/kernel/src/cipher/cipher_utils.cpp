#include <cipher/cipher_utils.h>

void Secure_Zero(void *ptr, uint32_t length) {
    if (ptr == nullptr) {
        return;
    }

    volatile uint8_t *p = static_cast<uint8_t *>(ptr);

    for (uint32_t i = 0; i < length; i++) {
        p[i] = 0;
    }
}