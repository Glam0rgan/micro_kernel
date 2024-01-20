BITS = 64
LDFLAGS = -m elf_x86_64
XFLAGS = -m64 -DX64 -mcmodel=kernel -mtls-direct-seg-refs -mno-red-zone
FSIMAGE := fs.img

OPT ?= -O0

CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump
CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -Wall -MD -ggdb -fno-omit-frame-pointer
CFLAGS += -ffreestanding -fno-common -nostdlib -Iinclude -gdwarf-2 $(XFLAGS) $(OPT)
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
ASFLAGS = -fno-pic -gdwarf-2 -Wa,-divide -Iinclude $(XFLAGS)

SRCS := $(wildcard src/**/*.c)
SRSS := $(wildcard src/**/*.s)

FSIMAGE := fs.img

os.img: out/bootblock out/kernel.elf fs.img
    dd if=/dev/zero of=os.img count=10000
	dd if=out/bootblock of=os.img conv=notrunc
	dd if=out/kernel.elf of=os.img conv=notrunc

out/bootblock: src/kernel/bootasm.S src/kernel/bootmain.c
	@mkdir -p out
	$(CC) -fno-builtin -fno-pic -m32 -nostdinc -Iinclude -O -o out/bootmain.o -c kernel/bootmain.c
	$(CC) -fno-builtin -fno-pic -m32 -nostdinc -Iinclude -o out/bootasm.o -c kernel/bootasm.S
	$(LD) -m elf_i386 -nodefaultlibs -N -e start -Ttext 0x7C00 -o out/bootblock.o out/bootasm.o out/bootmain.o
	$(OBJDUMP) -S out/bootblock.o > out/bootblock.asm
	$(OBJCOPY) -S -O binary -j .text out/bootblock.o out/bootblock
	tools/sign.pl out/bootblock

ENTRYCODE = kobj/entry$(BITS).o
LINKSCRIPT = src/kernel/kernel$(BITS).ld
out/kernel.elf: $(OBJS) $(ENTRYCODE) $(LINKSCRIPT) $(FSIMAGE)
	$(LD) $(LDFLAGS) -T $(LINKSCRIPT) -o out/kernel.elf $(ENTRYCODE) $(OBJS) -b binary $(FSIMAGE)
	$(OBJDUMP) -S out/kernel.elf > out/kernel.asm
	$(OBJDUMP) -t out/kernel.elf | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > out/kernel.sym

MKVECTORS = tools/vectors$(BITS).pl
src/kernel/vectors.S: $(MKVECTORS)
	perl $(MKVECTORS) > kernel/vectors.S

kobj/%.o: SRCS
    @mkdir -p kobj
    $(CC) $(CFLAGS) -c -o $@ $<

kobj/%.o: SRSS
    @mkdir -p kobj
    $(CC) $(ASFLAGS) -c -o $@ $<

uobj/%.o: plugin/%.c
	@mkdir -p uobj
	$(CC) $(CFLAGS) -c -o $@ $<

fs/%: uobj/%.o
	@mkdir -p fs out
	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $@ $^
	
PLUGINS=\
       fs/plugin0\
       fs/plugin1\

fs.img : plugin

plugin : $(PLUGINS)
	@python3 movePlugin.py

QEMUOPTS = -net none -hda os.img -hdb fs.img -m 512 $(QEMUEXTRA)

qemu: fs.img os.img
    $(QEMU) -serial mon:stdio$(QEMUOPTS)