#pragma once

#include <hal/intdef.h>

/**
 * @brief Vynuluje pamet tak, aby zapis kompilator pri optimalizaci neodstranil
 *
 * Pouziva se pro mazani citlivych dat (klic, stav sifry) pred uvolnenim pameti
 *
 * @param ptr Ukazatel na zacatek pameti, kterou je treba vynulovat. Pokud je
 *            nullptr, funkce nic nedela
 * @param length Pocet bajtu k vynulovani
 */
void Secure_Zero(void *ptr, uint32_t length);