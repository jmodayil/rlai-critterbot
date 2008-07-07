CC = arm-elf-gcc
LD = arm-elf-ld

CFLAGS = -c -mcpu=arm7tdmi -Winline -O3

THUMB = -marm

OBJLIB = lib_spi.o lib_ledctl.o lib_ssc.o lib_ui.o armio.o
SRC = lib_spi.c lib_ui.c lib_ssc.c lib_ledctl.c armio.c

all: all_libs

clean:
	rm *.o

# For now, we compile everything using gcc because inline functions are dup'ed
all_libs:
	$(CC) $(CFLAGS) $(THUMB) $(SRC)

lib_ui_test:
	@echo not done yet!
	$(LD) -T$(LDDIR)/lib_ui_test.ld --oformat elf32-littlearm $(OBJLIB)

#libs.s19: $(OBJLIB)
#	$(LD) --oformat elf32-littlearm -o libs.elf $(OBJLIB)

lib_spi.o: lib_spi.c
	$(CC) $(CFLAGS) $(THUMB) lib_spi.c -o lib_spi.o

lib_ssc.o: lib_ssc.c
	$(CC) $(CFLAGS) $(THUMB) lib_ssc.c -o lib_ssc.o

lib_ui.o: lib_ui.c
	$(CC) $(CFLAGS) $(THUMB) lib_ui.c -o lib_ui.o

lib_ledctl.o: lib_ledctl.c
	$(CC) $(CFLAGS) $(THUMB) lib_ledctl.c -o lib_ledctl.o

