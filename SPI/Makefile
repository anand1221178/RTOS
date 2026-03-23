# --- Variables ---
CC = arm-none-eabi-gcc
CFLAGS = -c -mcpu=cortex-m4 -mthumb -std=gnu11 -DSTM32F401xE
INCLUDES = -IInc \
           -I/Users/anand/stm32_dev/CMSIS/Include \
           -I/Users/anand/stm32_dev/CMSIS/Device/ST/STM32F4xx/Include

# --- Build Rules ---
# Using 'all' as the default target is standard practice
all: all.elf

# Rule for main.o - Depends on Src AND Inc
main.o: Src/main.c Inc/gpio.h Inc/systick.h Inc/tim.h
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

# Rule for gpio.o
gpio.o: Src/gpio.c Inc/gpio.h
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

# Rule for systick.o
systick.o: Src/systick.c Inc/systick.h
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

# Rule for tim.o
tim.o: Src/tim.c Inc/tim.h
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

# Rule for uart.o
uart.o: Src/uart.c Inc/uart.h
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

#rule for adc
adc.o: Src/adc.c Inc/adc.h
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

# Rule for startup.o
stm32f411_startup.o: stm32f411_startup.c
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

# Linking everything together
# We explicitly list the objects so 'make' knows to link when any .o changes
all.elf: main.o gpio.o systick.o tim.o uart.o adc.o stm32f411_startup.o
	$(CC) -nostartfiles -T stm32_ls.ld main.o gpio.o systick.o tim.o uart.o adc.o stm32f411_startup.o -o all.elf --specs=nano.specs --specs=nosys.specs -Wl,-Map=all.map 
flash:
	arm-none-eabi-gdb -batch \
	-ex "target remote localhost:3333" \
	-ex "monitor reset init" \
	-ex "monitor flash write_image erase all.elf" \
	-ex "monitor reset halt" \
	-ex "monitor resume"

load:
	openocd -f board/st_nucleo_f4.cfg

clean:
	rm -f *.o *.elf *.map