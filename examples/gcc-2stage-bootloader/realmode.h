#ifndef REALMODE_H
#define REALMODE_H

#ifndef __clang__
    __asm__ (".code16gcc\n\t");
#endif

#endif
