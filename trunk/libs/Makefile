CC = arm-elf-gcc
LD = arm-elf-ld
OBJCOPY = arm-elf-objcopy

# Directories
LDDIR=ldscripts
BINDIR=bin
GCC_LIB_DIR=/opt/local/lib/gcc/arm-elf/4.1.1
STDC_LIB_DIR=/opt/local/arm-elf/lib/

# Compiler flags
COMMONFLAGS = -c -mcpu=arm7tdmi
AFLAGS = -x assembler-with-cpp
CFLAGS = -Winline -O3 -ffunction-sections -fdata-sections
LDFLAGS = --gc-sections
LIBS=$(GCC_LIB_DIR)/libgcc.a $(STDC_LIB_DIR)/libc.a
# Uncomment to produce a memory map
# PRODUCE_MEMORY_MAP=-Wl,-M
PRODUCE_MEMORY_MAP=
PRODUCE_MEMORY_MAP_LD=-M
#PRODUCE_MEMORY_MAP_LD=

THUMB = -marm

# Lib files to compile
CSRC = lib_spi.c lib_ui.c lib_ssc.c lib_ledctl.c armio.c
COBJ = $(CSRC:.c=.o)
ASRC = startup_SAM7S.s
AOBJ = $(ASRC:.s=.o)

# Builds libraries (but nothing else)
all: all_libs

# Builds all tests
tests: lib_ui_test

clean:
	rm -f *.o
	rm -f bin/*

# For now, we compile everything using gcc because inline functions are dup'ed
all_libs: $(AOBJ) $(COBJ)
#	$(CC) $(CFLAGS) $(THUMB) $(CSRC)

# Test binaries
lib_ui_test: all_libs
	$(CC) $(COMMONFLAGS) $(CFLAGS) $(THUMB) lib_ui_test.c
	$(LD) -T$(LDDIR)/lib_ui_test.ld --oformat elf32-littlearm $(COBJ) $(AOBJ) $(LIBS) lib_ui_test.o --output $(BINDIR)/lib_ui_test.elf $(PRODUCE_MEMORY_MAP_LD)
#	$(CC) -Wl,-T$(LDDIR)/lib_ui_test.ld -mcpu=arm7tdmi $(THUMB) $(COBJ) lib_ui_test.o -o $(BINDIR)/lib_ui_test.elf $(PRODUCE_MEMORY_MAP) -Wl,--verbose
	$(OBJCOPY) -O binary $(BINDIR)/lib_ui_test.elf $(BINDIR)/lib_ui_test.bin
	@echo
	@echo Binary size: `ls -l $(BINDIR)/lib_ui_test.bin | awk '{print $$5}'` bytes

$(COBJ): %.o : %.c
	@echo $<
	$(CC) $(COMMONFLAGS) $(CFLAGS) $(THUMB) $< -o $@

$(AOBJ): %.o : %.s
	arm-elf-gcc $(COMMONFLAGS) $(AFLAGS) $(THUMB) $(INCLUDES) -Wall $< -o $@

