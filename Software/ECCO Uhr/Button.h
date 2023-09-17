/*
 * Button.h
 *
 * Created: 22.05.2012 02:25:49
 *  Author: Marcel
 */ 


#ifndef BUTTON_H_
#define BUTTON_H_

#define BUTTON_PLUS		0
#define BUTTON_MINUS	1
#define BUTTON_SAVE		2
#define BUTTON_SELECT	3

#define BUTTON_MIN_DOWN	10		// ticks die button gehalten werden muss

void button_init();
void button_poll();

uint8_t button_is_down(uint8_t btn);		// fallende flanke
uint8_t button_is_up(uint8_t btn);			// steigende flanke
uint8_t button_is_hold(uint8_t btn);		// gedrückt halten


#endif /* BUTTON_H_ */