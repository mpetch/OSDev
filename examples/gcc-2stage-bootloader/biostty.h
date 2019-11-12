#ifndef BIOSTTY_H
#define BIOSTTY_H

#include <stdint.h>
#include "x86helper.h"

extern fastcall void
writetty_str (char *str, const uint8_t page, const uint8_t fg_color);

extern fastcall void
writetty_char (const uint8_t outchar, const uint8_t page, const uint8_t fg_color);

static inline fastcall always_inline void
writetty_char_i (const uint8_t outchar, const uint8_t page, const uint8_t fg_color)
{
   __asm__ ("int $0x10\n\t"
            :
            : "a"((0x0e << 8) | outchar),
              "b"((page << 8) | fg_color));
}

static inline fastcall always_inline void
writetty_str_i (char *str,  const uint8_t page, const uint8_t fg_color)
{
    /* write characters until we reach nul terminator in str */
    while (*str)
        writetty_char_i (*str++, page, fg_color);
}

#endif
