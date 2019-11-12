OUTPUT_FORMAT("elf32-i386");
ENTRY(start);
SECTIONS
{
    . = 0x7C00;
    .mbrjmp : {
        BYTE(0xEB);
        BYTE(ADDR(.text.mbr)-2);
        BYTE(0x90);
    }

    .bpb 0x7C03 : AT(0x7C03) SUBALIGN(0) {
        mbrboot.o(.data.bpb);
    }

    .text.mbr : SUBALIGN(0) {
        mbrboot.o(.text.entry);
        mbrboot.o(.text);
    }
    .data.mbr : SUBALIGN(4) {
        mbrboot.o(.data);
        mbrboot.o(.rodata*);
    }

    /* Boot signature */
    .sig 0x7DFE : AT(0x7DFE) {
        SHORT(0xaa55);
    }

    __stage2_load_start     = .;
    __stage2_load_start_lba = (__stage2_load_start - ADDR(.mbrjmp)) / 512;

    .text : SUBALIGN(4) {
        *(.text.start);
        *(.text);
    }

    .data : SUBALIGN(4) {
        *(.data);
        *(.rodata*);
    }
    __stage2_load_end = .;
    __stage2_load_sectors = ((__stage2_load_end - __stage2_load_start) +511) / 512;

    .bss : SUBALIGN(4) {
        __bss_start = .;
        *(.COMMON);
        *(.bss)
    }
    . = ALIGN(4);
    __bss_end = .;

    __bss_sizel = ((__bss_end)-(__bss_start))>>2;
    __bss_sizeb = ((__bss_end)-(__bss_start));

    /DISCARD/ : { 
        *(.eh_frame);
        *(.comment);
        *(.note.gnu.build-id);
    }
}
