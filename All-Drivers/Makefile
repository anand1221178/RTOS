# --- Variables ---
CC = arm-none-eabi-gcc
CFLAGS = -c -mcpu=cortex-m4 -mthumb -std=gnu11 -DSTM32F401xE

# Include Paths (Where the .h files are)
INCLUDES = -IInc \
           -I/Users/anand/stm32_dev/CMSIS/Include \
           -I/Users/anand/stm32_dev/CMSIS/Device/ST/STM32F4xx/Include

# --- Build Rules ---
final: systick.elf

# Rule for main.o
main.o: Src/main.c
	$(CC) $(CFLAGS) $(INCLUDES) Src/main.c -o main.o

# Rule for gpio.o
gpio.o: Src/gpio.c
	$(CC) $(CFLAGS) $(INCLUDES) Src/gpio.c -o gpio.o

# Rule for systick.o (NEW)
systick.o: Src/systick.c
	$(CC) $(CFLAGS) $(INCLUDES) Src/systick.c -o systick.o

# Rule for startup.o
stm32f411_startup.o: stm32f411_startup.c
	$(CC) $(CFLAGS) $(INCLUDES) stm32f411_startup.c -o stm32f411_startup.o

# Linking everything together (Unified to systick.elf)
systick.elf: main.o gpio.o systick.o stm32f411_startup.o
	$(CC) -nostdlib -T stm32_ls.ld *.o -o systick.elf -Wl,-Map=systick.map

flash:
	arm-none-eabi-gdb -batch \
	-ex "target remote localhost:3333" \
	-ex "monitor reset init" \
	-ex "monitor flash write_image erase systick.elf" \
	-ex "monitor reset halt" \
	-ex "monitor resume"

load:
	openocd -f board/st_nucleo_f4.cfg

clean:
	rm -f *.o *.elf *.map