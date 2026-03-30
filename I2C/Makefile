# --- Variables ---
CC = arm-none-eabi-gcc
CFLAGS = -c -mcpu=cortex-m4 -mthumb -std=gnu11 -DSTM32F401xE
INCLUDES = -IInc \
           -I/Users/anand/stm32_dev/CMSIS/Include \
           -I/Users/anand/stm32_dev/CMSIS/Device/ST/STM32F4xx/Include

# --- Build Rules ---
all: all.elf

# Rule for main.o
main.o: Src/main.c Inc/uart.h Inc/i2c.h Inc/lcd.h Inc/systick.h
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

# Rule for spi.o
spi.o: Src/spi.c Inc/spi.h
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

# Rule for i2c.o
i2c.o: Src/i2c.c Inc/i2c.h
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

# Rule for lcd.o
lcd.o: Src/lcd.c Inc/lcd.h Inc/i2c.h Inc/systick.h
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

# Rule for startup.o
stm32f411_startup.o: stm32f411_startup.c
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

# Linking everything together
all.elf: main.o gpio.o systick.o tim.o uart.o adc.o spi.o i2c.o lcd.o stm32f411_startup.o
	$(CC) -nostartfiles -T stm32_ls.ld main.o gpio.o systick.o tim.o uart.o adc.o spi.o i2c.o lcd.o stm32f411_startup.o -o all.elf --specs=nano.specs --specs=nosys.specs -Wl,-Map=all.map

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
