#ifndef X86HELPER_H
#define X86HELPER_H

#include <stdint.h>

#define STR_TEMP(x) #x
#define STR(x) STR_TEMP(x)

#define TRUE 1
#define FALSE 0
#define NULL (void *)0

/* regparam(3) is a calling convention that passes first
   three parameters via registers instead of on stack.
   1st param = EAX, 2nd param = EDX, 3rd param = ECX */
#define fastcall  __attribute__((regparm(3)))

/* noreturn lets GCC know that a function that it may detect
   won't exit is intentional */
#define noreturn      __attribute__((noreturn))
#define always_inline __attribute__((always_inline))
#define used          __attribute__((used))

/* Define helper x86 function */
static inline void fastcall always_inline x86_hlt(void){
    __asm__ ("hlt\n\t");
}
static inline void fastcall always_inline x86_cli(void){
    __asm__ ("cli\n\t");
}
static inline void fastcall always_inline x86_sti(void){
    __asm__ ("sti\n\t");
}
static inline void fastcall always_inline x86_cld(void){
    __asm__ ("cld\n\t");
}

/* Infinite loop with hlt to end bootloader code */
static inline void noreturn fastcall haltcpu()
{
    while(1){
        x86_hlt();
    }
}

#endif
