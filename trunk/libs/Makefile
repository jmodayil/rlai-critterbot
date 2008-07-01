CC = arm-elf-gcc
LD = arm-elf-ld

CFLAGS = -c -mcpu=arm7tdmi -Wall

OBJLIB = lib_spi.o lib_ledctl.o lib_ssc.o lib_ui.o

all: libs.s19

# For now, we compile everything using gcc because inline functions are dup'ed
libs.s19:
	$(CC) $(CFLAGS) -mthumb lib_spi.c lib_ui.c lib_ssc.c lib_ledctl.c
#libs.s19: $(OBJLIB)
#	$(LD) --oformat elf32-littlearm -o libs.elf $(OBJLIB)

lib_spi.o: lib_spi.c
	$(CC) $(CFLAGS) -mthumb lib_spi.c -o lib_spi.o

lib_ssc.o: lib_ssc.c
	$(CC) $(CFLAGS) -mthumb lib_ssc.c -o lib_ssc.o

lib_ui.o: lib_ui.c
	$(CC) $(CFLAGS) -mthumb lib_ui.c -o lib_ui.o

lib_ledctl.o: lib_ledctl.c
	$(CC) $(CFLAGS) -mthumb lib_ledctl.c -o lib_ledctl.o

