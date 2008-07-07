CC = arm-elf-gcc
LD = arm-elf-ld
OBJCOPY = arm-elf-objcopy

# Directories
LDDIR=ldscripts
BINDIR=bin
GCC_LIB_DIR=/opt/local/lib/gcc/arm-elf/4.1.1
STDC_LIB_DIR=/opt/local/arm-elf/lib/

# Compiler flags
CFLAGS = -c -mcpu=arm7tdmi -Winline -O3 -ffunction-sections -fdata-sections
LDFLAGS = --gc-sections
LIBS=$(GCC_LIB_DIR)/libgcc.a $(STDC_LIB_DIR)/libc.a
# Uncomment to produce a memory map
# PRODUCE_MEMORY_MAP=-Wl,-M
PRODUCE_MEMORY_MAP=
#PRODUCE_MEMORY_MAP_LD=-M
PRODUCE_MEMORY_MAP_LD=

THUMB = -marm

# Lib files to compile
OBJLIB = lib_spi.o lib_ledctl.o lib_ssc.o lib_ui.o armio.o
OBJSTARTUP = startup_SAM7S.o
SRC = lib_spi.c lib_ui.c lib_ssc.c lib_ledctl.c armio.c

all: all_libs

clean:
	rm -f *.o
	rm -f bin/*

# For now, we compile everything using gcc because inline functions are dup'ed
all_libs: startup_SAM7S.o
	$(CC) $(CFLAGS) $(THUMB) $(SRC)

startup_SAM7S.o: startup_SAM7S.S
	arm-elf-gcc $(THUMB) $(INCLUDES) -mcpu=arm7tdmi -c -Wall -x assembler-with-cpp startup_SAM7S.S

# Test binaries
lib_ui_test: all_libs
	$(CC) $(CFLAGS) $(THUMB) lib_ui_test.c
	$(LD) -T$(LDDIR)/lib_ui_test.ld --oformat elf32-littlearm $(OBJLIB) $(OBJSTARTUP) $(LIBS) lib_ui_test.o --output $(BINDIR)/lib_ui_test.elf $(PRODUCE_MEMORY_MAP_LD)
#	$(CC) -Wl,-T$(LDDIR)/lib_ui_test.ld -mcpu=arm7tdmi $(THUMB) $(OBJLIB) lib_ui_test.o -o $(BINDIR)/lib_ui_test.elf $(PRODUCE_MEMORY_MAP) -Wl,--verbose
	$(OBJCOPY) -O binary $(BINDIR)/lib_ui_test.elf $(BINDIR)/lib_ui_test.bin
	@echo
	@echo Binary size: `ls -l $(BINDIR)/lib_ui_test.bin | awk '{print $$5}'` bytes

