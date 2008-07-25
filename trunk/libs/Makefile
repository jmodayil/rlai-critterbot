CC = arm-elf-gcc
LD = arm-elf-ld
OBJCOPY = arm-elf-objcopy

# Directories
LDDIR=ldscripts
BINDIR=bin
GCC_LIB_DIR=~/Desktop/gnuarm-4.0.2/lib/gcc/arm-elf/4.0.2/
STDC_LIB_DIR=~/Desktop/gnuarm-4.0.2/arm-elf/lib/

LIBS=$(GCC_LIB_DIR)/libgcc.a $(STDC_LIB_DIR)/libc.a

# Uncomment to produce a memory map
# PRODUCE_MEMORY_MAP=-Wl,-Map=memory.map
PRODUCE_MEMORY_MAP=
PRODUCE_MEMORY_MAP_LD=-Map=memory.map
#PRODUCE_MEMORY_MAP_LD=

# Compiler flags
COMMONFLAGS = -c -mcpu=arm7tdmi
AFLAGS = -x assembler-with-cpp
CFLAGS = -Winline -O3 -ffunction-sections -fdata-sections
# Set DEBUGFLAGS to empty for normal compilation
# DEBUGFLAGS =
DEBUGFLAGS = -Wall

# Linker flags
LDFLAGS = --gc-sections --oformat elf32-littlearm $(PRODUCE_MEMORY_MAP_LD)

THUMB = -marm

# Lib files to compile
CSRC = lib_spi.c lib_ui.c lib_ssc.c lib_ledctl.c lib_error.c lib_accel.c armio.c lib_events.c
COBJ = $(CSRC:.c=.o)
ASRC = startup_SAM7S.s
AOBJ = $(ASRC:.s=.o)

# Test targets
TESTBINS = $(BINDIR)/lib_ui_test.bin $(BINDIR)/main_test.bin

# Builds libraries (but nothing else)
all: all_libs

# Builds all tests
tests: $(TESTBINS)

clean:
	rm -f *.o
	rm -f $(BINDIR)/*

# For now, we compile everything using gcc because inline functions are dup'ed
all_libs: $(AOBJ) $(COBJ)
#	$(CC) $(CFLAGS) $(THUMB) $(CSRC)

# Test binaries
$(TESTBINS): all_libs $(notdir $(TESTBINS:.bin=.o))
	$(LD) -T$(LDDIR)/$(notdir $(addsuffix .ld, $(basename $@))) $(LDFLAGS) $(COBJ) $(AOBJ) $(LIBS) $(notdir $(addsuffix .o, $(basename $@))) --output $(addsuffix .elf, $(basename $@))
#	@echo $@ $(basename $@)
	$(OBJCOPY) -O binary $(addsuffix .elf, $(basename $@)) $@
#	$(LD) -T$(LDDIR)/lib_ui_test.ld $(LDFLAGS) $(COBJ) $(AOBJ) $(LIBS) lib_ui_test.o --output $(BINDIR)/lib_ui_test.elf
#	$(OBJCOPY) -O binary $(BINDIR)/lib_ui_test.elf $(BINDIR)/lib_ui_test.bin
	@echo
	@echo Binary size: `ls -l $@ | awk '{print $$5}'` bytes

$(COBJ): %.o : %.c
	@echo $<
	$(CC) $(COMMONFLAGS) $(CFLAGS) $(THUMB) $(DEBUGFLAGS) $< -o $@

$(notdir $(TESTBINS:.bin=.o)): %.o : %.c
	@echo $<
	$(CC) $(COMMONFLAGS) $(CFLAGS) $(THUMB) $(DEBUGFLAGS) $< -o $@

$(AOBJ): %.o : %.s
	arm-elf-gcc $(COMMONFLAGS) $(AFLAGS) $(THUMB) $(INCLUDES) $(DEBUGFLAGS) -Wall $< -o $@

