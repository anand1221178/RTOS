#include "lcd.h"
#include "i2c.h"
#include "systick.h"

/*
  PCF8574 pin mapping to HD44780:
  P0 = RS   (0=command, 1=character data)
  P1 = RW   (always 0 for write)
  P2 = EN   (pulse HIGH->LOW to latch data)
  P3 = Backlight
  P4 = D4 \
  P5 = D5  | 4-bit data bus
  P6 = D6  |
  P7 = D7 /

  To send one byte to the LCD we split it into two nibbles:
  1. Upper nibble (bits 7-4) goes on P4-P7, pulse EN
  2. Lower nibble (bits 3-0) shifted to P4-P7, pulse EN
*/

/*Send a single 4-bit nibble to the LCD via I2C*/
static void lcd_write_nibble(uint8_t nibble, uint8_t rs)
{
    /* nibble is already in upper 4 bits (P4-P7 position)
       rs = 0 for command, LCD_RS (0x01) for data
       Always keep backlight on */
    uint8_t data = nibble | rs | LCD_BACKLIGHT;

    /* Pulse EN: write with EN high, short delay, then EN low */
    i2c1_write_no_reg(LCD_ADDR, data | LCD_EN);  /* EN = 1 */
    systick_msec_delay(1);
    i2c1_write_no_reg(LCD_ADDR, data & ~LCD_EN); /* EN = 0 */
    systick_msec_delay(1);
}


/* TODO(human): Implement lcd_send_byte
 *
 * This function sends a full 8-bit value to the LCD using two nibble writes.
 * Parameters:
 *   - byte: the 8-bit value to send (a command like 0x01, or a char like 'H')
 *   - rs:   0 for command, LCD_RS for character data
 *
 * Remember:
 *   - The LCD is in 4-bit mode, so you must split the byte into two halves
 *   - Upper nibble first (bits 7-4), then lower nibble (bits 3-0)
 *   - The nibble must be in the upper 4 bits of the byte you pass to
 *     lcd_write_nibble (because P4-P7 are bits 4-7 of the PCF8574)
 *   - Use lcd_write_nibble() for each half
 */
static void lcd_send_byte(uint8_t byte, uint8_t rs)
{
    lcd_write_nibble(byte & 0xF0, rs);        /* upper nibble: mask off lower 4 bits */
    lcd_write_nibble((byte << 4) & 0xF0, rs); /* lower nibble: shift up into position */
}


void lcd_send_cmd(uint8_t cmd)
{
    lcd_send_byte(cmd, 0);      /* rs=0 means command */
}

void lcd_send_char(uint8_t ch)
{
    lcd_send_byte(ch, LCD_RS);  /* rs=1 means character data */
}

void lcd_send_string(const char *str)
{
    while (*str)
    {
        lcd_send_char(*str++);
    }
}

void lcd_set_cursor(uint8_t row, uint8_t col)
{
    uint8_t addr = (row == 0) ? LCD_CMD_SET_LINE1 : LCD_CMD_SET_LINE2;
    lcd_send_cmd(addr + col);
}

void lcd_clear(void)
{
    lcd_send_cmd(LCD_CMD_CLEAR);
    systick_msec_delay(2);  /* clear command needs extra time */
}

void lcd_init(void)
{
    /* Wait for LCD to power up */
    systick_msec_delay(50);

    /* HD44780 initialization sequence for 4-bit mode
       Must send 0x30 three times, then 0x20 to enter 4-bit mode */
    lcd_write_nibble(0x30, 0);
    systick_msec_delay(5);
    lcd_write_nibble(0x30, 0);
    systick_msec_delay(1);
    lcd_write_nibble(0x30, 0);
    systick_msec_delay(1);
    lcd_write_nibble(0x20, 0);  /* now in 4-bit mode */
    systick_msec_delay(1);

    /* Now we can use lcd_send_cmd (sends two nibbles) */
    lcd_send_cmd(LCD_CMD_FUNCTION);   /* 4-bit, 2 lines, 5x8 */
    lcd_send_cmd(LCD_CMD_DISPLAY_ON); /* display on, cursor off */
    lcd_send_cmd(LCD_CMD_CLEAR);      /* clear display */
    systick_msec_delay(2);
    lcd_send_cmd(LCD_CMD_ENTRY_MODE); /* cursor moves right */
}

void lcd_print_number(int num)
{
    char buffer[10];
    int i = 0;

    if (num == 0)
    {
        lcd_send_char('0');
        return;
    }

    while (num > 0)
    {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    while (i > 0)
    {
        lcd_send_char(buffer[--i]);
    }
}
