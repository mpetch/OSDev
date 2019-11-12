#ifndef BIOSBOOT_H
#define BIOSBOOT_H

#include <stdint.h>

/* Sections containing the bootloader prologue code */
#define BPB_SECTION_DATA __attribute__((section(".data.bpb")))

extern void main(const uint8_t drive_num);

#ifndef __clang__
    __asm__ (".code16gcc\n\t");
#endif

#endif
