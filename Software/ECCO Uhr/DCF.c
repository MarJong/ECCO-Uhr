/*
 * DCF.c
 *
 * Created: 17.05.2012 16:42:04
 *  Author: Marcel
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "DCF.h"
#include "ECCO Uhr.h"
#include "LCD.h"

//#define __DCF_DEBUG_2__	// ausgabe von dcf_decode

#define DCF_LEN			60
#define DCF_SIG_SYNC	2	// Sync Signal bei Minute 59
#define DCF_SIG_ERROR	3	// fehler bei auswertung des signals

volatile uint8_t dcf_flags, dcf_buffer[DCF_LEN], dcf_pos;
volatile dcf_result_t dcf_result;

extern volatile uint8_t g_Flags;	// für dcf update flag

void dcf_init()
{
	uint8_t i;
	dcf_flags = 0;
	sbi(dcf_flags, DCF_SYNC_WAIT);
	
	for(i = 0; i < DCF_LEN; i++)
		dcf_buffer[i] = 0;
	dcf_pos = 0;	
	
	// start bei fallender flanke - altes modul
//	sbi(PDCF, DCF_ONOFF);
//	cbi(PDCF, DCF_ONOFF);
}

/*
	DCF Timer 200 Hz - 1 Timerdurchgang = 5ms
	100 ms Absenkung = 0	-> dcf_high = 20
	200 ms Absenkung = 1	-> dcf_high = 40
	synchronisation			-> dcf_high = 0
*/
void dcf_signal(uint8_t sig)
{
	uint8_t sync = 0;
	
#ifdef __DCF_DEBUG_2__
	char temp[4];
	sprintf(temp, "%3i\0", sig);
	lcd_setpos(65, 7);
	lcd_putstr(temp, 0);
#endif
	
	if(sig > 3 && sig < 30) //(sig > 11 && sig < 29)
	{
		dcf_buffer[dcf_pos] = 0;
#ifdef __DCF_DEBUG_2__
		lcd_setpos(100, 7);
		lcd_putc('0', 0);
#endif
	}		
	else if(sig > 30)  //(sig > 31 && sig < 49)
	{
		dcf_buffer[dcf_pos] = 1;
#ifdef __DCF_DEBUG_2__
		lcd_setpos(100, 7);
		lcd_putc('1', 0);
#endif
	}		
	else if (sig < 3)
	{
		dcf_buffer[dcf_pos] = DCF_SIG_SYNC;
		sync = 1;
#ifdef __DCF_DEBUG_2__
		lcd_setpos(100, 7);
		lcd_putc('S', 1);
#endif
	}		
	else
	{
		dcf_buffer[dcf_pos] = DCF_SIG_ERROR;
#ifdef __DCF_DEBUG_2__
		lcd_setpos(100, 7);
		lcd_putc('?', 0);
#endif
	}
#ifdef __DCF_DEBUG_2__
	lcd_redraw();
#endif

	dcf_pos++;
	if(sync)
	{	
		if (dcf_pos == 59)
		{
			if(dcf_decode() != 0)
			{
				sbi(g_Flags, FLAG_DCF_UPDATE);
			}
		}
		dcf_pos = 0;
	}
	if(dcf_pos >= 59)
	{
		dcf_pos = 0;
	}
}

/*
	DCF Protokoll
0						Start einer neuen Minute (ist immer „0“)
1 - 14					Wetterinformationen der Firma MeteoTime sowie Informationen des Katastrophenschutzes
15						Rufbit 
16						„1“: Am Ende dieser Stunde wird MEZ/MESZ umgestellt
17						MEZ
18						MESZ
19						„1“: Am Ende dieser Stunde wird eine Schaltsekunde eingefügt
20						Beginn der Zeitinformation (ist immer „1“)
21 - 24					Minute 1 LSB
25 - 27					Minute 10 LSB
28						Parität Minute
29 - 32					Stunde 1 LSB
33 - 34					Stunde 10 LSB
35						Parität Stunde
36 - 39					Kalendertag	1 LSB
40 - 41					Kalendertag 10 LSB
42 - 44					Wochentag LSB
45 - 48					Monat 1 LSB
49						Monat 10 LSB
50 - 53					Jahr 1 LSB
54 - 57					Jahr 10 LSB
58						Parität Datum
59						Sync

hierbei handelt es sich um eine gerade Parität
Ist die Parität „even“ (gerade) eingestellt, so wird das Paritätsbit gesetzt, falls eine ungerade Anzahl Datenbits im Zeichen gesetzt ist.
*/

uint8_t dcf_decode()
{
	uint8_t parity, i, j;
#ifdef __DCF_DEBUG_2__
	char temp[21];
#endif
	
	dcf_result.flags = 0;
	
#ifdef __DCF_DEBUG_2__
	lcd_clear();
	lcd_setpos(0, 0);
#endif

	if(dcf_buffer[59] != DCF_SIG_SYNC && dcf_buffer[0] != 0 && dcf_buffer[19] != 1)
	{
#ifdef __DCF_DEBUG_2__
		lcd_putstr("Format Fehlerhaft\0", 0);
		lcd_setpos(0, 1);
		for(i = 0; i < DCF_LEN; i++)
		{
			if(dcf_buffer[i] == 0)
				temp[0] = '0';
			else if(dcf_buffer[i] == 1)
				temp[0] = '1';
			else if(dcf_buffer[i] == 2)
				temp[0] = 'S';
			else if(dcf_buffer[i] == 3)
				temp[0] = '?';
			else
				temp[0] = 'x';
			if(i == 0 || i == 19 || i == 59)
				lcd_putc(temp[0], 1);
			else
				lcd_putc(temp[0], 0);
		}
		lcd_redraw();
#endif
		return 0;
	}
	
	// wetter
	for(i = 1, j = 0; i <= 14; i++, j++)
	{
		dcf_result.wetter[j] = dcf_buffer[i];
	}
	
	// zeitumstellung
	if(dcf_buffer[16] == 1)
	{
		sbi(dcf_result.flags, DCF_RES_MEZW);
	}
	
	// minute
	parity = 0;
	dcf_result.minute = 0;
	for(i = 21; i <= 27; i++)
	{
		if(dcf_buffer[i] == DCF_SIG_ERROR ||dcf_buffer[i] == DCF_SIG_SYNC)
		{
			break;
		}
		parity ^= dcf_buffer[i];
	}
	if(parity == dcf_buffer[28] && i == 28) //parity ok
	{
		dcf_result.minute = dcf_buffer[21] + dcf_buffer[22] * 2 + dcf_buffer[23] * 4 + dcf_buffer[24] * 8 + dcf_buffer[25] * 10 + dcf_buffer[26] * 20 + dcf_buffer[27] * 40;
		if(dcf_result.minute < 60)
		{
			sbi(dcf_result.flags, DCF_RES_MINUTE);
		}
#ifdef __DCF_DEBUG_2__
		lcd_setpos(0, 0);
		sprintf(temp, "Minute: %5i\0", dcf_result.minute);
		lcd_putstr(temp, 0);
#endif
	}
#ifdef __DCF_DEBUG_2__
	else  // parity fehler
	{
		lcd_setpos(0, 0);
		lcd_putstr("Minute:\0", 1);
		for(i = 27; i >= 21; i--)
			lcd_putc(dcf_buffer[i] + 48, 1);
	}
#endif

	// stunde
	parity = 0;
	dcf_result.stunde = 0;
	for(i = 29; i <= 34; i++)
	{
		if(dcf_buffer[i] == DCF_SIG_ERROR || dcf_buffer[i] == DCF_SIG_SYNC)
		{
			break;
		}
		parity ^= dcf_buffer[i];
	}
	if(parity == dcf_buffer[35] && i == 35) //parity ok
	{
		dcf_result.stunde = dcf_buffer[29] + dcf_buffer[30] * 2 + dcf_buffer[31] * 4 + dcf_buffer[32] * 8 + dcf_buffer[33] * 10 + dcf_buffer[34] * 20;
		if(dcf_result.stunde < 24)
		{
			sbi(dcf_result.flags, DCF_RES_STUNDE);
		}
#ifdef __DCF_DEBUG_2__
		lcd_setpos(0, 1);
		sprintf(temp, "Stunde: %5i\0", dcf_result.stunde);
		lcd_putstr(temp, 0);
#endif
	}
#ifdef __DCF_DEBUG_2__
	else  // parity fehler
	{
		lcd_setpos(0, 1);
		lcd_putstr("Stunde:\0", 1);
		for(i = 34; i >= 29; i--)
			lcd_putc(dcf_buffer[i] + 48, 1);
	}
#endif

	// datum
	parity = 0;
	dcf_result.ktag = 0;
	dcf_result.wtag = 0;
	dcf_result.jahr = 0;
	dcf_result.monat = 0;
	for(i = 36; i <= 57; i++)
	{
		if(dcf_buffer[i] == DCF_SIG_ERROR || dcf_buffer[i] == DCF_SIG_SYNC)
		{
			break;
		}
		parity ^= dcf_buffer[i];
	}
	if(1 /*parity == dcf_buffer[58] && i == 58*/) //parity ok
	{
		dcf_result.ktag = dcf_buffer[36] + dcf_buffer[37] * 2 + dcf_buffer[38] * 4 + dcf_buffer[39] * 8 + dcf_buffer[40] * 10 + dcf_buffer[41] * 20;
		dcf_result.wtag = dcf_buffer[42] + dcf_buffer[43] * 2 + dcf_buffer[44] * 4;
		dcf_result.monat = dcf_buffer[45] + dcf_buffer[46] * 2 + dcf_buffer[47] * 4 + dcf_buffer[48] * 8 + dcf_buffer[49] * 10;
		dcf_result.jahr = dcf_buffer[50] + dcf_buffer[51] * 2 + dcf_buffer[52] * 4 + dcf_buffer[53] * 8 + dcf_buffer[54] * 10 + dcf_buffer[55] * 20 + dcf_buffer[56] * 40 + dcf_buffer[57] * 80;
		sbi(dcf_result.flags, DCF_RES_DATUM);
#ifdef __DCF_DEBUG_2__
		lcd_setpos(0, 2);
		sprintf(temp, "WTag: %1i\0", dcf_result.wtag);
		lcd_putstr(temp, 0);
		lcd_setpos(0, 3);
		sprintf(temp, "%02i.%02i.%04i\0", dcf_result.ktag, dcf_result.monat, dcf_result.jahr);
		lcd_putstr(temp, 0);
#endif
	}
#ifdef __DCF_DEBUG_2__
	else  // parity fehler
	{
		lcd_setpos(0, 2);
		lcd_putstr("Datum:\0", 2);
		for(i = 57; i >= 36; i--)
			lcd_putc(dcf_buffer[i] + 48, 1);
	}
#endif
#ifdef __DCF_DEBUG_2__
	lcd_redraw();
#endif

	return 1;
}