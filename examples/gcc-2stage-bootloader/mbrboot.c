#include <stdint.h>
#include "realmode.h"
#include "x86helper.h"
#include "biostty.h"
#include "biosdisk.h"
#include "biosboot.h"

#define SECONDSTAGE_SEGMENT 0x0000
#define SECONDSTAGE_OFFSET 0x7e00
#define SECONDSTAGE_LOAD_SEG SECONDSTAGE_OFFSET>>4
#define FIRSTSTAGE_SEGMENT 0x0000
#define FIRSTSTAGE_OFFSET 0x7c00

/* Stack offset below bootloader */
#define STACK_OFFSET FIRSTSTAGE_OFFSET

/* Define the BIOS Parameter Block. This contains values
 * for 1.44MB 3.5" floppy
 */

disk_bpb_s BPB_SECTION_DATA bpb_disk_info = {
    .sectorsPerTrack = 18,
    .bytesPerSector = 512,
    .numHeads = 2,
    .numSectors = 2880,
    .mediaType = 0xf0,
    .signature = 0x29,
    .OEMname = "mkfs.fat",
    .volumeLabel = "NO NAME   ",
    .fileSysType = "FAT12   ",
    .sectPerCluster = 1,
    .reservedSectors = 1,
    .numFAT = 2,
    .numRootDirEntries = 224,
    .numFATsectors = 9
};

extern uintptr_t __stage2_load_start_lba;
extern uintptr_t __stage2_load_sectors;
extern uintptr_t __bss_start;
extern uintptr_t __bss_end;

/* Before anything else set stack pointer. The linker script will
 * place section .text.entry before all other code.
 * Here we set to SS:ESP to STACK_SEGMENT:STACK_OFFSET below the
 * bootloader. Pass drive number in DL into AL
 * (first parameter for a fastcall). Jump to start function
 * and set CS. Function start should never return
 */

__asm__ (".pushsection .text.entry\n\t"
         "mov $" STR(SECONDSTAGE_SEGMENT) ", %ax\n\t"
         "mov %ax, %ss\n\t"
         "mov $" STR(STACK_OFFSET) ", %esp\n\t"
         "mov %ax, %ds\n\t"
         "mov %ax, %es\n\t"
         "mov %dl, %al\n\t"
         "cld\n\t"
         "jmp $" STR(SECONDSTAGE_SEGMENT) ", $start\n"
         ".popsection\n\t");

/* Zero the BSS section */
static inline void fastcall zero_bss()
{
    uint32_t *memloc = &__bss_start;

    while (memloc < &__bss_end)
        *memloc++ = 0;
}

/* This is the bootloader prologue that sets up the segment registers
 * for the second stage, reads the second stage into memory and then
 * calls main in the second stage.
 */

void fastcall noreturn used start (uint8_t drive_num)
{
    uint16_t lba;
    uint32_t lba_end;

    static disk_info_s stage2_disk_info = {
            .segment    = SECONDSTAGE_LOAD_SEG,
            .offset     = 0,
            .retries    = 3,
            .numSectors = 1,
    };

    static char error_str[] = "Disk Error";

    /* Enable interrupts */
    x86_sti();

    stage2_disk_info.sectorsPerTrack = bpb_disk_info.sectorsPerTrack;
    stage2_disk_info.numHeads        = bpb_disk_info.numHeads;
    stage2_disk_info.driveNum        = drive_num;

    /* Read disk sectors into memory using both segment and offset in
     * stage2_disk_info structure */
    lba_end = (uint32_t)&__stage2_load_sectors + (uint32_t)&__stage2_load_start_lba;
    lba = (uint32_t)&__stage2_load_start_lba;

    while (lba < lba_end) {
        if (read_sector_lba_i (&stage2_disk_info, lba) != 0) {
            writetty_str_i (error_str, 0x00, 0x00);
            haltcpu();
        }
        /* Next logical block */
        lba++;
        /* Next segment is +0x20(32) 32*16=512 */
        stage2_disk_info.segment += 0x20;
    }

    /* Set all the bytes in BSS section to 0 */
    zero_bss();
    main(drive_num);
    /* Infinite loop as we can't properly return from start */
    haltcpu();
}
