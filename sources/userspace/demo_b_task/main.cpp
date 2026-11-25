#include <stdstring.h>
#include <stdfile.h>

/**
 * Demo task B
 *
 * Ceka na zpravu v kanalu a2b, precte ji a loguje "B<A <payload>".
 * Potom posle svuj tick (hex text) do kanalu b2a a loguje "B>A <tick>".
 * Kapacita kanalu (#96) je shodna s ostatnimi tasky; pri existujicim kanalu se ignoruje.
 **/

constexpr const char* A2B_Path = "SYS:bcast/a2b#96";
constexpr const char* B2A_Path = "SYS:bcast/b2a#96";

constexpr const char* Log_Pipe_Name = "log";
constexpr uint32_t Log_Pipe_Size = 32;

// logger cte z log pipe po 15 bytech, proto zprava musi byt kratsi nebo rovna 15
constexpr uint32_t Log_Max_Len = 15;
constexpr uint32_t Log_Buf_Len = Log_Max_Len + 1;

// hex text 32bitoveho tiku ma max 8 znaku, payload z kanalu a2b ocekavame stejne dlouhy
constexpr uint32_t Hex_Base = 16;
constexpr uint32_t Tick_Text_Len = 8;
constexpr uint32_t Tick_Buf_Len = Tick_Text_Len + 1;

/**
 * Slozi zpravu z prefixu a textu a zapise ji jednim write do log pipe.
 * Text se zkrati, pokud by se zprava nevesla do Log_Max_Len.
 * 
 * @param log_pipe deskriptor pipe pro log.
 * @param prefix prefix.
 * @param text text.
 * @return void.
 **/
static void log_message(uint32_t log_pipe, const char* prefix, const char* text) {
    char line[Log_Buf_Len];

    const uint32_t prefix_len = strlen(prefix);
    const uint32_t room_for_text = Log_Max_Len - prefix_len;

    const uint32_t text_len_full = strlen(text);
    const uint32_t text_len = (text_len_full < room_for_text) ? text_len_full : room_for_text;

    strncpy(line, prefix, prefix_len);
    strncpy(line + prefix_len, text, text_len);

    write(log_pipe, line, prefix_len + text_len);
}

int main(int argc, char** argv) {
    uint32_t a2b = open(A2B_Path, NFile_Open_Mode::Read_Only);
    uint32_t b2a = open(B2A_Path, NFile_Open_Mode::Write_Only);
    uint32_t log_pipe = pipe(Log_Pipe_Name, Log_Pipe_Size);

    char tick_buf[Tick_Buf_Len];
    char payload_buf[Tick_Buf_Len];

    while (true) {
        // cekame, dokud v kanalu a2b neni nova zprava
        wait(a2b, 1);

        // read vraci nejvys Tick_Text_Len bytu
        const uint32_t payload_len = read(a2b, payload_buf, Tick_Text_Len);
        if (payload_len < Tick_Buf_Len) {
            payload_buf[payload_len] = '\0';
        }

        log_message(log_pipe, "B<A ", payload_buf);

        // vlastni tick posleme jako hex text
        itoa(get_tick_count(), tick_buf, Hex_Base);
        const uint32_t tick_len = strlen(tick_buf);

        write(b2a, tick_buf, tick_len);
        log_message(log_pipe, "B>A ", tick_buf);
    }

    return 0;
}
