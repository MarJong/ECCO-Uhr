/*
 * ECCO_Uhr.h
 *
 * Created: 17.05.2012 16:45:57
 *  Author: Marcel
 */ 


#ifndef ECCO_UHR_H_
#define ECCO_UHR_H_

#define nop					asm volatile ("nop"::)

// Bit Operationen
#define sbi(port, pin)		((port) |= (1 << (pin)))
#define cbi(port, pin)		((port) &= ~(1 << (pin)))
#define isset(port, pin)	((port) & (1 << (pin)))

// DDRD Richtungen
//#define DDRD_LED_OUT		DDRD = 0x0B //(1 << PD0) | (1 << PD1) | (1 << PD3)
#define DDRD_OUT			DDRD = 0xFF
#define DDRD_IN				DDRD = 0x00

// Flags für Hauptprogramm
#define FLAG_UPDATE_ZEIGER		0	// Zeiger braucht update
#define FLAG_IN_MENU			1	// menü aktiv
#define FLAG_DCF_UPDATE			2	// dcf mit etwas glück erfolgreich
#define FLAG_UPDATE_TIME		3	// Zeit aktualisieren
#define FLAG_FORCE_DCF_UPDATE	4	// dcf update erzwingen
#define FLAG_WECKER_AKTIV		5	// wecker soll zu g_weckzeit spielen
#define FLAG_WECKER_REP			6	// wiederholen
#define FLAG_WECKER_AM_WECKEN	7	// wecker spielt

// Schieberegister ansteuerung
#define PSR			PORTB
#define SR_LATCH	PB4
#define SR_OUT		PB5
#define SR_IN		PB6
#define SR_CLK		PB7
#define SR_ENABLE	PB2

// LED ansteuerung
#define PLED		PORTD
#define LED_RED		PD0
#define LED_BLUE	PD2
#define LED_GREEN	PD1

// DCF ansteuerung
#define PDCF		PORTA
#define PINDCF		PINA
#define DCF_ONOFF	PA0
#define DCF_SIGNAL	PA1

// LCD ansteuerung
#define PLCDDATA	PORTD
#define PINLCDDATA	PIND

#define PLCDCTRL	PORTC
#define LCD_CS1		PC0
#define LCD_CS2		PC1
#define LCD_RS		PC6
#define LCD_RW		PC7

#define PLCDCLK		PORTB
#define LCD_CLK		PB0
#define LCD_RESET	PB1

// Buttons
#define PINBUTTON	PINA
#define BTN_PLUS	PA4
#define BTN_MINUS	PA5
#define BTN_SELECT	PA6
#define BTN_SAVE	PA7

#define PBTNSEL		PORTA
#define BTNSEL		PA3

// speaker
#define PSPEAKER	PORTB
#define SPEAKER_OUT	PB3
#define PSPEAKERON	PORTA
#define SPEAKERON	PA2


typedef struct
{
	uint8_t std, min, sek;
} zeit_t;

typedef struct
{
	uint8_t tag, monat;
	uint16_t jahr;
} datum_t;

#endif /* ECCO_UHR_H_ */