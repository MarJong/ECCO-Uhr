/*
 * ECCO_Uhr.c
 *
 * Created: 17.05.2012 15:56:43
 *  Author: Marcel
 */ 

// [OPT] - optimieren
// mez/mesz umstellung - fertig?
// status icons; wecker, dcf, ...
// 2ter zeiger?
// datum?

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdio.h>
#include <stdlib.h>
#include "ECCO Uhr.h"
#include "LED.h"
#include "LCD.h"
#include "DCF.h"
#include "Button.h"
#include "Menu.h"
#include "config.h"
#include "speaker.h"

volatile zeit_t g_Zeit, g_Weckzeit;
volatile uint8_t g_Flags, wecksong_num, wecker_cnt;
volatile datum_t g_Datum;

extern volatile dcf_result_t dcf_result; // dcf auswertung, flag beachten
extern volatile uint8_t spk_flags;
extern volatile uint8_t led_flags;

/*
	Timer 0 - LED/LCD Steuerung
	PWM: 4bit = 16 Schritte
	100 Hz -> 1.6kHz mit PWM
	6,4 kHz = 4 Durchgänge
	11,2kHz = 7 Durchgänge
	1. Durchgang: LCD
	2. 5. Durchgang: R
	3. 6. Durchgang: G
	4. 7. Durchgang: B

	11.2kHz: CTC; prescaler: 8; max: 0xDF // 7 durchgänge; 1785 befehle/durchgang; 1.7k ~ 85µs
	6.4 kHz: CTC; prescaler: 64; max: 0x31 // 4 Durchgänge; 3125 befehle/durchgang; 3k ~ 150µs
*/
ISR(TIMER0_COMPA_vect)
{
	static uint8_t cycle = 1, pwm_step = 0;

	speaker_speak();

	if(isset(g_Flags, FLAG_UPDATE_ZEIGER) && cycle == 0)
	{
		led_zeiger_update(g_Zeit.std, g_Zeit.min, g_Zeit.sek);
		cbi(g_Flags, FLAG_UPDATE_ZEIGER);
	}		
	
	PORTD = 0xFF;
	
	switch(cycle)
	{
	case 1:		// R
//	case 4:
		led_schreibe_r(pwm_step);
		cbi(PLED, LED_RED);
		break;
	case 2:		// G
//	case 5:
		led_schreibe_g(pwm_step);
		cbi(PLED, LED_GREEN);
		break;
	case 3:		// B
//	case 6:
		led_schreibe_b(pwm_step);
		cbi(PLED, LED_BLUE);
		break;
	case 0:		// LCD
		if(lcd_need_redraw())
		{
			sbi(PSR, SR_ENABLE); //LED aus -> LCD an
			lcd_step();
			cbi(PSR, SR_ENABLE); //LED an  -> LCD aus
		}
		else
		{
			// platz für rechenintesive sachen
			if(isset(led_flags, LED_FLAG_FADE) && pwm_step == 10)		// dynmischen zeiger updaten
			{
				led_zeiger_update_dyn_zeiger();
			}
		}			
		break;
	}
	
	pwm_step++;
	if(pwm_step == 16)
	{
		pwm_step = 0;	
		cycle++;
		if(cycle == 4)
		{
			cycle = 0;
		}			
	}
}

/*
	Timer 1 - Zeit + LCD Licht
	100 Hz: CTC; prescaler: 8; max: 0x61A8
*/
ISR(TIMER1_COMPA_vect)
{
	static uint8_t cycle = 1, lcd_bg_licht = 0;
	
	cycle++;		// [DEBUG]
	if(cycle == 100) // 1 sek weiter
	{
//		g_Zeit.sek++;
		g_Zeit.sek += 30;
		if(g_Zeit.sek >= 60) // 1 min weiter
		{
			g_Zeit.sek = 0;
			sbi(g_Flags, FLAG_UPDATE_TIME);
//			g_Zeit.min++;
			g_Zeit.min += 10;
			if(isset(g_Flags, FLAG_WECKER_AKTIV)) // wecker automatisch aus
			{
				wecker_cnt++;
				if(wecker_cnt == 5)
				{
					cbi(g_Flags, FLAG_WECKER_AM_WECKEN);
					if(!isset(g_Flags, FLAG_WECKER_REP))
					{
						cbi(g_Flags, FLAG_WECKER_AKTIV);
					}
					lcd_bg_licht = 1;
					speaker_stop();
				}
			}
			
			if(g_Zeit.min >= 60) // 1 h weiter
			{
				g_Zeit.min = 0;
				g_Zeit.std++;
				if(g_Zeit.std >= 23)
					g_Zeit.std = 0;
			}
		}
		
		sbi(g_Flags, FLAG_UPDATE_ZEIGER);
		cycle = 0;
		if(lcd_bg_licht > 0)
		{
			lcd_bg_licht--;
			if(lcd_bg_licht == 0 && !isset(g_Flags, FLAG_IN_MENU) && !isset(spk_flags, SPK_FLAG_ON))
				cbi(PSPEAKERON, SPEAKERON);
		}		
	}

	if(isset(g_Flags, FLAG_WECKER_AKTIV))	// wecker an
	{
		if(g_Zeit.std == g_Weckzeit.std && g_Zeit.min == g_Weckzeit.min && g_Zeit.sek == g_Weckzeit.sek && !speaker_isplaying())
		{		
			wecker_cnt = 0;
			sbi(g_Flags, FLAG_WECKER_AM_WECKEN);
			speaker_start(wecksong_num, 1);
		}
	}

	if(!(isset(PINA, PA4) && isset(PINA, PA5) && isset(PINA, PA6) && isset(PINA, PA7))) // taste gedrückt für lcd licht
	{
		lcd_bg_licht = 5;
		sbi(PSPEAKERON, SPEAKERON);
	}
}

/*
	Timer 2 - DCF77 + Buttons
	200 Hz: CTC; prescaler: 1024; max: 0x62; fehler: ca 3.5%!
	DCF - 1 Timerdurchgang = 5ms
	100 ms Absenkung = 0	-> dcf_high = 20
	200 ms Absenkung = 1	-> dcf_high = 40
	synchronisation			-> dcf_high = 0
*/
ISR(TIMER2_COMPA_vect)
{
	static uint8_t cycle = 1, dcf_high = 0, btn_p_cnt = 0, btn_m_cnt = 0;

	if(isset(PINDCF, DCF_SIGNAL)) // dcf zählen + abgleichen
	{
		dcf_high++;
		if((cycle < 60 || cycle > 140) && TCCR2A > 5)	// korrektur
		{
			TCCR2A -= 5;
		}
	}
	
	//Eingabe abfragen
	if(cycle & 1) // buttons
	{
		cbi(PBTNSEL, BTNSEL); // Buttons an PA4-PA7
		button_poll();
		
		if(isset(g_Flags, FLAG_WECKER_AM_WECKEN))	// wecker aktiv -> beenden
		{
			if(button_is_down(BUTTON_SAVE) || button_is_down(BUTTON_SELECT) || button_is_down(BUTTON_PLUS) || button_is_down(BUTTON_MINUS))
			{
				cbi(g_Flags, FLAG_WECKER_AM_WECKEN);
				if(!isset(g_Flags, FLAG_WECKER_REP))
				{
					cbi(g_Flags, FLAG_WECKER_AKTIV);
				}
				speaker_stop();
			}
		}		
		
		// Button down und im Menü
		if(isset(g_Flags, FLAG_IN_MENU))
		
		{
			if(button_is_down(BUTTON_PLUS))
			{
				btn_p_cnt = 0;
				menu_plus();
			}
			else if(button_is_down(BUTTON_MINUS))
			{	btn_m_cnt = 0;
				menu_minus();
			}
			else if(button_is_down(BUTTON_SAVE))
			{
				menu_save();
			}
			else if(button_is_down(BUTTON_SELECT))
			{
				menu_select();
			}
			
			if(button_is_hold(BUTTON_PLUS))
			{
				btn_p_cnt++;
				if(btn_p_cnt == 50)
				{
					btn_p_cnt = 0;
					menu_plus();
				}
			}
			else if(button_is_hold(BUTTON_MINUS))
			{
				btn_m_cnt++;
				if(btn_m_cnt == 50)
				{
					btn_m_cnt = 0;
					menu_minus();
				}
			}
		}
		else	// nicht im menü
		{
			if(button_is_down(BUTTON_SAVE))
			{
				menu_start();
				sbi(g_Flags, FLAG_IN_MENU);
			}
		}		
	}
	else  // tone decoder
	{
		//sbi(PBTNSEL, BTNSEL); // Tone Decoder an PA4-PA7
	}
	
	cycle++;
	if(cycle == 200)
	{
		// nicht im menü -> ...
		if(!isset(g_Flags, FLAG_IN_MENU))
		{
			if(isset(g_Flags, FLAG_UPDATE_TIME))
			{
				char temp[6];
				sprintf(temp, "%02i:%02i\0", g_Zeit.std, g_Zeit.min);
				lcd_setpos(20, 0);
				lcd_putstrbig(temp, 0);
				lcd_redraw();
				cbi(g_Flags, FLAG_UPDATE_TIME);
			}
		}
		
		if(isset(g_Flags, FLAG_DCF_UPDATE)) // dcf übernehmen, flag wird x:y:00 gesetzt und x:y:01 ausgewertet!
		{
			if(isset(dcf_result.flags, DCF_RES_STUNDE) && isset(dcf_result.flags, DCF_RES_MINUTE))
			{
				uint16_t tuhr, tdcf, toff;
				
				tuhr = g_Zeit.std * 100 + g_Zeit.min;
				tdcf = dcf_result.stunde * 100 + dcf_result.minute;
				
				if(isset(dcf_result.flags, DCF_RES_MEZW))
				{
					toff = 200;
				}
				else
				{
					toff = 0;
				}

				if(tdcf - tuhr < (DCF_UPDATE_ZEIT_DIFFERENZ + toff) || isset(g_Flags, FLAG_FORCE_DCF_UPDATE))	// nur übernehmen wenn neue zeit in tolleranzbereich [OPT]
				{
					g_Zeit.std = dcf_result.stunde;
					g_Zeit.min = dcf_result.minute;
					g_Zeit.sek = 1;
					sbi(g_Flags, FLAG_UPDATE_TIME);
					cbi(g_Flags, FLAG_FORCE_DCF_UPDATE);
				}
			}
			
			// Datum übernehmen [OPT]
			if(isset(dcf_result.flags, DCF_RES_DATUM))
			{
				char temp[15];
				g_Datum.tag = dcf_result.ktag;
				g_Datum.monat = dcf_result.monat;
				g_Datum.jahr = 2000 + dcf_result.jahr;
				
				sprintf(temp, "%02i.%02i.%04i\0", g_Datum.tag, g_Datum.monat, g_Datum.jahr);
				lcd_setpos(34, 4);
				lcd_putstr(temp, 0);
				lcd_redraw();
			}
			cbi(g_Flags, FLAG_DCF_UPDATE);
		}
		
		dcf_signal(dcf_high);

		cycle = 0;
		dcf_high = 0;
	}
}


int main(void)
{
	/*
		I/O Richtungen initialisieren
	*/
	/*
		DDRA
		0	O: DCF An/Aus	(frei)		4	I: Freq0/PLUS
		1	I: DCF Signal				5	I: Freq1/MINUS
		2	O: LCD/Speaker an(H)/aus(L)	6	I: Freq2/SELECT
		3	O: Taster (L)/Tone (H) 		7	I: Freq3/SAVE
	*/
	DDRA = (1 << PA0) | (1 << PA2) | (1 << PA3);
	
	/*
		DDRB
		0	O: LCD CLK			4	O: LED LATCH (H; min 100ns)
		1	O: LCD RESET		5	O: LED SOUT		(frei)
		2	O: LED EN (L)		6	I: LED SIN
		3	O: Speaker			7	O: LED CLK (steigende Flanke)
	*/
	DDRB = (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4) | (1 << PB5) | (1 << PB7);

	/*
		DDRC
		0	O: LCD CS1			4	JTAG
		1	O: LCD CS2			5	JTAG
		2	JTAG				6	O: LCD RS
		3	JTAG				7	O: LCD R/W
	*/
	DDRC = (1 << PC0) | (1 << PC1) | (1 << PC6) | (1 << PC7);

	/*
		DDRD
		0	I/O: LED RED (L)		| LCD DATA 0					4 I/O: LCD DATA 4
		1	I/O: LED GREEN (L)		| LCD DATA 1					5 I/O: LCD DATA 5
		2	I/O LED BLUE (L)		| LCD DATA 2					6 I/O: LCD DATA 6
		3	I/O:  LCD DATA 3										7 I/O: LCD DATA 7
		!! Richtungen ändern beim Benutzen !!
	*/
	DDRD_OUT; // default: alles ausgang. bei bedarf eingang setzen und zurücksetzen
	
	/*
		Ports
	*/
	PORTA = 0;
	PORTB = (1 << PB2); // LED enable aus
	PORTC = 0;
	PORTD = (1 << PD0) | (1 << PD1) | (1 << PD2); // led red | green | blue aus
	
	/*
		Timer initialisieren
	*/
	
	// Timer 0
	TCCR0A = (1 << WGM01); // CTC
//	TCCR0B = (1 << CS01); // prescaler = 8 ;
//	OCR0A = 0xDF;
	TCCR0B = (1 << CS01) | (1 << CS00); // prescaler = 64 ;
	OCR0A = 0x31;
	TIMSK0 = (1 << OCIE0A);
	
	// Timer 1
	TCCR1B = (1 << WGM12) | (1 << CS11); // CTC; prescaler = 8
	OCR1A = 0x61A8;
	TIMSK1 = (1 << OCIE1A);
	
	// Timer 2
	TCCR2A = (1 << WGM21); // CTC
	TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20); // prescaler = 1024
	OCR2A = 0x62;
	TIMSK2 = (1 << OCIE2A);
	
	/*
		Variablen
	*/
	g_Flags = 0;
	g_Zeit.std = 0;
	g_Zeit.min = 0;
	g_Zeit.sek = 0;
	
	g_Weckzeit.sek = 0;
	g_Weckzeit.std = 0;
	g_Weckzeit.min = 0;
	
	wecksong_num = 0;
	
	wecker_cnt = 0;
	
	g_Datum.tag = 0;
	g_Datum.monat = 0;
	g_Datum.jahr = 0;
	
	// initialisierungen
	lcd_init();
	lcd_clear();
	lcd_redraw();
	lcd_show_gecco();
	sbi(PSPEAKERON, SPEAKERON);
	
	_delay_ms(500);
	
	led_zeiger_init();
	led_zeiger_typ1();
	led_zeiger_update(g_Zeit.std, g_Zeit.min, g_Zeit.sek);
	sbi(g_Flags, FLAG_UPDATE_TIME);
	
	dcf_init();

	menu_init();
	
	speaker_init();

	if(config_read())
	{
		led_zeiger_recreate(1);
		led_zeiger_update(g_Zeit.std, g_Zeit.min, g_Zeit.sek);
	}
	
	sbi(g_Flags, FLAG_FORCE_DCF_UPDATE); // erstes brauchbare DCF datenpaket benutzen
	
//	_delay_ms(5000); // zeit zum gecco bewundern
	lcd_clear();
	lcd_redraw();
	cbi(PSPEAKERON, SPEAKERON); 

	// Interrupts starten
	sei();
	
    while(1)
    {
        
    }
}