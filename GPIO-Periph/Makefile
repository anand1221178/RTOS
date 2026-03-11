# --- Variables ---
CC = arm-none-eabi-gcc
CFLAGS = -c -mcpu=cortex-m4 -mthumb -std=gnu11 -DSTM32F401xE

# Include Paths (Where the .h files are)
# Added -IInc to find your gpio.h
INCLUDES = -IInc \
           -I/Users/anand/stm32_dev/CMSIS/Include \
           -I/Users/anand/stm32_dev/CMSIS/Device/ST/STM32F4xx/Include

# --- Build Rules ---
final: cmsis.elf

# Rule for main.o
main.o: Src/main.c
	$(CC) $(CFLAGS) $(INCLUDES) Src/main.c -o main.o

# Rule for gpio.o
gpio.o: Src/gpio.c
	$(CC) $(CFLAGS) $(INCLUDES) Src/gpio.c -o gpio.o

# Rule for startup.o (stays in root based on your tree)
stm32f411_startup.o: stm32f411_startup.c
	$(CC) $(CFLAGS) $(INCLUDES) stm32f411_startup.c -o stm32f411_startup.o

# Linking everything together
cmsis.elf: main.o gpio.o stm32f411_startup.o
	$(CC) -nostdlib -T stm32_ls.ld *.o -o 4_makefile_project.elf -Wl,-Map=4_makefile_project.map

flash:
	arm-none-eabi-gdb -batch \
	-ex "target remote localhost:3333" \
	-ex "monitor reset init" \
	-ex "monitor flash write_image erase 4_makefile_project.elf" \
	-ex "monitor reset halt" \
	-ex "monitor resume"

load:
	openocd -f board/st_nucleo_f4.cfg

clean:
	rm -f *.o *.elf *.map