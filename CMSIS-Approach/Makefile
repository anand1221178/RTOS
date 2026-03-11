# --- Variables ---
CC = arm-none-eabi-gcc
CFLAGS = -c -mcpu=cortex-m4 -mthumb -std=gnu11 -DSTM32F401xE
INCLUDES = -I ~/stm32_dev/CMSIS/Include -I ~/stm32_dev/CMSIS/Device/ST/STM32F4xx/Include

# --- Build Rules ---
final : cmsis.elf

main.o : main.c
	$(CC) $(CFLAGS) $(INCLUDES) main.c -o main.o

stm32f411_startup.o : stm32f411_startup.c
	$(CC) $(CFLAGS) $(INCLUDES) stm32f411_startup.c -o stm32f411_startup.o

cmsis.elf : main.o stm32f411_startup.o
	$(CC) -nostdlib -T stm32_ls.ld *.o -o cmsis.elf -Wl,-Map=cmsis.map

load :
	openocd -f board/st_nucleo_f4.cfg

clean:
	rm -f *.o *.elf *.map

flash:
	arm-none-eabi-gdb -batch \
	-ex "target remote localhost:3333" \
	-ex "monitor reset init" \
	-ex "monitor flash write_image erase cmsis.elf" \
	-ex "monitor reset halt" \
	-ex "monitor resume"