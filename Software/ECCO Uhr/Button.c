/*
 * Button.c
 *
 * Created: 17.05.2012 16:43:37
 *  Author: Marcel
 */ 

#include <avr/io.h>
#include "ECCO Uhr.h"
#include "Button.h"

uint8_t btn_down, btn_up, btn_hold; // steigende flanke, fallende flanke, gedr�ckt

void button_init()
{
	btn_down = 0;
	btn_hold = 0;
	btn_up = 0;
}

void button_poll()
{
	static uint8_t save_ctn = 0, select_ctn = 0, plus_ctn = 0, minus_ctn = 0; // z�hler
	
	if(!isset(PINBUTTON, BTN_PLUS)) // gedr�ck
	{
		if(save_ctn < BUTTON_MIN_DOWN)
			save_ctn++;
		else
		{
			if(!isset(btn_hold, BUTTON_PLUS))
			{
				sbi(btn_down, BUTTON_PLUS);
				sbi(btn_hold, BUTTON_PLUS);
			}
			else
				cbi(btn_down, BUTTON_PLUS);
		}
	}
	else // nicht gedr�ck
	{
		cbi(btn_down, BUTTON_PLUS);
		if(isset(btn_hold, BUTTON_PLUS))
		{
			sbi(btn_up, BUTTON_PLUS);
			save_ctn = 0;
		}		
		else
			cbi(btn_up, BUTTON_PLUS);
		cbi(btn_hold, BUTTON_PLUS);
	}
	
	if(!isset(PINBUTTON, BTN_MINUS)) // gedr�ck
	{
		if(save_ctn < BUTTON_MIN_DOWN)
			save_ctn++;
		else
		{
			if(!isset(btn_hold, BUTTON_MINUS))
			{
				sbi(btn_down, BUTTON_MINUS);
				sbi(btn_hold, BUTTON_MINUS);
			}
			else
				cbi(btn_down, BUTTON_MINUS);
		}
	}
	else // nicht gedr�ck
	{
		cbi(btn_down, BUTTON_MINUS);
		if(isset(btn_hold, BUTTON_MINUS))
		{
			sbi(btn_up, BUTTON_MINUS);
			save_ctn = 0;
		}		
		else
			cbi(btn_up, BUTTON_MINUS);
		cbi(btn_hold, BUTTON_MINUS);
	}
	
	if(!isset(PINBUTTON, BTN_SAVE)) // gedr�ck
	{
		if(save_ctn < BUTTON_MIN_DOWN)
			save_ctn++;
		else
		{
			if(!isset(btn_hold, BUTTON_SAVE))
			{
				sbi(btn_down, BUTTON_SAVE);
				sbi(btn_hold, BUTTON_SAVE);
			}
			else
				cbi(btn_down, BUTTON_SAVE);
		}
	}
	else // nicht gedr�ck
	{
		cbi(btn_down, BUTTON_SAVE);
		if(isset(btn_hold, BUTTON_SAVE))
		{
			sbi(btn_up, BUTTON_SAVE);
			save_ctn = 0;
		}		
		else
			cbi(btn_up, BUTTON_SAVE);
		cbi(btn_hold, BUTTON_SAVE);
	}
	
	if(!isset(PINBUTTON, BTN_SELECT)) // gedr�ck
	{
		if(save_ctn < BUTTON_MIN_DOWN)
			save_ctn++;
		else
		{
			if(!isset(btn_hold, BUTTON_SELECT))
			{
				sbi(btn_down, BUTTON_SELECT);
				sbi(btn_hold, BUTTON_SELECT);
			}
			else
				cbi(btn_down, BUTTON_SELECT);
		}
	}
	else // nicht gedr�ck
	{
		cbi(btn_down, BUTTON_SELECT);
		if(isset(btn_hold, BUTTON_SELECT))
		{
			sbi(btn_up, BUTTON_SELECT);
			save_ctn = 0;
		}		
		else
			cbi(btn_up, BUTTON_SELECT);
		cbi(btn_hold, BUTTON_SELECT);
	}
}

uint8_t button_is_down(uint8_t btn)
{
	return (btn_down & (1 << btn));
}

uint8_t button_is_up(uint8_t btn)
{
	return (btn_up & (1 << btn));
}

uint8_t button_is_hold(uint8_t btn)
{
	return (btn_hold & (1 << btn));
}