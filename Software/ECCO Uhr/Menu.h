/*
 * Menu.h
 *
 * Created: 24.05.2012 18:52:37
 *  Author: Marcel
 */ 

#ifndef MENU_H_
#define MENU_H_

#include "LED.h"

#define MENU_ITEM_NAME_LEN		11	// Länge der Einträge max 10 + '\0'

#define MENU_TYPE_NONE				0	// nichts
#define MENU_TYPE_SUBMENU			1	// untermenü öffnen
#define MENU_TYPE_COLOR				2	// farbe lesen
#define MENU_TYPE_TIME				3	// Zeit einlesen
#define MENU_TYPE_ZFORM				4	// Zeigerform
#define MENU_TYPE_TOGGLE_FLAG		5	// funktion aufrufen
#define MENU_TYPE_TOGGLE_DCF_FLAG	6	// DCF flag
#define MENU_TYPE_SONG_PREVIEW		7	// lieder probehören
#define MENU_TYPE_TOGGLE_WECKER		8	// Wecker flag
#define MENU_TYPE_WECKER_SONG		9	// lieder für wecker auswählen
#define MENU_TYPE_TOGGLE_WECKER_REP	10	// wecker wiederholen
#define MENU_TYPE_TOGGLE_ZEIGER		11	// zeiger 1, 2, dyn wechseln

#define MENU_STATE_MENU			0	// im menü
#define MENU_STATE_TIME			1	// zeit eingeben
#define MENU_STATE_COLOR		2	// Farbe eingeben
#define MENU_STATE_ZFORM		3	// Zeigerform eingeben
#define MENU_STATE_SONG_PREVIEW	4	// lieder probehören
#define MENU_STATE_WECKER_SONG	5	// lieder für wecker auswählen

typedef struct
{
	char name[MENU_ITEM_NAME_LEN];	// name
	uint8_t typ;					// typ
	volatile void *data;			// daten hier drin speichern
} menu_entry_t;

typedef struct
{
	volatile uint8_t pos;
	uint8_t num;
	menu_entry_t *item;
} menu_t, *menu_tp;

// init
void menu_init();
void menu_start();

// eingabe
void menu_plus();
void menu_minus();
void menu_save();
void menu_select();

// interner kram zur ausgabe
void menu_draw(menu_tp m);		// menü
void menu_draw2(menu_entry_t *t);

void menu_read_time();			// zeit eingabe
void menu_show_time(zeit_t *t);

void menu_read_color();
void menu_show_color(led_t *c);

void menu_read_hand();
void menu_show_hand(led_zeiger_t *z);

void menu_song_prev();

uint8_t menu_toggle_wecker(uint8_t cmd);
void menu_show_wecker();

uint8_t menu_toggle_wecker_rep(uint8_t cmd);
void menu_show_wecker_rep();

void menu_show_wecksong();

// toggle
uint8_t menu_toggle_force_dcf(uint8_t cmd);
void menu_show_dcf();

void menu_show_tzeiger();
void menu_toggle_zeiger();

#endif /* MENU_H_ */