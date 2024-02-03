BITS = 64
LDFLAGS = -m elf_x86_64
XFLAGS = -m64 -DX64 -mcmodel=kernel -mtls-direct-seg-refs -mno-red-zone
FSIMAGE := fs.img
QEMU = qemu-system-x86_64

OPT ?= -O0

VPATH := include :lib

CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump
CFLAGS = -fno-pic -static -fno-builtin -nostdlib -fno-strict-aliasing -Wall -MD -ggdb -fno-omit-frame-pointer 
CFLAGS += -ffreestanding -fno-common -I./include -I./lib -gdwarf-2 $(XFLAGS) $(OPT)
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
IFLAGS := -I./include/model -I./lib/include/os -I./include/arch/x86 -I./include/arch/x86/arch/64
IFLAGS += -I lib -I include
IFLAGS += -I./lib/include
IFLAGS += -I./lib/os_arch_include/x86_64
IFLAGS += -I./lib/arch_include/x86
IFLAGS += -I./include/kernel
ASFLAGS = -fno-pic -gdwarf-2 -Wa,-divide -I./include $(XFLAGS)

KOBJ := \
	kobj/inlines.o\
	kobj/kernel_main.o\
	kobj/kernel_swtch.o\
	kobj/kernel_acpi.o\
	kobj/kernel_console.o\
	kobj/kernel_cspace.o\
	kobj/kernel_ioapic.o\
	kobj/kernel_lapic.o\
	kobj/kernel_loadplugin.o\
	kobj/kernel_vm.o\
	kobj/kernel_memblock.o\
	kobj/kernel_proc.o\
	kobj/kernel_string.o\
	kobj/kernel_syscall.o\
	kobj/kernel_sysproc.o\
	kobj/kernel_thread.o\
	kobj/kernel_trap.o\
	kobj/object_cnode.o\
	kobj/object_endpoint.o\
	kobj/object_notification.o\
	kobj/object_untyped.o\
	kobj/object_objecttype.o\
	kobj/object_tcb.o\
	kobj/api_faults.o\
	kobj/x86_64_kernel_thread.o\
	kobj/x86_64_kernel_trapasm.o\
	kobj/x86_64_kernel_vspace.o\
	kobj/x86_64_machine_registerset.o\
	kobj/x86_64_model_statedata.o\
	kobj/x86_kernel_vspace.o\
	kobj/x86_machine_registerset.o\
	kobj/model_statedata.o\
	kobj/vectors.o
	
SRCS := $(wildcard src/**/*.c)
SRSS := $(wildcard src/**/*.s)

LIB  := libout/os.o

libout/os.o: lib/src/os.c
	@mkdir -p libout
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<

kobj/api_faults.o: src/api/faults.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	
kobj/x86_64_kernel_thread.o: src/arch/x86/64/kernel/thread.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<

kobj/x86_64_kernel_trapasm.o: src/arch/x86/64/kernel/trapasm.S
	@mkdir -p kobj
	$(CC) $(ASFLAGS) -c -o $@ $<
	
kobj/x86_64_kernel_vspace.o: src/arch/x86/64/kernel/vspace.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	
kobj/x86_64_machine_registerset.o: src/arch/x86/64/machine/registerset.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	
kobj/x86_64_model_statedata.o: src/arch/x86/64/model/statedata.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<

kobj/x86_kernel_vspace.o: src/arch/x86/kernel/vspace.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	
kobj/x86_machine_registerset.o: src/arch/x86/machine/registerset.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	
kobj/kernel_acpi.o: src/kernel/acpi.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<

kobj/kernel_console.o: src/kernel/console.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	
kobj/kernel_swtch.o: src/kernel/swtch.S
	@mkdir -p kobj
	$(CC) $(ASFLAGS) -c -o $@ $<

kobj/vectors.o: out/vectors.S
	@mkdir -p kobj
	$(CC) $(ASFLAGS) -c -o $@ $<

kobj/kernel_cspace.o: src/kernel/cspace.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	
kobj/kernel_ioapic.o: src/kernel/ioapic.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	
kobj/kernel_lapic.o: src/kernel/lapic.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	
kobj/kernel_loadplugin.o: src/kernel/loadplugin.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	
kobj/kernel_main.o: src/kernel/main.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<

kobj/kernel_memblock.o: src/kernel/memblock.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	
kobj/kernel_proc.o: src/kernel/proc.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	
kobj/kernel_string.o: src/kernel/string.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	
kobj/kernel_syscall.o: src/kernel/syscall.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	
kobj/kernel_sysproc.o: src/kernel/sysproc.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	
kobj/kernel_thread.o: src/kernel/thread.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	
kobj/kernel_trap.o: src/kernel/trap.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	
kobj/kernel_vm.o: src/kernel/vm.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	
kobj/model_statedata.o: src/model/statedata.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	
kobj/object_cnode.o: src/object/cnode.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<

kobj/object_endpoint.o: src/object/endpoint.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	
kobj/object_notification.o: src/object/notification.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	
kobj/object_objecttype.o: src/object/objecttype.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	
kobj/object_tcb.o: src/object/tcb.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	
kobj/object_untyped.o: src/object/untyped.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	
kobj/inlines.o: src/inlines.c
	@mkdir -p kobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<

out/os.img: out/bootblock out/kernel.elf out/fs.img
	dd if=/dev/zero of=out/os.img count=10000
	dd if=out/bootblock of=out/os.img conv=notrunc
	dd if=out/kernel.elf of=out/os.img seek=1 conv=notrunc

out/bootblock: src/kernel/bootasm.S src/kernel/bootmain.c
	@mkdir -p out
	$(CC) $(IFLAGS) -fno-builtin -fno-pic -m32 -Iinclude -O -o out/bootmain.o -c src/kernel/bootmain.c
	$(CC) $(IFLAGS) -fno-builtin -fno-pic -m32 -Iinclude -o out/bootasm.o -c src/kernel/bootasm.S
	$(LD) -m elf_i386 -N -e start -Ttext 0x7C00 -o out/bootblock.o out/bootasm.o out/bootmain.o
	$(OBJDUMP) -S out/bootblock.o > out/bootblock.asm
	$(OBJCOPY) -S -O binary -j .text out/bootblock.o out/bootblock
	tools/sign.pl out/bootblock

ENTRYCODE = kobj/entry$(BITS).o
LINKSCRIPT = src/kernel/kernel$(BITS).ld
out/kernel.elf:  $(ENTRYCODE) $(LINKSCRIPT) $(KOBJ)
	$(LD) $(LDFLAGS) -T $(LINKSCRIPT) -o out/kernel.elf $(ENTRYCODE) $(KOBJ) -b binary
	$(OBJDUMP) -S out/kernel.elf > out/kernel.asm
	$(OBJDUMP) -t out/kernel.elf | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > out/kernel.sym

$(ENTRYCODE): src/kernel/entry$(BITS).s
	@mkdir -p kobj
	$(CC) $(ASFLAGS) -c -o $@ $<

MKVECTORS = tools/vectors$(BITS).pl
src/kernel/vectors.S: $(MKVECTORS)
	perl $(MKVECTORS) > kernel/vectors.S
	
uobj/usys.o: src/kernel/usys.S
	@mkdir -p uobj
	$(CC) $(ASFLAGS) -c -o $@ $<

CPLUGINS=\
	plugin/sendtest.c\
	plugin/receivetest.c\

uobj/%.o: $(CPLUGINS)
	@mkdir -p uobj
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<
	


fs/%: uobj/%.o uobj/usys.o libout/os.o
	@mkdir -p fs out
	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $@  $^
	
MKVECTORS = tools/vectors64.pl

out/vectors.S: $(MKVECTORS)
	perl $(MKVECTORS) > out/vectors.S	
	
PLUGINS=\
	fs/sendtest\
	fs/reveivetest\

out/fs.img : $(PLUGINS)
	dd if=/dev/zero of=out/os.img count=10000
	python3 ./tools/movePlugin.py

QEMUOPTS = -net none -hda out/os.img -hdb out/fs.img -m 512

qemu: out/fs.img out/os.img
	$(QEMU) -serial mon:stdio $(QEMUOPTS) -no-reboot -D ./log.txt -d int
	
qemu-gdb: out/fs.img out/os.img
	$(QEMU) -serial mon:stdio $(QEMUOPTS) -S -s -D ./log.txt -d int
    	
clean:
	@rm -rf ./fs
	@rm -rf ./kobj
	@rm -rf ./out
	@rm -rf ./uobj
	@rm -rf ./libout
