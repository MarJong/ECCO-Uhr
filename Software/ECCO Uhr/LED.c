/*
 * LED.c
 *
 * Created: 17.05.2012 16:41:39
 *  Author: Marcel
 */ 

#include <avr/io.h>
#include <util/delay.h>

#include "LED.h"
#include "ECCO Uhr.h"

volatile led_t *zeiger_sek, *zeiger_min, *zeiger_std;
volatile led_t zeiger_sek1[LED_ZEIGER_MAX], zeiger_min1[LED_ZEIGER_MAX], zeiger_std1[LED_ZEIGER_MAX]; // Zeiger 1
volatile led_zeiger_t zsek1, zmin1, zstd1;

volatile led_t zeiger_buf[LED_MAX]; // Ausgabebuffer
volatile led_t marker_5, marker_15;

volatile uint8_t led_flags = 0;

//
volatile led_t zeiger_sek2[LED_ZEIGER_MAX], zeiger_min2[LED_ZEIGER_MAX], zeiger_std2[LED_ZEIGER_MAX]; // Zeiger 2
volatile led_zeiger_t  zsek2, zmin2, zstd2;
// kombinierte zeiger
volatile led_t zeiger_akt_sek[LED_ZEIGER_MAX], zeiger_akt_min[LED_ZEIGER_MAX], zeiger_akt_std[LED_ZEIGER_MAX];
//

extern volatile zeit_t g_Zeit;

/*
	Daten in SR schreiben
*/
void led_schreibe_r(uint8_t step)
{
	uint8_t i, j;
	
	j = 0;
	
	for(i = 0; i < 64; i++)
	{
		if(i % 16 == 0)
		{
			cbi(PSR, SR_OUT);
			sbi(PSR, SR_CLK);
			cbi(PSR, SR_CLK);
			continue;
		}
		
		if(zeiger_buf[j].r > step)
			sbi(PSR, SR_OUT);
		else
			cbi(PSR, SR_OUT);
		
		sbi(PSR, SR_CLK);
		cbi(PSR, SR_CLK);
		
		j++;
	}
	
	sbi(PSR, SR_LATCH); // min 150ns H
	//_delay_us(1);
	nop;nop;nop;nop;
	cbi(PSR, SR_LATCH);
}

void led_schreibe_g(uint8_t step)
{
	uint8_t i, j;
	
	j = 0;
	
	for(i = 0; i < 64; i++)
	{
		if(i % 16 == 0)
		{
			cbi(PSR, SR_OUT);
			sbi(PSR, SR_CLK);
			cbi(PSR, SR_CLK);
			continue;
		}
		
		if(zeiger_buf[j].g > step)
			sbi(PSR, SR_OUT);
		else
			cbi(PSR, SR_OUT);
		
		sbi(PSR, SR_CLK);
		cbi(PSR, SR_CLK);
		
		j++;
	}
	
	sbi(PSR, SR_LATCH); // min 150ns H
	//_delay_us(1);
	nop;nop;nop;nop;
	cbi(PSR, SR_LATCH);
}

void led_schreibe_b(uint8_t step)
{
	uint8_t i, j;
	
	j = 0;
	
	for(i = 0; i < 64; i++)
	{
		if(i % 16 == 0)
		{
			cbi(PSR, SR_OUT);
			sbi(PSR, SR_CLK);
			cbi(PSR, SR_CLK);
			continue;
		}
		
		if(zeiger_buf[j].b > step)
			sbi(PSR, SR_OUT);
		else
			cbi(PSR, SR_OUT);
		
		sbi(PSR, SR_CLK);
		cbi(PSR, SR_CLK);
		
		j++;
	}
	
	sbi(PSR, SR_LATCH); // min 150ns H
	//_delay_us(1);
	nop;nop;nop;nop;	
	cbi(PSR, SR_LATCH);
}

/*
	Uhrzeiger
*/

//	Farbe alle Zeiger auf 0 setzen
void led_zeiger_init()
{
	uint8_t i;
	
	marker_5.r = 1;
	marker_5.g = 1;
	marker_5.b = 1;
	
	marker_15.r = 2;
	marker_15.g = 2;
	marker_15.b = 0;
	
	zeiger_sek = zeiger_sek1;
	zeiger_min = zeiger_min1;
	zeiger_std = zeiger_std1;	
	
	for(i = 0; i < LED_ZEIGER_MAX; i++)
	{
		zeiger_sek1[i].r = 0;
		zeiger_sek1[i].g = 0;
		zeiger_sek1[i].b = 0;

		zeiger_min1[i].r = 0;
		zeiger_min1[i].g = 0;
		zeiger_min1[i].b = 0;
		
		zeiger_std1[i].r = 0;
		zeiger_std1[i].g = 0;
		zeiger_std1[i].b = 0;	

		zeiger_std2[i].r = 0;
		zeiger_std2[i].g = 0;
		zeiger_std2[i].b = 0;
		
		zeiger_sek2[i].r = 0;
		zeiger_sek2[i].g = 0;
		zeiger_sek2[i].b = 0;

		zeiger_min2[i].r = 0;
		zeiger_min2[i].g = 0;
		zeiger_min2[i].b = 0;
		
		zeiger_akt_sek[i].r = 0;
		zeiger_akt_sek[i].g = 0;
		zeiger_akt_sek[i].b = 0;
		
		zeiger_akt_min[i].r = 0;
		zeiger_akt_min[i].g = 0;
		zeiger_akt_min[i].b = 0;

		zeiger_akt_std[i].r = 0;
		zeiger_akt_std[i].g = 0;
		zeiger_akt_std[i].b = 0;
	}
	
	for(i = 0; i < LED_MAX; i++)
	{
		zeiger_buf[i].r = 0;
		zeiger_buf[i].g = 0;
		zeiger_buf[i].b = 0;
	}
	
	sbi(led_flags, LED_FLAG_ZEIGER1);
}

//	Zeigertyp 1
void led_zeiger_typ1()
{
	uint8_t i;
	
	for(i = 0; i < LED_ZEIGER_MAX; i++)
	{
		zeiger_sek[i].r = 0;
		zeiger_sek[i].g = 0;
		zeiger_sek[i].b = 0;

		zeiger_min[i].r = 0;
		zeiger_min[i].g = 0;
		zeiger_min[i].b = 0;

		zeiger_std[i].r = 0;
		zeiger_std[i].g = 0;
		zeiger_std[i].b = 0;	
	}

	zsek1.r = 10;
	zsek1.g = 0;
	zsek1.b = 0;
	zsek1.min = 0;
	zsek1.max = 0;
	
	zmin1.r = 0;
	zmin1.g = 10;
	zmin1.b = 0;
	zmin1.min = 1;
	zmin1.max = 1;
	
	zstd1.r = 0;
	zstd1.g = 0;
	zstd1.b = 15;
	zstd1.min = 2;
	zstd1.max = 2;
	
	led_zeiger_create((led_t*)zeiger_sek1, (led_zeiger_t*)&zsek1);
	led_zeiger_create((led_t*)zeiger_min1, (led_zeiger_t*)&zmin1);
	led_zeiger_create((led_t*)zeiger_std1, (led_zeiger_t*)&zstd1);
	
	zsek2.r = 10;
	zsek2.g = 0;
	zsek2.b = 0;
	zsek2.min = 0;
	zsek2.max = 0;
	
	zmin2.r = 0;
	zmin2.g = 10;
	zmin2.b = 0;
	zmin2.min = 1;
	zmin2.max = 1;
	
	zstd2.r = 0;
	zstd2.g = 0;
	zstd2.b = 15;
	zstd2.min = 2;
	zstd2.max = 2;
	
	led_zeiger_create((led_t*)zeiger_sek2, (led_zeiger_t*)&zsek2);
	led_zeiger_create((led_t*)zeiger_min2, (led_zeiger_t*)&zmin2);
	led_zeiger_create((led_t*)zeiger_std2, (led_zeiger_t*)&zstd2);
}

void led_zeiger_create(led_t *z, led_zeiger_t *zdata)
{
	uint8_t i, mi, ma;
	
	if(zdata->min > 4)
		zdata->min = 4;
	if(zdata->max > 4)
		zdata->max = 4;
		
	mi = LED_ZEIGER_OFFSET - zdata->min;
	ma = LED_ZEIGER_OFFSET + zdata->max;
	
	z[LED_ZEIGER_OFFSET].r = zdata->r;
	z[LED_ZEIGER_OFFSET].g = zdata->g;
	z[LED_ZEIGER_OFFSET].b = zdata->b;
	
	for(i = LED_ZEIGER_OFFSET + 1; i < LED_ZEIGER_MAX; i++)
	{
		if(i <= ma)
		{
			z[i].r = zdata->r / (i - LED_ZEIGER_OFFSET + 1);
			z[i].g = zdata->g / (i - LED_ZEIGER_OFFSET + 1);
			z[i].b = zdata->b / (i - LED_ZEIGER_OFFSET + 1);
		}
		else
		{
			z[i].r = 0;
			z[i].g = 0;
			z[i].b = 0;
		}
	}

	for(i = 0; i < LED_ZEIGER_OFFSET; i++)
	{
		if(i >= mi)
		{
			z[i].r = zdata->r / (LED_ZEIGER_OFFSET - i);
			z[i].g = zdata->g / (LED_ZEIGER_OFFSET - i);
			z[i].b = zdata->b / (LED_ZEIGER_OFFSET - i);
		}
		else
		{
			z[i].r = 0;
			z[i].g = 0;
			z[i].b = 0;
		}		
	}
}

void led_zeiger_recreate(uint8_t num)
{
	led_zeiger_create((led_t*)zeiger_sek1, (led_zeiger_t*)&zsek1);
	led_zeiger_create((led_t*)zeiger_min1, (led_zeiger_t*)&zmin1);
	led_zeiger_create((led_t*)zeiger_std1, (led_zeiger_t*)&zstd1);
		
	led_zeiger_create((led_t*)zeiger_sek2, (led_zeiger_t*)&zsek2);
	led_zeiger_create((led_t*)zeiger_min2, (led_zeiger_t*)&zmin2);
	led_zeiger_create((led_t*)zeiger_std2, (led_zeiger_t*)&zstd2);
}

void led_zeiger_set_active(uint8_t num)
{
	cbi(led_flags, LED_FLAG_ZEIGER1);
	cbi(led_flags, LED_FLAG_ZEIGER2);
	cbi(led_flags, LED_FLAG_FADE);
	
	switch(num)
	{
	case LED_ZEIGER2:
		sbi(led_flags, LED_FLAG_ZEIGER2);
		zeiger_sek = zeiger_sek2;
		zeiger_min = zeiger_min2;
		zeiger_std = zeiger_std2;
		break;
	case LED_FADE:
		sbi(led_flags, LED_FLAG_FADE);
		zeiger_sek = zeiger_akt_sek;
		zeiger_min = zeiger_akt_min;
		zeiger_std = zeiger_akt_std;
		break;
	default:
		sbi(led_flags, LED_FLAG_ZEIGER1);
		zeiger_sek = zeiger_sek1;
		zeiger_min = zeiger_min1;
		zeiger_std = zeiger_std1;
		break;
	}
}

uint8_t led_zeiger_get_active()
{
	if(isset(led_flags, LED_FLAG_ZEIGER1))
		return LED_ZEIGER1;
	else if(isset(led_flags, LED_FLAG_ZEIGER2))
		return LED_ZEIGER2;
	else if(isset(led_flags, LED_FLAG_FADE))
		return LED_FADE;
}

#define		ZEIGER_MAX_REF		720

void led_zeiger_update_dyn_zeiger()
{
	int16_t temp, t2;
	int8_t t1;
	uint8_t i;
	
	if(g_Zeit.std < 12)
	{
		temp = (g_Zeit.min  + g_Zeit.std * 60);
		
		for(i = 0; i < LED_ZEIGER_MAX; i++)
		{
			t1 = (int8_t)(zeiger_sek1[i].r - zeiger_sek2[i].r);
			t2 = (t1 * temp) / ZEIGER_MAX_REF;
			zeiger_akt_sek[i].r = (uint8_t)(zeiger_sek2[i].r + (int8_t)t2);

			t1 = (int8_t)(zeiger_sek1[i].g - zeiger_sek2[i].g);
			t2 = (t1 * temp) / ZEIGER_MAX_REF;
			zeiger_akt_sek[i].g = (uint8_t)(zeiger_sek2[i].g + (int8_t)t2);

			t1 = (int8_t)(zeiger_sek1[i].b - zeiger_sek2[i].b);
			t2 = (t1 * temp) / ZEIGER_MAX_REF;
			zeiger_akt_sek[i].b = (uint8_t)(zeiger_sek2[i].b + (int8_t)t2);

			t1 = zeiger_min1[i].r - zeiger_min2[i].r;
			t2 = (t1 * temp) / ZEIGER_MAX_REF;
			zeiger_akt_min[i].r = zeiger_min2[i].r + t2;
			t1 = zeiger_min1[i].g - zeiger_min2[i].g;
			t2 = (t1 * temp) / ZEIGER_MAX_REF;
			zeiger_akt_min[i].g = zeiger_min2[i].g + t2;
			t1 = zeiger_min1[i].b - zeiger_min2[i].b;
			t2 = (t1 * temp) / ZEIGER_MAX_REF;
			zeiger_akt_min[i].b = zeiger_min2[i].b + t2;
			
			t1 = zeiger_std1[i].r - zeiger_std2[i].r;
			t2 = (t1 * temp) / ZEIGER_MAX_REF;
			zeiger_akt_std[i].r = zeiger_std2[i].r + t2;
			t1 = zeiger_std1[i].g - zeiger_std2[i].g;
			t2 = (t1 * temp) / ZEIGER_MAX_REF;
			zeiger_akt_std[i].g = zeiger_std2[i].g + t2;
			t1 = zeiger_std1[i].b - zeiger_std2[i].b;
			t2 = (t1 * temp) / ZEIGER_MAX_REF;
			zeiger_akt_std[i].b = zeiger_std2[i].b + t2;
		}
	}
	else
	{
		temp = (g_Zeit.min  + (g_Zeit.std - 12) * 60);
		
		for(i = 0; i < LED_ZEIGER_MAX; i++)
		{
			t1 = zeiger_sek2[i].r - zeiger_sek1[i].r;
			t2 = (t1 * temp) / ZEIGER_MAX_REF;
			zeiger_akt_sek[i].r = zeiger_sek1[i].r + t2;
			t1 = zeiger_sek2[i].g - zeiger_sek1[i].g;
			t2 = (t1 * temp) / ZEIGER_MAX_REF;
			zeiger_akt_sek[i].g = zeiger_sek1[i].g + t2;
			t1 = zeiger_sek2[i].b - zeiger_sek1[i].b;
			t2 = (t1 * temp) / ZEIGER_MAX_REF;
			zeiger_akt_sek[i].b = zeiger_sek1[i].b + t2;
			
			t1 = zeiger_min2[i].r - zeiger_min1[i].r;
			t2 = (t1 * temp) / ZEIGER_MAX_REF;
			zeiger_akt_min[i].r = zeiger_min1[i].r + t2;
			t1 = zeiger_min2[i].g - zeiger_min1[i].g;
			t2 = (t1 * temp) / ZEIGER_MAX_REF;
			zeiger_akt_min[i].g = zeiger_min1[i].g + t2;
			t1 = zeiger_min2[i].b - zeiger_min1[i].b;
			t2 = (t1 * temp) / ZEIGER_MAX_REF;
			zeiger_akt_min[i].b = zeiger_min1[i].b + t2;
			
			t1 = zeiger_std2[i].r - zeiger_std1[i].r;
			t2 = (t1 * temp) / ZEIGER_MAX_REF;
			zeiger_akt_std[i].r = zeiger_std1[i].r + t2;
			t1 = zeiger_std2[i].g - zeiger_std1[i].g;
			t2 = (t1 * temp) / ZEIGER_MAX_REF;
			zeiger_akt_std[i].g = zeiger_std1[i].g + t2;
			t1 = zeiger_std2[i].b - zeiger_std1[i].b;
			t2 = (t1 * temp) / ZEIGER_MAX_REF;
			zeiger_akt_std[i].b = zeiger_std1[i].b + t2;
		}
	}
}

/*
	Zeiger zusammenlegen in Ausgabebuffer
	st - Stunde
	mi - Minute
	se - Sekunde
*/
void led_zeiger_update(uint8_t st, uint8_t mi, uint8_t se)
{
	uint8_t i, st2, temp, st_s, mi_s, se_s;
	
	if(st >= 12)
		st2 = (st - 12) * 5;
	else
		st2 = st * 5;
	st2 += (mi / 12);
	
	se_s = se - LED_ZEIGER_OFFSET;
	if(se_s > 59)
		se_s = 59 - (0xFF - se_s);
	mi_s = mi - LED_ZEIGER_OFFSET;
	if(mi_s > 59)
		mi_s = 59 - (0xFF - mi_s);
	st_s = st2 - LED_ZEIGER_OFFSET;
	if(st_s > 59)
		st_s = 59 - (0xFF - st_s);
	
	// markierungen
	for(i = 0; i < LED_MAX; i++)
	{
		if(i % 15 == 0)
		{
			zeiger_buf[i].r = marker_15.r;
			zeiger_buf[i].g = marker_15.g;
			zeiger_buf[i].b = marker_15.b;	
		}
		else if(i % 5 == 0)
		{
			zeiger_buf[i].r = marker_5.r;
			zeiger_buf[i].g = marker_5.g;
			zeiger_buf[i].b = marker_5.b;
		}
		else
		{
			zeiger_buf[i].r = 0;
			zeiger_buf[i].g = 0;
			zeiger_buf[i].b = 0;			
		}
	}
	
	// zeiger einfügen
	for(i = 0; i < LED_ZEIGER_MAX; i++)
	{
		// stunde
		zeiger_buf[st_s].r |= zeiger_std[i].r;
		zeiger_buf[st_s].g |= zeiger_std[i].g;
		zeiger_buf[st_s].b |= zeiger_std[i].b;
		
		// minute
		zeiger_buf[mi_s].r |= zeiger_min[i].r;
		zeiger_buf[mi_s].g |= zeiger_min[i].g;
		zeiger_buf[mi_s].b |= zeiger_min[i].b;	
		
		//sekunde
		zeiger_buf[se_s].r |= zeiger_sek[i].r;
		zeiger_buf[se_s].g |= zeiger_sek[i].g;
		zeiger_buf[se_s].b |= zeiger_sek[i].b;
		
		st_s++;
		if(st_s == 60)
			st_s = 0;
		mi_s++;
		if(mi_s == 60)
			mi_s = 0;
		se_s++;
		if(se_s == 60)
			se_s = 0;
	}
}