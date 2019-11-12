#include <stdint.h>
#include "realmode.h"
#include "x86helper.h"
#include "biosdisk.h"

fastcall uint8_t
reset_disk (disk_info_s *const disk_info)
{
    return reset_disk_i (disk_info);
}

fastcall uint8_t
read_sector_chs (disk_info_s *const disk_info)
{
    return read_sector_chs_i (disk_info);
}

fastcall uint8_t
read_sector_lba (disk_info_s *const disk_info, const uint32_t lba)
{
    return read_sector_lba_i (disk_info, lba);
}
