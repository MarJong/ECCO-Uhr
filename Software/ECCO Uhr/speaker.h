/*
 * speaker.h
 *
 * Created: 12.06.2012 16:38:31
 *  Author: Marcel
 */ 

#ifndef SPEAKER_H_
#define SPEAKER_H_

#include <avr/io.h>
#include <avr/pgmspace.h>

#define SPEAKER_H		sbi(PSPEAKER, SPEAKER_OUT)
#define SPEAKER_L		cbi(PSPEAKER, SPEAKER_OUT)
#define SPK_FLAG_ON		0
#define SPK_FLAG_REPEAT	1

void speaker_init();
void speaker_speak();

void speaker_start(uint8_t song, uint8_t rep);
void speaker_stop();
void speaker_toggle(uint8_t song);

uint8_t speaker_isplaying();

#define SPK_SONG_NUM		6

typedef struct
{
	uint8_t ton, len;
} note_t, *note_tp;

typedef struct 
{
	char name[11];
	note_tp noten;
} song_t;

#endif /* SPEAKER_H_ */