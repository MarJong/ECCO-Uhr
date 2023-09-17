/*
 * Menu.c
 *
 * Created: 17.05.2012 16:43:48
 *  Author: Marcel
 */ 

#include <avr/io.h>
#include <stdio.h>

#include "ECCO Uhr.h"
#include "LCD.h"
#include "Menu.h"
#include "LED.h"
#include "config.h"
#include "speaker.h"

#define MENU_MAX_ITEMS		8		// 8 Zeilen auf LCD

uint8_t menu_state, need_save;

extern volatile zeit_t g_Zeit, g_Weckzeit;		// zum zeit einstellen
extern volatile uint8_t g_Flags;				// zum menü beenden
extern volatile uint8_t wecksong_num;

// zeiger/ziffernblatt
extern volatile led_zeiger_t zsek1, zmin1, zstd1;
extern volatile led_zeiger_t zsek2, zmin2, zstd2;
extern volatile led_t marker_5, marker_15;

// lieder
extern song_t songlist[SPK_SONG_NUM];

// Zeit einstellen
menu_entry_t menu_e_zeit[2] =	{
									{"Einstellen\0", MENU_TYPE_TIME, (void*)&g_Zeit},
									{"Auto.-DCF\0", MENU_TYPE_TOGGLE_DCF_FLAG, (void*)1}
								};
menu_t menu_zeit = {0, 2, menu_e_zeit};

// Wecker einstellen
menu_entry_t menu_e_wecker[4] =	{
									{"An/Aus\0", MENU_TYPE_TOGGLE_WECKER, (void*)1},
									{"Zeit\0", MENU_TYPE_TIME, &g_Weckzeit},
									{"Wdh.\0", MENU_TYPE_TOGGLE_WECKER_REP, (void*)1},
									{"Sound\0", MENU_TYPE_WECKER_SONG, (void*)1}								
								};
menu_t menu_wecker = {0, 4, menu_e_wecker};

// Zeiger einstellen
menu_entry_t menu_e_zeiger[3] =	{
									{"Stunde\0", MENU_TYPE_ZFORM, (void*)&zstd1},
									{"Minute\0", MENU_TYPE_ZFORM, (void*)&zmin1},
									{"Sekunde\0", MENU_TYPE_ZFORM, (void*)&zsek1}
								};
menu_t menu_zeiger = {0, 3, menu_e_zeiger};
	
menu_entry_t menu_e_zeiger2[3] =	{
									{"Stunde\0", MENU_TYPE_ZFORM, (void*)&zstd2},
									{"Minute\0", MENU_TYPE_ZFORM, (void*)&zmin2},
									{"Sekunde\0", MENU_TYPE_ZFORM, (void*)&zsek2}
								};
menu_t menu_zeiger2 = {0, 3, menu_e_zeiger2};

// Ziffernblatt
menu_entry_t menu_e_ziffblatt[2] =	{
										{"5er\0", MENU_TYPE_COLOR, (void*)&marker_5},
										{"15er\0", MENU_TYPE_COLOR, (void*)&marker_15}
									};
menu_t menu_ziffblatt = {0, 2, menu_e_ziffblatt};

// Hauptmenü
menu_entry_t menu_e_main[7]	=	{
									{"Zeit\0", MENU_TYPE_SUBMENU, (void*)&menu_zeit},
									{"Wecker\0", MENU_TYPE_SUBMENU, (void*)&menu_wecker},
									{"Zeiger\0", MENU_TYPE_TOGGLE_ZEIGER, (void*)1},									
									{"Zeiger 12\0", MENU_TYPE_SUBMENU, (void*)&menu_zeiger},
									{"Zeiger 24\0", MENU_TYPE_SUBMENU, (void*)&menu_zeiger2},
									{"Marker\0", MENU_TYPE_SUBMENU, (void*)&menu_ziffblatt},
									{"Lieder\0", MENU_TYPE_SONG_PREVIEW, (void*)1}
								};

menu_t menu_main =	{0, 7, menu_e_main};

// Tracker
#define MAX_SUB_MENU			5		// Max Anzahl an (Unter)menüs; ES ERFOLGT KEINE ABFRAGE
menu_tp menu_tracker[MAX_SUB_MENU];
volatile uint8_t menu_sub_num;

/*
	temp var für masken
	zeit: var1 = std, var2 = min, var3 = sek; pos 0 = std, 1 = min, 2 = sek
	marker: 1 = r, 2 = g, 3 = b; pos 0 = r, 1 = g, 2 = b
	zform: 1 = r, 2 = g, 3 = b, 4 = min, 5 = max; pos ...
	etc
*/
struct {
	uint8_t var1, var2, var3, var4, var5, var6;
	uint8_t pos;
} temp_input;

void menu_init()
{
	menu_main.pos = 0;
	menu_sub_num = 0;
	menu_tracker[0] = NULL;
}

void menu_start()
{
	uint8_t i;

	need_save = 0;
	menu_main.pos = 0;
	
	menu_sub_num = 1;
	menu_tracker[0] = &menu_main;
	menu_state = MENU_STATE_MENU;
	
	lcd_clear_cs2();
	menu_draw(menu_tracker[menu_sub_num - 1]);
}

// Eingabe auswerten

void menu_plus()
{
	if(menu_state == MENU_STATE_MENU)	// im menü
	{
		if(menu_tracker[menu_sub_num - 1]->pos > 0)
			menu_tracker[menu_sub_num - 1]->pos--;
		
		menu_draw(menu_tracker[menu_sub_num - 1]);
		menu_draw2(&menu_tracker[menu_sub_num - 1]->item[menu_tracker[menu_sub_num - 1]->pos]);
	}
	
	else if(menu_state == MENU_STATE_TIME)	// zeit eingeben
	{
		if(temp_input.pos == 0)	// std
		{
			if(temp_input.var1 == 23)
				temp_input.var1 = 0;
			else
				temp_input.var1++;
		}
		else if(temp_input.pos == 1)	// min
		{
			if(temp_input.var2 == 59)
				temp_input.var2 = 0;
			else
				temp_input.var2++;			
		}
		else if(temp_input.pos == 2)	// sek
		{
			if(temp_input.var3 == 59)
				temp_input.var3 = 0;
			else
				temp_input.var3++;			
		}
		menu_read_time();		
	}
	
	else if(menu_state == MENU_STATE_COLOR)
	{
		led_t *c = (led_t *)menu_tracker[menu_sub_num - 1]->item[menu_tracker[menu_sub_num - 1]->pos].data;
		
		if(temp_input.pos == 0 && c->r < 15)
			c->r++;
		if(temp_input.pos == 1 && c->g < 15)
			c->g++;
		if(temp_input.pos == 2 && c->b < 15)
			c->b++;
		menu_read_color();
	}
	
	else if(menu_state == MENU_STATE_ZFORM)
	{
		led_zeiger_t *z = (led_zeiger_t *)menu_tracker[menu_sub_num - 1]->item[menu_tracker[menu_sub_num - 1]->pos].data;
		
		if(temp_input.pos == 0 && z->r < 15)
			z->r++;
		else if(temp_input.pos == 1 && z->g < 15)
			z->g++;
		else if(temp_input.pos == 2 && z->b < 15)
			z->b++;
		else if(temp_input.pos == 3 && z->max < 4)
			z->max++;
		else if(temp_input.pos == 4 && z->min < 4)
			z->min++;
		led_zeiger_recreate(1);
		menu_read_hand();
	}
	
	else if(menu_state == MENU_STATE_SONG_PREVIEW)
	{
		if(temp_input.pos > 0)
			temp_input.pos--;
		menu_song_prev();
	}
	
	else if(menu_state == MENU_STATE_WECKER_SONG)
	{
		if(temp_input.pos > 0)
			temp_input.pos--;
		speaker_start(temp_input.pos, 0);
		menu_song_prev();
	}
}

void menu_minus()
{
	if(menu_state == MENU_STATE_MENU)	// im menü
	{
		if(menu_tracker[menu_sub_num - 1]->pos < (menu_tracker[menu_sub_num - 1]->num - 1))
			menu_tracker[menu_sub_num - 1]->pos++;
		
		menu_draw(menu_tracker[menu_sub_num - 1]);
		menu_draw2(&menu_tracker[menu_sub_num - 1]->item[menu_tracker[menu_sub_num - 1]->pos]);
	}
	
	else if(menu_state == MENU_STATE_TIME)	// zeit eingeben
	{
		if(temp_input.pos == 0)	// std
		{
			if(temp_input.var1 == 0)
				temp_input.var1 = 23;
			else
				temp_input.var1--;
		}
		else if(temp_input.pos == 1)	// min
		{
			if(temp_input.var2 == 0)
				temp_input.var2 = 59;
			else
				temp_input.var2--;
		}
		else if(temp_input.pos == 2)	// sek
		{
			if(temp_input.var3 == 0)
				temp_input.var3 = 59;
			else
				temp_input.var3--;
		}
		menu_read_time();
	}
	
	else if(menu_state == MENU_STATE_COLOR)
	{
		led_t *c = (led_t *)menu_tracker[menu_sub_num - 1]->item[menu_tracker[menu_sub_num - 1]->pos].data;
		
		if(temp_input.pos == 0 && c->r > 0)
			c->r--;
		else if(temp_input.pos == 1 && c->g > 0)
			c->g--;
		else if(temp_input.pos == 2 && c->b > 0)
			c->b--;
		menu_read_color();
	}
	
	else if(menu_state == MENU_STATE_ZFORM)
	{
		led_zeiger_t *z = (led_zeiger_t *)menu_tracker[menu_sub_num - 1]->item[menu_tracker[menu_sub_num - 1]->pos].data;
		
		if(temp_input.pos == 0 && z->r > 0)
			z->r--;
		else if(temp_input.pos == 1 && z->g > 0)
			z->g--;
		else if(temp_input.pos == 2 && z->b > 0)
			z->b--;
		else if(temp_input.pos == 3 && z->max > 0)
			z->max--;
		else if(temp_input.pos == 4 && z->min > 0)
			z->min--;
		led_zeiger_recreate(1);
		menu_read_hand();
	}
	
	else if(menu_state == MENU_STATE_SONG_PREVIEW)
	{
		if(temp_input.pos < SPK_SONG_NUM - 1)
			temp_input.pos++;
		menu_song_prev();
	}
	
	else if(menu_state == MENU_STATE_WECKER_SONG)
	{
		if(temp_input.pos < SPK_SONG_NUM - 1)
			temp_input.pos++;
		speaker_start(temp_input.pos, 0);
		menu_song_prev();
	}
}

void menu_save()
{
	menu_tp m = menu_tracker[menu_sub_num - 1];
	
	if(!m->item[m->pos].data)
		return;
	
	if(menu_state == MENU_STATE_MENU)
	{
		if(m->item[m->pos].typ == MENU_TYPE_SUBMENU)	// untermenü öffnen
		{
			if(m->item[m->pos].data == NULL)
				return;

			menu_tracker[menu_sub_num] = (menu_tp)m->item[m->pos].data;
			menu_tracker[menu_sub_num]->pos = 0;
			menu_sub_num++;
			menu_draw(menu_tracker[menu_sub_num - 1]);
			menu_draw2(&menu_tracker[menu_sub_num - 1]->item[menu_tracker[menu_sub_num - 1]->pos]);
		}
		
		else if(m->item[m->pos].typ == MENU_TYPE_TIME)	// zeit eingeben
		{
			zeit_t *t = (zeit_t*)m->item[m->pos].data;
			temp_input.pos = 0;
			temp_input.var1 = t->std;
			temp_input.var2 = t->min;
			temp_input.var3 = t->sek;
			
			menu_state = MENU_STATE_TIME;
			menu_read_time();
		}
		
		else if(m->item[m->pos].typ == MENU_TYPE_COLOR)	// farbe eingeben
		{
			led_t *c = (led_t*)m->item[m->pos].data;			
			temp_input.pos = 0;
			temp_input.var1 = c->r;
			temp_input.var2 = c->g;
			temp_input.var3 = c->b;
			
			menu_state = MENU_STATE_COLOR;
			menu_read_color();
		}
		
		else if(m->item[m->pos].typ == MENU_TYPE_ZFORM)	// Zeigerform eingeben
		{
			led_zeiger_t *z = (led_zeiger_t*)m->item[m->pos].data;
			temp_input.pos = 0;
			temp_input.var1 = z->r;
			temp_input.var2 = z->g;
			temp_input.var3 = z->b;
			temp_input.var4 = z->min;
			temp_input.var5 = z->max;
			temp_input.var6 = led_zeiger_get_active();
			
			if(z == &zsek1 || z == &zmin1 || z == &zstd1)
			{
				led_zeiger_set_active(LED_ZEIGER1);
			}
			else
			{
				led_zeiger_set_active(LED_ZEIGER2);
			}
			
			menu_state = MENU_STATE_ZFORM;
			menu_read_hand();
		}
		
		else if(m->item[m->pos].typ == MENU_TYPE_TOGGLE_DCF_FLAG)	// DCF abfrage erzwingen
		{
			menu_toggle_force_dcf(1);
			lcd_clear_cs2();
			menu_show_dcf();
			lcd_redraw_cs2();
		}
		
		else if(m->item[m->pos].typ == MENU_TYPE_SONG_PREVIEW)	// DCF abfrage erzwingen
		{
			temp_input.pos = 0;
			menu_state = MENU_STATE_SONG_PREVIEW;
			menu_song_prev();
		}
		
		else if(m->item[m->pos].typ == MENU_TYPE_TOGGLE_WECKER)	// Wecker umschalten
		{
			menu_toggle_wecker(1);
			lcd_clear_cs2();
			menu_show_wecker();
			lcd_redraw_cs2();
		}

		else if(m->item[m->pos].typ == MENU_TYPE_TOGGLE_WECKER_REP)	// Wecker wiederholen umschalten
		{
			menu_toggle_wecker_rep(1);
			lcd_clear_cs2();
			menu_show_wecker_rep();
			lcd_redraw_cs2();
		}
		
		else if(m->item[m->pos].typ == MENU_TYPE_WECKER_SONG)	// Wecker umschalten
		{
			temp_input.pos = 0;
			menu_state = MENU_STATE_WECKER_SONG;
			menu_song_prev();
		}
		
		else if(m->item[m->pos].typ == MENU_TYPE_TOGGLE_ZEIGER)
		{
			menu_toggle_zeiger();
			need_save = 1;
		}
	}
	
	// Aktionen für Eingabemasken
	
	else if(menu_state == MENU_STATE_TIME)		// zeit eingeben
	{
		if(temp_input.pos == 2)		// eingabe fertig -> übernehmen
		{
			zeit_t *z = (zeit_t*)m->item[m->pos].data;
			z->std = temp_input.var1;
			z->min = temp_input.var2;
			z->sek = temp_input.var3;
			
			need_save = 1;
			menu_state = MENU_STATE_MENU;
			lcd_clear_cs2();
			menu_draw(menu_tracker[menu_sub_num - 1]);
		}
		else
		{
			temp_input.pos++;
			menu_read_time();
		}
	}
	
	else if(menu_state == MENU_STATE_COLOR)	// farbe eingeben
	{
		if(temp_input.pos == 2) // übernehmen
		{
			need_save = 1;
			menu_state = MENU_STATE_MENU;
			lcd_clear_cs2();
			menu_draw(menu_tracker[menu_sub_num - 1]);
		}
		else
		{
			temp_input.pos++;
			menu_read_color();
		}
	}
	
	else if(menu_state == MENU_STATE_ZFORM)	// farbe eingeben
	{
		if(temp_input.pos == 4) // übernehmen
		{
			led_zeiger_set_active(temp_input.var6);
			led_zeiger_recreate(1);
			need_save = 1;
			menu_state = MENU_STATE_MENU;
			lcd_clear_cs2();
			menu_draw(menu_tracker[menu_sub_num - 1]);
		}
		else
		{
			temp_input.pos++;
			menu_read_hand();
		}
	}
	
	else if(menu_state == MENU_STATE_SONG_PREVIEW)
	{
		speaker_start(temp_input.pos, 0);
	}
	
	else if(menu_state == MENU_STATE_WECKER_SONG)
	{
		wecksong_num = temp_input.pos;
		speaker_stop();
		menu_state = MENU_STATE_MENU;
		lcd_clear_cs2();
		lcd_redraw_cs2();
	}
}

void menu_select()
{
	if(menu_state == MENU_STATE_MENU)
	{
		menu_sub_num--;
		if(menu_sub_num == 0) // menü beendet
		{
			cbi(g_Flags, FLAG_IN_MENU);
			lcd_clear();
			lcd_redraw();
			sbi(g_Flags, FLAG_UPDATE_TIME);
			if(need_save)
			{
				config_write();
			}
			return;
		}
		menu_draw(menu_tracker[menu_sub_num - 1]);
	}
	
	else if(menu_state == MENU_STATE_TIME)
	{
		if(temp_input.pos == 0)	// abbrechen
		{
			menu_state = MENU_STATE_MENU;
			lcd_clear_cs2();
			lcd_redraw_cs2();
			menu_draw(menu_tracker[menu_sub_num - 1]);
		}
		else		// schritt zurück
		{
			temp_input.pos--;
			menu_read_time();
		}
	}
	
	else if(menu_state == MENU_STATE_COLOR)
	{
		if(temp_input.pos == 0)	//abbrechen; wiederherstellen
		{
			led_t *c = (led_t *)menu_tracker[menu_sub_num - 1]->item[menu_tracker[menu_sub_num - 1]->pos].data;
			c->r = temp_input.var1;
			c->g = temp_input.var2;
			c->b = temp_input.var3;
			
			menu_state = MENU_STATE_MENU;
			lcd_clear_cs2();
			lcd_redraw_cs2();
			menu_draw(menu_tracker[menu_sub_num - 1]);			
		}
		else
		{
			temp_input.pos--;
			menu_read_color();
		}
	}
	
	else if(menu_state == MENU_STATE_ZFORM)
	{
		if(temp_input.pos == 0)	//abbrechen; wiederherstellen
		{
			led_zeiger_t *z = (led_zeiger_t *)menu_tracker[menu_sub_num - 1]->item[menu_tracker[menu_sub_num - 1]->pos].data;
			z->r = temp_input.var1;
			z->g = temp_input.var2;
			z->b = temp_input.var3;
			z->min = temp_input.var4;
			z->max = temp_input.var5;
			
			led_zeiger_set_active(temp_input.var6);
			led_zeiger_recreate(1);
			
			menu_state = MENU_STATE_MENU;
			lcd_clear_cs2();
			lcd_redraw_cs2();
			menu_draw(menu_tracker[menu_sub_num - 1]);			
		}
		else
		{
			temp_input.pos--;
			menu_read_hand();
		}
	}
	
	else if(menu_state == MENU_STATE_SONG_PREVIEW)
	{
		if(speaker_isplaying())
		{
			speaker_stop();
		}
		else
		{
			menu_state = MENU_STATE_MENU;
			lcd_clear_cs2();
			lcd_redraw_cs2();
		}
	}
	
	else if(menu_state == MENU_STATE_WECKER_SONG)
	{
		speaker_stop();
		menu_state = MENU_STATE_MENU;
		lcd_clear_cs2();
		lcd_redraw_cs2();
	}
}

// Ausgabefunktionen
// linken teil ausgeben
void menu_draw(menu_tp m)
{
	uint8_t i, start, ende, zeile;
	
	start = MENU_MAX_ITEMS * (m->pos / MENU_MAX_ITEMS);
	if(start + MENU_MAX_ITEMS < m->num)
	{
		ende = start + MENU_MAX_ITEMS;
	}
	else
	{
		ende = m->num;
	}
	
	lcd_clear_cs1();
	
	for(i = start, zeile = 0; i < ende; i++, zeile++)
	{
		lcd_setpos(0, zeile);
		if(i == m->pos)
		{
			lcd_putstr(m->item[i].name, 1);
		}
		else
		{
			lcd_putstr(m->item[i].name, 0);
		}
	}

	lcd_redraw();
}	

// rechten teil ausgeben
void menu_draw2(menu_entry_t *t)
{
	lcd_clear_cs2();
	
	if(!t)
		return;
	if(!t->data)
		return;
	
	if(t->typ == MENU_TYPE_TIME)
	{
		menu_show_time((zeit_t*)t->data);
	}
	else if(t->typ == MENU_TYPE_COLOR)
	{
		menu_show_color((led_t*)t->data);
	}
	else if(t->typ == MENU_TYPE_ZFORM)
	{
		menu_show_hand((led_zeiger_t*)t->data);
	}
	else if(t->typ == MENU_TYPE_TOGGLE_DCF_FLAG)
	{
		menu_show_dcf();
	}
	else if(t->typ == MENU_TYPE_TOGGLE_WECKER)
	{
		menu_show_wecker();
	}
	else if(t->typ == MENU_TYPE_TOGGLE_WECKER_REP)
	{
		menu_show_wecker_rep();
	}
	else if(t->typ == MENU_TYPE_WECKER_SONG)
	{
		menu_show_wecksong();
	}
	else if(t->typ == MENU_TYPE_TOGGLE_ZEIGER)
	{
		menu_show_tzeiger();
	}
	
	lcd_redraw_cs2();
}

// Zeit

void menu_read_time()
{
	char temp[3];
//	lcd_clear_cs2();
	
	lcd_setpos(66, 3);
	// stunde
	sprintf(temp, "%02i\0", temp_input.var1);
	if(temp_input.pos == 0)
	{
		lcd_putstr(temp, 1);
	}
	else
	{
		lcd_putstr(temp, 0);
	}
	lcd_putc(':', 0);
	
	// minute
	sprintf(temp, "%02i\0", temp_input.var2);
	if(temp_input.pos == 1)
	{
		lcd_putstr(temp, 1);
	}
	else
	{
		lcd_putstr(temp, 0);
	}
	lcd_putc(':', 0);
	
	// sekunde
	sprintf(temp, "%02i\0", temp_input.var3);
	if(temp_input.pos == 2)
	{
		lcd_putstr(temp, 1);
	}
	else
	{
		lcd_putstr(temp, 0);
	}
	
	lcd_redraw_cs2();
}

void menu_show_time(zeit_t *t)
{
	char temp[9];
	
	sprintf(temp, "%02i:%02i:%02i\0", t->std, t->min, t->sek);
	lcd_setpos(66, 3);
	lcd_putstr(temp, 0);
}

// Farbe

void menu_read_color()
{
	char temp[11], inv;
	
	led_t *c = (led_t *)menu_tracker[menu_sub_num - 1]->item[menu_tracker[menu_sub_num - 1]->pos].data;
	
//	lcd_clear_cs2();
	
	if(temp_input.pos == 0)
		inv = 1;
	else
		inv = 0;
	
	lcd_setpos(64, 1);
	sprintf(temp, "Rot: %02i\0", c->r);
	lcd_putstr(temp, inv);

	if(temp_input.pos == 1)
		inv = 1;
	else
		inv = 0;

	lcd_setpos(64, 2);
	sprintf(temp, "Gr%cn: %02i\0", 129, c->g);
	lcd_putstr(temp, inv);

	if(temp_input.pos == 2)
		inv = 1;
	else
		inv = 0;
	
	lcd_setpos(64, 3);
	sprintf(temp, "Blau: %02i\0", c->b);
	lcd_putstr(temp, inv);
	
	lcd_redraw_cs2();	
}

void menu_show_color(led_t *c)
{
	char temp[11];
	
	lcd_setpos(64, 1);
	sprintf(temp, "Rot: %02i\0", c->r);
	lcd_putstr(temp, 0);
	
	lcd_setpos(64, 2);
	sprintf(temp, "Gr%cn: %02i\0", 129, c->g);
	lcd_putstr(temp, 0);
	
	lcd_setpos(64, 3);
	sprintf(temp, "Blau: %02i\0", c->b);
	lcd_putstr(temp, 0);
}

// zeiger

void menu_show_hand(led_zeiger_t *z)
{
	char temp[11];
	
	lcd_setpos(64, 1);
	sprintf(temp, "Rot: %02i\0", z->r);
	lcd_putstr(temp, 0);
	
	lcd_setpos(64, 2);
	sprintf(temp, "Gr%cn: %02i\0", 129, z->g);
	lcd_putstr(temp, 0);
	
	lcd_setpos(64, 3);
	sprintf(temp, "Blau: %02i\0", z->b);
	lcd_putstr(temp, 0);
	
	lcd_setpos(64, 4);
	sprintf(temp, "Vor: %02i\0", z->max);
	lcd_putstr(temp, 0);
	
	lcd_setpos(64, 5);
	sprintf(temp, "Nach: %02i\0", z->min);
	lcd_putstr(temp, 0);
}

void menu_read_hand()
{
	char temp[11], inv;
	
	led_zeiger_t *z = (led_zeiger_t *)menu_tracker[menu_sub_num - 1]->item[menu_tracker[menu_sub_num - 1]->pos].data;
	
	if(temp_input.pos == 0)
		inv = 1;
	else
		inv = 0;
	
	lcd_setpos(64, 1);
	sprintf(temp, "Rot: %02i\0", z->r);
	lcd_putstr(temp, inv);

	if(temp_input.pos == 1)
		inv = 1;
	else
		inv = 0;

	lcd_setpos(64, 2);
	sprintf(temp, "Gr%cn: %02i\0", 129, z->g);
	lcd_putstr(temp, inv);

	if(temp_input.pos == 2)
		inv = 1;
	else
		inv = 0;
	
	lcd_setpos(64, 3);
	sprintf(temp, "Blau: %02i\0", z->b);
	lcd_putstr(temp, inv);
	
	if(temp_input.pos == 3)
		inv = 1;
	else
		inv = 0;
	
	lcd_setpos(64, 4);
	sprintf(temp, "Vor: %02i\0", z->max);
	lcd_putstr(temp, inv);
	
	if(temp_input.pos == 4)
		inv = 1;
	else
		inv = 0;
	
	lcd_setpos(64, 5);
	sprintf(temp, "Nach: %02i\0", z->min);
	lcd_putstr(temp, inv);
	
	lcd_redraw_cs2();	
}

// dcf toggle
uint8_t menu_toggle_force_dcf(uint8_t cmd)
{	
	if(cmd == 0)	// flag abfragen
	{
		return isset(g_Flags, FLAG_FORCE_DCF_UPDATE);
	}
	else if(cmd == 1)	// flag setzen
	{
		if(isset(g_Flags, FLAG_FORCE_DCF_UPDATE))
		{
			cbi(g_Flags, FLAG_FORCE_DCF_UPDATE);
			return 0;
		}
		else
		{
			sbi(g_Flags, FLAG_FORCE_DCF_UPDATE);
			return 1;
		}
	}
}

void menu_show_dcf()
{
	uint8_t res;
	res = menu_toggle_force_dcf(0);
	
	lcd_setpos(65, 1);
	lcd_putstr("Abgleich\0", 0);
	lcd_setpos(65, 2);
	lcd_putstr("per DCF\0", 0);
	lcd_setpos(65, 3);
	if(res)
		lcd_putstr("aktiv\0", 0);
	else
		lcd_putstr("inaktiv\0", 0);
}

// lieder anhören
void menu_song_prev()
{
	uint8_t i, start, ende, zeile;
	
	start = MENU_MAX_ITEMS * (temp_input.pos / MENU_MAX_ITEMS);
	if(start + MENU_MAX_ITEMS < SPK_SONG_NUM)
	{
		ende = start + MENU_MAX_ITEMS;
	}
	else
	{
		ende = SPK_SONG_NUM;
	}
	
	lcd_clear_cs2();
	
	for(i = start, zeile = 0; i < ende; i++, zeile++)
	{
		lcd_setpos(64, zeile);
		if(i == temp_input.pos)
		{
			lcd_putstr(songlist[i].name, 1);
		}
		else
		{
			lcd_putstr(songlist[i].name, 0);
		}
	}

	lcd_redraw_cs2();
}

// wecker toggle
uint8_t menu_toggle_wecker(uint8_t cmd)
{	
	if(cmd == 0)	// flag abfragen
	{
		return isset(g_Flags, FLAG_WECKER_AKTIV);
	}
	else if(cmd == 1)	// flag setzen
	{
		need_save = 1;
		if(isset(g_Flags, FLAG_WECKER_AKTIV))
		{
			cbi(g_Flags, FLAG_WECKER_AKTIV);
			return 0;
		}
		else
		{
			sbi(g_Flags, FLAG_WECKER_AKTIV);
			return 1;
		}
	}
}

void menu_show_wecker()
{
	uint8_t res;
	res = menu_toggle_wecker(0);
	
	lcd_setpos(65, 2);
	lcd_putstr("Wecker\0", 0);
	lcd_setpos(65, 3);
	if(res)
		lcd_putstr("aktiv\0", 0);
	else
		lcd_putstr("inaktiv\0", 0);
}

// wecker repeat toggle
uint8_t menu_toggle_wecker_rep(uint8_t cmd)
{	
	if(cmd == 0)	// flag abfragen
	{
		return isset(g_Flags, FLAG_WECKER_REP);
	}
	else if(cmd == 1)	// flag setzen
	{
		need_save = 1;
		if(isset(g_Flags, FLAG_WECKER_REP))
		{
			cbi(g_Flags, FLAG_WECKER_REP);
			return 0;
		}
		else
		{
			sbi(g_Flags, FLAG_WECKER_REP);
			return 1;
		}
	}
}

void menu_show_wecker_rep()
{
	uint8_t res;
	res = menu_toggle_wecker_rep(0);
	
	lcd_setpos(65, 2);
	lcd_putstr("Wieder-\0", 0);
	lcd_setpos(65, 3);
	lcd_putstr("holen\0", 0);
	lcd_setpos(65, 4);
	if(res)
		lcd_putstr("aktiv\0", 0);
	else
		lcd_putstr("inaktiv\0", 0);
}

void menu_show_wecksong()
{
	lcd_setpos(65, 2);
	lcd_putstr(songlist[wecksong_num].name, 0);
}

void menu_show_tzeiger()
{
	uint8_t n;
	
	n = led_zeiger_get_active();
	
	lcd_setpos(65, 1);
	lcd_putstr("Aktiver\0", 0);
	lcd_setpos(65, 2);
	lcd_putstr("Zeiger:\0", 0);
	lcd_setpos(65, 3);
	
	switch(n)
	{
	case LED_ZEIGER1:
		lcd_putstr("Zeiger1\0", 0);
		break;
	case LED_ZEIGER2:
		lcd_putstr("Zeiger2\0", 0);
		break;
	case LED_FADE:
		lcd_putstr("Fade\0", 0);
		break;
	}	
}

void menu_toggle_zeiger()
{
	uint8_t n;
	
	n = led_zeiger_get_active();

	switch(n)
	{
	case LED_ZEIGER1:
		led_zeiger_set_active(LED_ZEIGER2);
		break;
	case LED_ZEIGER2:
		led_zeiger_set_active(LED_FADE);
		break;
	case LED_FADE:
		led_zeiger_set_active(LED_ZEIGER1);
		break;
	}		
	
	lcd_clear_cs2();
	menu_show_tzeiger();
	lcd_redraw_cs2();
}