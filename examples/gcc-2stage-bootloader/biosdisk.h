#ifndef BIOSDISK_H
#define BIOSDISK_H

#include <stdint.h>

/* BIOS Parameter Block (BPB) on floppy media */
typedef struct __attribute__((packed)) {
    char     OEMname[8];
    uint16_t bytesPerSector;
    uint8_t  sectPerCluster;
    uint16_t reservedSectors;
    uint8_t  numFAT;
    uint16_t numRootDirEntries;
    uint16_t numSectors;
    uint8_t  mediaType;
    uint16_t numFATsectors;
    uint16_t sectorsPerTrack;
    uint16_t numHeads;
    uint32_t numHiddenSectors;
    uint32_t numSectorsHuge;
    uint8_t  driveNum;
    uint8_t  reserved;
    uint8_t  signature;
    uint32_t volumeID;
    char     volumeLabel[11];
    char     fileSysType[8];
} disk_bpb_s;

/* State information for CHS disk accesses */
typedef struct __attribute__((packed)) {
    uint16_t segment;
    uint16_t offset;
    uint16_t status;
    /* Drive geometry needed to compute CHS from LBA */
    uint16_t sectorsPerTrack;
    uint16_t numHeads;
    /* Disk parameters */
    uint16_t cylinder;
    uint8_t  head;
    uint8_t  sector;
    uint8_t  driveNum;
    uint8_t  numSectors;    /* # of sectors to read */
    /* Number of retries for disk operations */
    uint8_t  retries;
} disk_info_s;

extern fastcall uint8_t
reset_disk (disk_info_s *const disk_info);
extern fastcall uint8_t
read_sector_chs (disk_info_s *const disk_info);

/* Forced inline version of reset_disk */
static inline fastcall always_inline uint8_t
reset_disk_i (disk_info_s *const disk_info)
{
    uint16_t temp_ax = 0x0000;
    uint8_t  carryf;

    __asm__ __volatile__ (
            "int $0x13\n\t"
#ifdef __GCC_ASM_FLAG_OUTPUTS__
            : [cf]"=@ccc"(carryf),
#else
            "setc %[cf]\n\t"
            : [cf]"=qm"(carryf),
#endif
              "+a"(temp_ax)
            : "d"(disk_info->driveNum)
            : "cc");

    disk_info->status = temp_ax;

    return (carryf);

}

/* Forced inline version of read_sector_chs */
static inline fastcall always_inline uint8_t
read_sector_chs_i (disk_info_s *const disk_info)
{
    uint16_t temp_ax;
    uint16_t temp_dx;
    uint8_t  carryf = 0;
    uint8_t  retry_count = 0;

#ifndef BUGGY_BIOS_SUPPORT
    temp_dx = (disk_info->head << 8) | disk_info->driveNum;
#endif

    do {
        /* Only reset disk if error detected previously */
        if (carryf)
            reset_disk_i (disk_info);

        /* Need to reload AX during each iteration since a previous
         * int 0x13 call will destroy its contents. There was a bug on
         * earlier BIOSes where DX may have been clobbered.
         */
        temp_ax = (0x02 << 8) | disk_info->numSectors;
#ifdef BUGGY_BIOS_SUPPORT
        temp_dx = (disk_info->head << 8) | disk_info->driveNum;
#endif

        __asm__ __volatile__ (
                "push %%es\n\t"
                "mov %w[seg], %%es\n\t"
#ifdef BUGGY_BIOS_SUPPORT
                "stc\n\t"        /* Some early bioses have CF bug */
                "int $0x13\n\t"
                "sti\n\t"        /* Some early bioses don't re-enable interrupts */
#else
                "int $0x13\n\t"
#endif
                "pop %%es\n\t"
#ifdef __GCC_ASM_FLAG_OUTPUTS__
                : [cf]"=@ccc"(carryf),
#else
                "setc %[cf]\n\t"
                : [cf]"=qm"(carryf),
#endif
#ifdef BUGGY_BIOS_SUPPORT
                  "+a"(temp_ax),
                  "+d"(temp_dx)
                  :
#else
                  "+a"(temp_ax)
                  :
                  "d"(temp_dx),
#endif
                  "c"(((disk_info->cylinder & 0xff) << 8) |
                     ((disk_info->cylinder >> 2) & 0xC0) |
                     (disk_info->sector & 0x3f)),
                  "b"(disk_info->offset),
                  [seg]"r"(disk_info->segment)
                : "memory", "cc");

    } while (carryf && (++retry_count < disk_info->retries));

    disk_info->status = temp_ax;
    return (carryf);
}

/* Forced inline version of read_sector_lba */
static inline fastcall always_inline uint8_t
read_sector_lba_i (disk_info_s *const disk_info, const uint32_t lba)
{
    disk_info->cylinder = lba / disk_info->sectorsPerTrack / disk_info->numHeads;
    disk_info->head     = (lba / disk_info->sectorsPerTrack) % disk_info->numHeads;
    disk_info->sector   = (lba % disk_info->sectorsPerTrack) + 1;

    return read_sector_chs_i (disk_info);
}
#endif
