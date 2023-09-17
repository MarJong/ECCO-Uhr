/*
 * LCD.h
 *
 * Created: 19.05.2012 22:19:54
 *  Author: Marcel
 */ 

#ifndef LCD_H_
#define LCD_H_

#include "ECCO Uhr.h"
#include <util/delay.h>

#define LCD_DELAY_500ns	{nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;}

#define LCD_CS1_ON		{sbi(PLCDCTRL, LCD_CS1); cbi(PLCDCTRL, LCD_CS2); LCD_DELAY_500ns;}
#define LCD_CS2_ON		{sbi(PLCDCTRL, LCD_CS2); cbi(PLCDCTRL, LCD_CS1); LCD_DELAY_500ns;}
#define LCD_CS1_CS2_ON	{sbi(PLCDCTRL, LCD_CS1); sbi(PLCDCTRL, LCD_CS2); LCD_DELAY_500ns;}
#define LCD_CS_OFF		{cbi(PLCDCTRL, LCD_CS1); cbi(PLCDCTRL, LCD_CS2); LCD_DELAY_500ns;}
	
#define LCD_CMD_MODE	{cbi(PLCDCTRL, LCD_RS); LCD_DELAY_500ns; cbi(PLCDCTRL, LCD_RW); LCD_DELAY_500ns;}
#define LCD_STATUS_MODE	{cbi(PLCDCTRL, LCD_RS); LCD_DELAY_500ns; sbi(PLCDCTRL, LCD_RW); LCD_DELAY_500ns;}
	
#define LCD_DATA_R_MODE	{sbi(PLCDCTRL, LCD_RS); LCD_DELAY_500ns; sbi(PLCDCTRL, LCD_RW); LCD_DELAY_500ns;}
#define LCD_DATA_W_MODE	{sbi(PLCDCTRL, LCD_RS); LCD_DELAY_500ns; cbi(PLCDCTRL, LCD_RW); LCD_DELAY_500ns;}

#define LCD_CLK_DELAY	{_delay_us(1);}

// Befehle für CMD Mode
#define LCD_CMD_ON		0x3F
#define LCD_CMD_OFF		0x3E
#define LCD_CMD_SETADD	0x40
#define LCD_CMD_SETPAGE 0xB8
#define LCD_CMD_DISP	0xC0

// Flags
#define LCD_FLAG_REDRAW_CS1		0
#define LCD_FLAG_REDRAW_CS2		1
#define LCD_FLAG_CS1_BUSY		2
#define LCD_FLAG_CS2_BUSY		3
#define LCD_FLAG_REDRAW2_CS1	4
#define LCD_FLAG_REDRAW2_CS2	5

// busy checks
uint8_t lcd_is_busy();
uint8_t lcd_cs1_is_busy();
uint8_t lcd_cs2_is_busy();
void lcd_wait_busy_12();

// init
void lcd_init();
void lcd_clear();
void lcd_clear_cs1();
void lcd_clear_cs2();
uint8_t lcd_need_redraw();

// zeichnen
void lcd_redraw();
void lcd_redraw_cs1();
void lcd_redraw_cs2();

// zeichenschritt
void lcd_step();

// ausschalten
void lcd_off();

// befehle
void lcd_add_cmd(uint8_t chip, uint8_t cmd, uint8_t val);
void lcd_exec_cmd();

// in buffer schreiben
void lcd_put(uint8_t c);								// übernimmt c 1:1
void lcd_setpos(uint8_t x, uint8_t y);					// position setzen

void lcd_putc(uint8_t c, uint8_t invert);				// char schreiben
void lcd_putstr(char *str, uint8_t invert);				// string schreiben


//void lcd_putstrxy(uint8_t x, uint8_t y, char *str, uint8_t invert);				// pos setzen + string schreiben

// nur für zahlen und ':'
void lcd_putstrbig(char *str, uint8_t invert);

// zum testen
void lcd_test_font(uint8_t start);
void lcd_show_gecco();

#endif /* LCD_H_ */