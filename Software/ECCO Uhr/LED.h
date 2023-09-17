/*
 * LED.h
 *
 * Created: 17.05.2012 18:03:43
 *  Author: Marcel
 */ 

#ifndef LED_H_
#define LED_H_

#define LED_MAX					60 // 60 LED im Kreis

//        |   
//0 1 2 3 4 5 6 7 8
#define LED_ZEIGER_MAX			9
#define LED_ZEIGER_OFFSET		4 // Zeiger Mitte

#define LED_FLAG_ZEIGER1		0
#define LED_FLAG_ZEIGER2		1
#define LED_FLAG_FADE			2

#define LED_ZEIGER1				LED_FLAG_ZEIGER1
#define LED_ZEIGER2				LED_FLAG_ZEIGER2
#define LED_FADE				LED_FLAG_FADE

typedef struct {
	uint8_t r, g, b;
} led_t;

typedef struct {
	uint8_t r, g, b;
	uint8_t min, max;
} led_zeiger_t;

void led_zeiger_init();
void led_zeiger_typ1();
void led_zeiger_update(uint8_t st, uint8_t mi, uint8_t se);

void led_schreibe_r(uint8_t step);
void led_schreibe_g(uint8_t step);
void led_schreibe_b(uint8_t step);

void led_zeiger_create(led_t *z, led_zeiger_t *zdata);
void led_zeiger_recreate(uint8_t num);
void led_zeiger_set_active(uint8_t num);
uint8_t led_zeiger_get_active();
void led_zeiger_update_dyn_zeiger();

#endif /* LED_H_ */