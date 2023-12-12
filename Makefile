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

uobj/%.o: plugin/%.c
	@mkdir -p uobj
	$(CC) $(CFLAGS) -c -o $@ $<

fs/%: uobj/%.o
	@mkdir -p fs out
	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $@ $^
	
PLUGINS=\
       fs/plugin0\
       fs/plugin1\

plugin : $(PLUGINS)
	@python3 movePlugin.py
