#include <stdint.h>
#include "realmode.h"
#include "biostty.h"

/* main "C" function. Will be part of code loaded for stage2 */
void main(const uint8_t drive_num)
{
    static char *strHello = "Hello ";
    static char strWorld[] = "World\n\r";
    (void)drive_num;

    writetty_str (strHello, 0x00, 0x00);
    writetty_str (strWorld, 0x00, 0x00 );
}

