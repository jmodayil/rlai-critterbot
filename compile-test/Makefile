# A primitive Makefile until I have a better grasp of how things work
#
# It assumes that the object files are in the current directory
#  You can get object files by compiling the ../libs directory
# It will also compile the startup assembly file for you.

THUMB=-mthumb-interwork

all: armcode.hex

# Object translation
armcode.hex: armcode.elf
	arm-elf-objcopy -O ihex armcode.elf armcode.hex

# Object linking
# Can alternatively be done using arm-elf-gcc, which provides crt0 and such
armcode.elf: startup_SAM7S.o
	arm-elf-ld -Tarmcode.ld --oformat elf32-littlearm *.o --output armcode.elf

startup_SAM7S.o: startup_SAM7S.S
	arm-elf-gcc $(THUMB) -mcpu=arm7tdmi -c -Wall -x assembler-with-cpp startup_SAM7S.S
