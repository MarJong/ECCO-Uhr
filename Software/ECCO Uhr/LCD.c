/*
 * LCD.c
 *
 * Created: 17.05.2012 16:41:52
 *  Author: Marcel
 */ 

#include <avr/io.h>
#include <util/delay.h>

#include "LCD.h"
#include "font.h"
/*
LCD intern
\ -> Y Adresse
| B B	...	B
v Y Y		Y
X T T		T
  E E		E
A . .		.
d 1 2		64
r . .		.
e . .		.
s B
s Y
e T
------------------
Start oben links (y = 0, x = 0)
64 Byte ausgeben (y = 63, x = 0)
x + 1 / 1 Zeile runter (y = 0, x = 1)
64 byte ausgeben (y = 63, x = 1)
...
bis y = 63, x = 7

x/y im programm vertauscht
*/

/*
 128 x 64 bit; 64 x 64 pro CS
*/
#define LCD_X_MAX		128 // 128 bit breit
#define LCD_Y_MAX		8   // 8 byte hoch (=64 bit)

volatile uint8_t lcd_buf[LCD_X_MAX][LCD_Y_MAX];
volatile uint8_t lcd_x, lcd_y, lcd_flags;

// befehls buffer
#define LCD_CMD_BUF_MAX			10
#define LCD_CMD_BUF_NONE		0	// keine befehle = zeichen dass keine befehle vorliegen
#define LCD_CMD_BUF_ADD			1	// Adresse; x pos
#define LCD_CMD_BUF_PAGE		2	// Page; y pos
#define LCD_CMD_BUF_DISP		3	// Display start; z pos

#define LCD_CMD_BUF_CHIP1		0	// chip auswahl
#define LCD_CMD_BUF_CHIP2		1

volatile uint8_t lcd_cmd_buf_pos = 0, lcd_cmd_rpos = 0;	// schreibe pos, lese pos
struct {
	uint8_t chip;
	uint8_t cmd;
	uint8_t val;	
} lcd_cmd_buf[LCD_CMD_BUF_MAX];

void lcd_write(uint8_t val);

//LCD initialisieren
void lcd_init()
{	
	uint8_t i, ready;
	
	cbi(PLCDCLK, LCD_RESET); // reset low
	_delay_ms(100);
	sbi(PLCDCLK, LCD_RESET); // reset high

	// RESET flag abfragen
	DDRD_IN;
	
	do {
		ready = 0;
		LCD_CS1_ON;
		LCD_STATUS_MODE;				// status abfragen
		sbi(PLCDCLK, LCD_CLK);			// clock high
		LCD_CLK_DELAY;					// warten
		ready |= PINLCDDATA & (1 << 4);	// reset bit auslesen
		cbi(PLCDCLK, LCD_CLK);			// clock low
		LCD_CLK_DELAY;
		
		LCD_CS2_ON;
		LCD_STATUS_MODE;				// status abfragen
		sbi(PLCDCLK, LCD_CLK);			// clock high
		LCD_CLK_DELAY;					// warten
		ready |= PINLCDDATA & (1 << 4);	// reset bit auslesen
		cbi(PLCDCLK, LCD_CLK);			// clock low
		LCD_CLK_DELAY;
	} while(ready);
	
	DDRD_OUT;

	lcd_wait_busy_12();
	LCD_CS1_CS2_ON;
	LCD_CMD_MODE;
	lcd_write(LCD_CMD_ON); // beide anschalten
	_delay_ms(1);
	
	lcd_wait_busy_12();
	LCD_CS1_CS2_ON;
	LCD_CMD_MODE;
	lcd_write(LCD_CMD_DISP | 0); // display start line = 0
	_delay_ms(1);
	
	lcd_wait_busy_12();
	LCD_CS1_CS2_ON;
	LCD_CMD_MODE;
	lcd_write(LCD_CMD_SETADD | 0); // x Address = 0
	_delay_ms(1);

	lcd_wait_busy_12();
	LCD_CS1_CS2_ON;
	LCD_CMD_MODE;
	lcd_write(LCD_CMD_SETPAGE | 0); // y Address = 0
	_delay_ms(1);
	
	LCD_CS_OFF;
	lcd_flags = 0;
	lcd_cmd_buf_pos = 0;
	lcd_cmd_rpos = 0;
	for(i = 0; i < LCD_CMD_BUF_MAX; i++)
		lcd_cmd_buf[i].cmd = LCD_CMD_BUF_NONE;
}

void lcd_off()
{
	lcd_wait_busy_12();
	LCD_CS1_CS2_ON;
	LCD_CMD_MODE;
	lcd_write(LCD_CMD_OFF); // beide anschalten
}

// LCD leeren
void lcd_clear()
{
	uint8_t x, y;
	
	for(x = 0; x < LCD_X_MAX; x++)
	{
		for(y = 0; y < LCD_Y_MAX; y++)
		{
			lcd_buf[x][y] = 0;
		}
	}
	
	lcd_x = 0;
	lcd_y = 0;
}

void lcd_clear_cs1()
{
	uint8_t x, y;

	for(x = 0; x < 64; x++)
	{
		for(y = 0; y < LCD_Y_MAX; y++)
		{
			lcd_buf[x][y] = 0;
		}
	}
}

void lcd_clear_cs2()
{
	uint8_t x, y;

	for(x = 0; x < 64; x++)
	{
		for(y = 0; y < LCD_Y_MAX; y++)
		{
			lcd_buf[64 + x][y] = 0;
		}
	}
}

// redraw flags setzen

void lcd_redraw()
{
	if(isset(lcd_flags, LCD_FLAG_REDRAW_CS1))
	{
		sbi(lcd_flags, LCD_FLAG_REDRAW2_CS1);
	}
	else
	{
		sbi(lcd_flags, LCD_FLAG_REDRAW_CS1);
	}
	
	if(isset(lcd_flags, LCD_FLAG_REDRAW_CS2))
	{
		sbi(lcd_flags, LCD_FLAG_REDRAW2_CS2);
	}
	else
	{
		sbi(lcd_flags, LCD_FLAG_REDRAW_CS2);
	}
}

void lcd_redraw_cs1()
{
	if(isset(lcd_flags, LCD_FLAG_REDRAW_CS1))
	{
		sbi(lcd_flags, LCD_FLAG_REDRAW2_CS1);
	}
	else
	{
		sbi(lcd_flags, LCD_FLAG_REDRAW_CS1);
	}
}

void lcd_redraw_cs2()
{
	if(isset(lcd_flags, LCD_FLAG_REDRAW_CS2))
	{
		sbi(lcd_flags, LCD_FLAG_REDRAW2_CS2);
	}
	else
	{
		sbi(lcd_flags, LCD_FLAG_REDRAW_CS2);
	}
}

// befehl hinzufügen
void lcd_add_cmd(uint8_t chip, uint8_t cmd, uint8_t val)
{
	lcd_cmd_buf[lcd_cmd_buf_pos].cmd = cmd;
	lcd_cmd_buf[lcd_cmd_buf_pos].chip = chip;
	lcd_cmd_buf[lcd_cmd_buf_pos].val = val;
	lcd_cmd_buf_pos++;
	if(lcd_cmd_buf_pos >= LCD_CMD_BUF_MAX)
		lcd_cmd_buf_pos = 0;
	lcd_cmd_buf[lcd_cmd_buf_pos].cmd = LCD_CMD_BUF_NONE;
}

// befehl ausführen
void lcd_exec_cmd()
{
	if(lcd_cmd_buf[lcd_cmd_rpos].cmd == LCD_CMD_BUF_NONE)
		return;
		
	switch(lcd_cmd_buf[lcd_cmd_rpos].chip)
	{
	case LCD_CMD_BUF_CHIP1:
		LCD_CS1_ON;
		break;
	case LCD_CMD_BUF_CHIP2:
		LCD_CS2_ON;
		break;
	default:
		lcd_cmd_buf[lcd_cmd_rpos].cmd = LCD_CMD_BUF_NONE;
		return;
	}
	
	LCD_CMD_MODE;
	
	switch(lcd_cmd_buf[lcd_cmd_rpos].cmd)
	{
	case LCD_CMD_BUF_ADD:	// x adresse
		lcd_write(LCD_CMD_SETADD | lcd_cmd_buf[lcd_cmd_rpos].val);
		break;
	case LCD_CMD_BUF_DISP:	// display start
		lcd_write(LCD_CMD_DISP | lcd_cmd_buf[lcd_cmd_rpos].val);
		break;
	case LCD_CMD_BUF_PAGE:	// y adresse
		lcd_write(LCD_CMD_SETPAGE | lcd_cmd_buf[lcd_cmd_rpos].val);
		break;
	}
	
	lcd_cmd_buf[lcd_cmd_rpos].cmd = LCD_CMD_BUF_NONE;
	
	lcd_cmd_rpos++;
	if(lcd_cmd_rpos >= LCD_CMD_BUF_MAX)
		lcd_cmd_rpos = 0;
}

// ausgabe in schritten
void lcd_step()
{
	// timer zum mitzählen der aktionen
	// jede aktion erhält wert entsprechend des aufwandes/zeit
	// -> zb busy check = 15; write = 20 etc
	// notfalls weiter senken bei neuen funktionen 
	uint16_t timer = 0;
	static uint8_t pos_x = 0, pos_y = 0, chip = 0, pos_cmd = 0;

	while(timer < 50)
	{	
		// befehle auswerten
		if(lcd_cmd_buf[lcd_cmd_rpos].cmd != LCD_CMD_BUF_NONE)
		{
			if(lcd_cmd_buf[lcd_cmd_rpos].chip == LCD_CMD_BUF_CHIP1)
			{
				LCD_CS1_ON;
			}
			else
			{
				LCD_CS2_ON;
			}
			
			if(lcd_is_busy())
			{
				timer += 15;
				continue;
			}
			
			lcd_exec_cmd();
			timer += 15;
			continue;
		}
		
		// daten ausgeben
		if(chip == 0)
		{
			if(!isset(lcd_flags, LCD_FLAG_REDRAW_CS1))	// leerlauf für cs1
			{
				chip = 1;
				return;
			}
			
			if(lcd_cs1_is_busy())  // busy -> warten
			{
				timer += 15;
				continue;
			}
			
			LCD_CS1_ON;
			LCD_DATA_W_MODE;
			lcd_write(lcd_buf[pos_x][pos_y]);
			timer += 20;
			pos_x++;
			if(pos_x >= 64)
			{
				pos_x = 0;
				pos_y++;
				if(pos_y >= 8)
				{
					pos_y = 0;
					chip = 1;
					if(isset(lcd_flags, LCD_FLAG_REDRAW2_CS1))
					{
						cbi(lcd_flags, LCD_FLAG_REDRAW2_CS1);
					}
					else
					{
						cbi(lcd_flags, LCD_FLAG_REDRAW_CS1);
					}
				}
				
				lcd_add_cmd(LCD_CMD_BUF_CHIP1, LCD_CMD_BUF_PAGE, pos_y);
				timer += 5;
				continue;
			}
		}
		else if(chip == 1)
		{

			if(!isset(lcd_flags, LCD_FLAG_REDRAW_CS2))
			{
				chip = 0;
				return;
			}
			
			if(lcd_cs2_is_busy())  // busy -> warten
			{
				timer += 15;
				continue;
			}		
			
			LCD_CS2_ON;
			LCD_DATA_W_MODE;
			lcd_write(lcd_buf[pos_x + 64][pos_y]);
			timer += 20;
			pos_x++;
			if(pos_x >= 64)
			{
				pos_x = 0;
				pos_y++;
				if(pos_y >= 8)
				{
					pos_y = 0;
					chip = 0;
					if(isset(lcd_flags, LCD_FLAG_REDRAW2_CS2))
					{
						cbi(lcd_flags, LCD_FLAG_REDRAW2_CS2);
					}
					else
					{
						cbi(lcd_flags, LCD_FLAG_REDRAW_CS2);
					}
				}
				
				lcd_add_cmd(LCD_CMD_BUF_CHIP2, LCD_CMD_BUF_PAGE, pos_y);
				timer += 5;
				continue;
			}			
		}
		else
		{
			chip = 0;
		}

	}
}

// übernimmt byte 1:1 in buffer
void lcd_put(uint8_t c)
{
	lcd_buf[lcd_x][lcd_y] = c;
	lcd_x++;
	if(lcd_x >= LCD_X_MAX)
	{
		lcd_x = 0;
		lcd_y++;
		if(lcd_y >= LCD_Y_MAX)
		{
			lcd_y = 0;
		}
	}
}

// char schreiben
void lcd_putc(uint8_t c, uint8_t invert)
{
	uint8_t i;
	
	if(lcd_x + FONT_WIDTH >= LCD_X_MAX)
	{
		lcd_x = 0;
		lcd_y++;
		if(lcd_y >= LCD_Y_MAX)
			lcd_y = 0;
	}

	for(i = 0; i < FONT_WIDTH; i++)
	{
		if(invert)
			lcd_put(~pgm_read_byte(&font[c][i]));
		else
			lcd_put(pgm_read_byte(&font[c][i]));

/*		if(invert)
			lcd_put(~font[c][i]);
		else
			lcd_put(font[c][i]);*/
	}		
}

// string schreiben
void lcd_putstr(char *str, uint8_t invert)
{
	uint8_t i = 0;
	
	while(str[i] != '\0')
	{
		lcd_putc(str[i], invert);
		i++;
	}
}

void lcd_putstrbig(char *str, uint8_t invert)
{
	uint8_t i = 0, j, x, y, c;
	
	y = lcd_y;
	x = lcd_x;
	
	while(str[i] != '\0')
	{
		c = str[i] - 0x30;
		if(c > 10)
			continue;
			
		for(j = 0; j < FONT_BIG_WIDTH; j += 3)
		{
			lcd_buf[x][y] = pgm_read_byte(&font_big[c][j]);
			lcd_buf[x][y + 1] = pgm_read_byte(&font_big[c][j + 1]);
			lcd_buf[x][y + 2] = pgm_read_byte(&font_big[c][j + 2]);
			x++;
		}
			
		y = lcd_y;
		lcd_x += 16;
		
		i++;	
	}
}

void lcd_setpos(uint8_t x, uint8_t y)
{
	if(x < LCD_X_MAX)
		lcd_x = x;
	else
		lcd_x = 0;
	
	if(y < LCD_Y_MAX)
		lcd_y = y;
	else
		lcd_y = 0;
}

// auf beide chips warten
// nur für init!
void lcd_wait_busy_12()
{
	while(lcd_cs1_is_busy() || lcd_cs2_is_busy())
		;
}

// gibt daten aus und clock signal
// !! vorher chip wählen, modus setzen !!
void lcd_write(uint8_t val)
{
	PLCDDATA = val;
	nop; nop; nop; nop; nop; nop;
	sbi(PLCDCLK, LCD_CLK);
	LCD_CLK_DELAY;
	cbi(PLCDCLK, LCD_CLK);
	LCD_CLK_DELAY;
}

// busy abfrage für aktuellen chip
uint8_t lcd_is_busy()
{
	uint8_t isbusy = 0;
	
	DDRD_IN;
	
	LCD_STATUS_MODE;				// status abfragen
	nop; nop; nop; nop; nop; nop;
	sbi(PLCDCLK, LCD_CLK);			// clock high
	LCD_CLK_DELAY;					// warten
	isbusy = PINLCDDATA & (1 << 7);	// busy bit auslesen
	cbi(PLCDCLK, LCD_CLK);			// clock low
	LCD_CLK_DELAY;
	
	DDRD_OUT;
	
	return isbusy;
}

// busy abfrage für cs1
uint8_t lcd_cs1_is_busy()
{
	uint8_t isbusy;
	
	LCD_CS1_ON;
	isbusy = lcd_is_busy();
	
	return isbusy;
}

// busy abfrage für cs2
uint8_t lcd_cs2_is_busy()
{
	uint8_t isbusy;
	
	LCD_CS2_ON;
	isbusy = lcd_is_busy();
	
	return isbusy;
}

uint8_t lcd_need_redraw()
{
	return isset(lcd_flags, LCD_FLAG_REDRAW_CS1) | isset(lcd_flags, LCD_FLAG_REDRAW_CS2);
}

// test der font
void lcd_test_font(uint8_t start)
{
	uint16_t i;
	uint8_t z;
	
	z = 0;
	
	lcd_clear();
	
	for(i = start; i < 256; i++)
	{
		if(lcd_x + FONT_WIDTH >= LCD_X_MAX)
		{
			z++;
			if(z >= LCD_Y_MAX)
				return;
			lcd_setpos(0, z);
		}
		lcd_putc(i, 0);
	}
}

void lcd_show_gecco()
{
	uint16_t i, j;
	uint8_t x = 32, y = 0;
	
	for(i = 0; i < 512; i += 8)
	{	
		for(j = 0; j < 8; j++)
		{
			lcd_buf[x][y + j] = pgm_read_byte(&font_gecco[i + j]);
		}
		x++;		
	}
	
	lcd_redraw();
	while(isset(lcd_flags, LCD_FLAG_REDRAW_CS1) || isset(lcd_flags, LCD_FLAG_REDRAW_CS2))
		lcd_step();
}

// debug
void lcd_reset_disp()
{
	lcd_add_cmd(LCD_CMD_BUF_CHIP1, LCD_CMD_BUF_DISP, 0);
	lcd_add_cmd(LCD_CMD_BUF_CHIP2, LCD_CMD_BUF_DISP, 0);
}