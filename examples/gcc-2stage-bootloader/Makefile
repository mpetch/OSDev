CROSSPREFIX:=i686-elf-
CC:=$(CROSSPREFIX)gcc
LD:=$(CROSSPREFIX)ld
AS:=$(CROSSPREFIX)as
OBJCOPY:=$(CROSSPREFIX)objcopy
NASM:=nasm
DD:=dd
QEMU:=qemu-system-i386
BOCHS:=bochs

CFLAGS=-g -Wall -Wextra -pedantic -std=gnu99 -ffreestanding \
       -fno-asynchronous-unwind-tables -fomit-frame-pointer -fno-pic
CFLAGS16=-Os -m16 -xc -mpreferred-stack-boundary=2
LINK_SCRIPT=link.ld
LINKFLAGS=-m32 -no-pie -nostdlib -Wl,--build-id=none -T$(LINK_SCRIPT)

QEMUFLAGS:=
BOCHS_CFG:=/dev/null

OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
C_INCLUDES=$(wildcard *.h)

KERNEL:=mbrboot
KERNEL_BIN:=$(KERNEL).bin
KERNEL_ELF:=$(KERNEL).elf
FLOPPY_IMG:=floppy.img

.PHONY: all clean bochs qemu

all:$(KERNEL_BIN) $(FLOPPY_IMG)

qemu:$(FLOPPY_IMG)
	$(QEMU) -fda $< $(QEMUFLAGS)

bochs:$(FLOPPY_IMG) $(BOCHS_CFG)
	$(BOCHS) -qf $(BOCHS_CFG) \
		'boot: floppy' \
		'floppya: type=1_44, 1_44="$(FLOPPY_IMG)", status=inserted, write_protected=0' \
		'magic_break: enabled=1'

$(KERNEL_ELF): $(OBJS) $(OBJS_ASM) $(LINK_SCRIPT)
	$(CC) $(LINKFLAGS) -o $@ $(OBJS) $(OBJS_ASM)

$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@

$(FLOPPY_IMG): $(KERNEL_BIN)
	$(DD) if=/dev/zero of=$@ bs=1024 count=1440
	$(DD) if=$< of=$@ conv=notrunc

%.o:%.c $(C_INCLUDES)
	$(CC) -c $(CFLAGS) $(CFLAGS16) -o $@ $<

%.o:%.asm $(ASM_INCLUDES)
	$(NASM) $(NASMFLAGS) -o $@ $<

clean:
	rm -f $(OBJS) \
		$(OBJS_NASM) \
		$(KERNEL_BIN) \
		$(KERNEL_ELF) \
		$(FLOPPY_IMG)
