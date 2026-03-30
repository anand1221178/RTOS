#ifndef LCD_H_
#define LCD_H_

#include "stdint.h"

#define LCD_ADDR  0x27    /* PCF8574 default I2C address */
#define LCD_BACKLIGHT  0x08
#define LCD_EN         0x04
#define LCD_RS         0x01

/* HD44780 commands */
#define LCD_CMD_CLEAR       0x01
#define LCD_CMD_HOME        0x02
#define LCD_CMD_ENTRY_MODE  0x06   /* cursor moves right, no shift */
#define LCD_CMD_DISPLAY_ON  0x0C   /* display on, cursor off */
#define LCD_CMD_FUNCTION    0x28   /* 4-bit mode, 2 lines, 5x8 font */
#define LCD_CMD_SET_LINE1   0x80   /* DDRAM address for line 1, col 0 */
#define LCD_CMD_SET_LINE2   0xC0   /* DDRAM address for line 2, col 0 */

void lcd_init(void);
void lcd_send_cmd(uint8_t cmd);
void lcd_send_char(uint8_t ch);
void lcd_send_string(const char *str);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_clear(void);
void lcd_print_number(int num);

#endif
