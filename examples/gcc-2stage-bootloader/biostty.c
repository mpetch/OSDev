#include <stdint.h>
#include "realmode.h"
#include "biostty.h"

void fastcall
writetty_str (char *str, const uint8_t page, const uint8_t fg_color)
{
    writetty_str_i (str, page, fg_color);
}

void fastcall
writetty_char (const uint8_t outchar, const uint8_t page, const uint8_t fg_color)
{
    writetty_char_i (outchar, page, fg_color);
}
