/*
 * speaker.c
 *
 * Created: 12.06.2012 16:52:26
 *  Author: Marcel
 */ 

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#include "speaker.h"
#include "ECCO Uhr.h"
#include "LCD.h"

/*
	7	- 914Hz	8	- 800	9	- 711
	10	- 640	11	- 581	12	- 533	13	- 492	14	- 457	15	- 426	16	- 400	17	- 376	18	- 355	19	- 336
	20	- 320	21	- 304	22	- 290	23	- 278	24	- 266	25	- 256	26	- 246	27	- 237	28	- 228	29	- 220
	30	- 213	31	- 206	32	- 200	33	- 193	34	- 188	35	- 182	36	- 177	37	- 172	38	- 168	39	- 164
	40	- 160	41	- 156	42	- 152	43	- 148	44	- 145	45	- 142	46	- 139	47	- 136	48	- 133	49	- 130
	50	- 128	51	- 
*/

// noten

enum SFREQ	{
			C0 = 0, CIS0, D0, DIS0, E0, F0, FIS0, G0, GIS0, A0, AIS0, H0,	// C - H
			C1, CIS1, D1, DIS1, E1, F1, FIS1, G1, GIS1, A1, AIS1, H1,		// c - h
			C2, CIS2, D2, DIS2, E2, F2, FIS2, G2, GIS2, A2, AIS2, H2,		// c' - h'
			PAUSE,	// pause
			FREQNUM
			}; //25

	const uint8_t spk_steps[FREQNUM] =	{
											98, 92, 87, 83, 78, 73, 69, 66, 62, 58, 55, 52,
											49, 46, 44, 41, 39, 37, 35, 33, 31, 29, 28, 26,
											24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13,
											0
										};

// länge

#define NOTE_L_2		16	//12800
#define NOTE_L_175		14	//11200
#define NOTE_L_15		12	//9600
#define NOTE_L_125		10	//8000
#define NOTE_L_1		8	//6400
#define NOTE_L_075		6	//4800
#define NOTE_L_05		4	//3200
#define NOTE_L_025		2	//1600
#define NOTE_L_0125		1	// 800
									 
volatile uint8_t spk_flags, spk_songnum, spk_songpos, spk_len_cnt;

note_t song1[] =	{{PAUSE, NOTE_L_05},
					{G1, NOTE_L_025}, {F1, NOTE_L_025}, {E1, NOTE_L_025}, {E1, NOTE_L_025}, {F1, NOTE_L_025}, {PAUSE, NOTE_L_175}, {A0, NOTE_L_025},
					{G1, NOTE_L_025}, {F1, NOTE_L_025}, {E1, NOTE_L_025}, {E1, NOTE_L_025}, {PAUSE, NOTE_L_025}, {F1, NOTE_L_025},{D1, NOTE_L_025},
					{PAUSE, NOTE_L_025}, {E1, NOTE_L_025}, {A0, NOTE_L_05}, {PAUSE, NOTE_L_125}, {E1, NOTE_L_025}, {F1, NOTE_L_025}, {G1, NOTE_L_075},
					{E1, NOTE_L_025}, {C1, NOTE_L_075}, {D1, NOTE_L_025}, {E1, NOTE_L_025}, {PAUSE, NOTE_L_05}, {A0, NOTE_L_025}, {D1, NOTE_L_025}, {E1, NOTE_L_025},
					{F1, NOTE_L_025}, {E1, NOTE_L_025}, {D1, NOTE_L_025}, 
					{0, 0}};

/*
{{G2, NOTE_L_025}, {F2, NOTE_L_025}, {E2, NOTE_L_025}, {E2, NOTE_L_025}, {F2, NOTE_L_025}, {PAUSE, NOTE_L_175}, {A1, NOTE_L_025},
					{G2, NOTE_L_025}, {F2, NOTE_L_025}, {E2, NOTE_L_025}, {E2, NOTE_L_025}, {PAUSE, NOTE_L_025}, {F2, NOTE_L_025},{D2, NOTE_L_025},
					{PAUSE, NOTE_L_025}, {E2, NOTE_L_025}, {A1, NOTE_L_05}, {PAUSE, NOTE_L_125}, {E2, NOTE_L_025}, {F2, NOTE_L_025}, {G2, NOTE_L_075},
					{E2, NOTE_L_025}, {C2, NOTE_L_075}, {D2, NOTE_L_025}, {E2, NOTE_L_025}, {PAUSE, NOTE_L_05}, {A1, NOTE_L_025}, {D2, NOTE_L_025}, {E2, NOTE_L_025},
					{F2, NOTE_L_025}, {E2, NOTE_L_025}, {D2, NOTE_L_025}, {C2, NOTE_L_025}, {A1, NOTE_L_025}, {H1, NOTE_L_025}, {C2, NOTE_L_025}, {F2, NOTE_L_025},
					{E2, NOTE_L_025}, {D2, NOTE_L_025}, {D2, NOTE_L_025}, {C2, NOTE_L_025}, {D2, NOTE_L_025}, {C2, NOTE_L_025}, {C2, NOTE_L_05}, {C2, NOTE_L_075},
					{A1, NOTE_L_025}, {H1, NOTE_L_025}, {C2, NOTE_L_05}, {F2, NOTE_L_05}, {G2, NOTE_L_025}, {F2, NOTE_L_025}, {E2, NOTE_L_025}, {D2, NOTE_L_025},
					{D2, NOTE_L_025}, {E2, NOTE_L_025}, {F2, NOTE_L_05}, {F2, NOTE_L_075}, {G2, NOTE_L_025}, {A2, NOTE_L_025}, {H2, NOTE_L_025}, {H2, NOTE_L_025},
					{A2, NOTE_L_05}, {G2, NOTE_L_05}, {F2, NOTE_L_025}, {G2, NOTE_L_025}, {A2, NOTE_L_025}, {A2, NOTE_L_025}, {G2, NOTE_L_05}, {F2, NOTE_L_05},
					{D2, NOTE_L_025}, {C2, NOTE_L_025}, {D2, NOTE_L_025}, {F2, NOTE_L_025}, {F2, NOTE_L_025}, {E2, NOTE_L_05}, {E2, NOTE_L_025}, {F2, NOTE_L_025},
					{F2, NOTE_L_05},
					{0, 0}};
*/						
						
note_t song2[] = {{E1, NOTE_L_05}, {E1, NOTE_L_05}, {PAUSE, NOTE_L_0125}, {A1, NOTE_L_025}, {A1, NOTE_L_025}, {PAUSE, NOTE_L_0125},
					{D1, NOTE_L_05}, {D1, NOTE_L_05}, {PAUSE, NOTE_L_0125}, {E1, NOTE_L_025}, {E1, NOTE_L_025},
					{PAUSE, NOTE_L_0125}, {E1, NOTE_L_05}, {E1, NOTE_L_05}, {PAUSE, NOTE_L_0125}, {G1, NOTE_L_025}, {G1, NOTE_L_025}, 
					{FIS1, NOTE_L_05}, {E1, NOTE_L_05}, {PAUSE, NOTE_L_0125}, {A1, NOTE_L_025}, {GIS1, NOTE_L_025},
					{D1, NOTE_L_025}, {D1, NOTE_L_05}, {PAUSE, NOTE_L_0125}, {FIS1, NOTE_L_05}, {E1, NOTE_L_05}, {E1, NOTE_L_05}, {E1, NOTE_L_05},
					{PAUSE, NOTE_L_0125}, {A1, NOTE_L_05}, {GIS1, NOTE_L_025}, {H1, NOTE_L_025}, {H1, NOTE_L_025}, {G1, NOTE_L_025}, {G1, NOTE_L_025},
					{FIS1, NOTE_L_025}, {FIS1, NOTE_L_025}, {E1, NOTE_L_075},
					{0, 0}};
						
note_t song3[] =	{{C1, NOTE_L_025}, {E1, NOTE_L_025}, {FIS1, NOTE_L_025}, {A1, NOTE_L_025}, {G1, NOTE_L_05}, {E1, NOTE_L_025},
					{D1, NOTE_L_025}, {A1, NOTE_L_05},
					{FIS1, NOTE_L_025}, {FIS1, NOTE_L_025}, {FIS1, NOTE_L_025}, {G1, NOTE_L_05},
					{FIS1, NOTE_L_025}, {FIS1, NOTE_L_025}, {FIS1, NOTE_L_025}, {G1, NOTE_L_05},
					{A1, NOTE_L_05}, {C1, NOTE_L_025}, {C1, NOTE_L_025}, {C1, NOTE_L_025}, {C1, NOTE_L_025},
					{0, 0}};
						
note_t song4[] =	{{F1, NOTE_L_0125}, {F1, NOTE_L_0125}, {F1, NOTE_L_025}, {F1, NOTE_L_0125}, {PAUSE, NOTE_L_0125}, {F1, NOTE_L_0125}, {F1, NOTE_L_0125}, {F1, NOTE_L_0125}, {F1, NOTE_L_0125}, {F1, NOTE_L_0125},
					{F1, NOTE_L_0125}, {F1, NOTE_L_0125}, {F1, NOTE_L_0125}, {F1, NOTE_L_0125}, {PAUSE, NOTE_L_0125}, {F1, NOTE_L_0125}, {G1, NOTE_L_0125}, {A1, NOTE_L_0125}, {H1, NOTE_L_0125}, {C2, NOTE_L_0125}, {D2, NOTE_L_0125}, {E2, NOTE_L_0125},
					{F2, NOTE_L_1}, {C2, NOTE_L_025}, {F2, NOTE_L_1},
					{H1, NOTE_L_0125}, {C2, NOTE_L_025}, {F1, NOTE_L_125}, {A1, NOTE_L_0125}, {C2, NOTE_L_0125},
					{F2, NOTE_L_025}, {C2, NOTE_L_025}, {G2, NOTE_L_025}, {F2, NOTE_L_075},
					{E2, NOTE_L_05}, {D2, NOTE_L_0125}, {C2, NOTE_L_0125}, {H1, NOTE_L_05}, {C2, NOTE_L_05},
					{F2, NOTE_L_075}, {F2, NOTE_L_025}, {C2, NOTE_L_05}, {F2, NOTE_L_15},
					{A1, NOTE_L_025}, {H1, NOTE_L_025}, {G1, NOTE_L_025}, {C2, NOTE_L_025}, {G1, NOTE_L_125},
					{H1, NOTE_L_05},  {G1, NOTE_L_05},
					{F1, NOTE_L_025}, {E1, NOTE_L_025}, {D1, NOTE_L_025}, {C1, NOTE_L_025},
					{A0, NOTE_L_025}, {H0, NOTE_L_025}, {C1, NOTE_L_025}, {E1, NOTE_L_025},
					{H1, NOTE_L_1},
					{0, 0}};
						
note_t song5[] =	{{G1, NOTE_L_05}, {D1, NOTE_L_025}, {E1, NOTE_L_025}, {F1, NOTE_L_025}, {G1, NOTE_L_025}, {F1, NOTE_L_025}, {E1, NOTE_L_025}, {D1, NOTE_L_025},
					{C1, NOTE_L_05}, {C1, NOTE_L_025}, {E1, NOTE_L_025}, {G1, NOTE_L_05}, {F1, NOTE_L_025}, {E1, NOTE_L_025},
					{D1, NOTE_L_075}, {E1, NOTE_L_025}, {F1, NOTE_L_05}, {G1, NOTE_L_05},
					{D1, NOTE_L_075}, {C1, NOTE_L_075}, {C1, NOTE_L_15},
					{PAUSE, NOTE_L_025}, {F1, NOTE_L_05}, {A1, NOTE_L_025}, {C2, NOTE_L_05}, {H1, NOTE_L_025}, {A1, NOTE_L_025},
					{G1, NOTE_L_075}, {E1, NOTE_L_025}, {G1, NOTE_L_05}, {F1, NOTE_L_025}, {E1, NOTE_L_025},
					{D1, NOTE_L_05}, {D1, NOTE_L_025}, {E1, NOTE_L_025}, {F1, NOTE_L_05}, {G1, NOTE_L_05},
					{E1, NOTE_L_05}, {C1, NOTE_L_05}, {C1, NOTE_L_05}, {PAUSE, NOTE_L_05},
					{0, 0}};
			
note_t song6[] =	{{PAUSE, NOTE_L_05}, {G1, NOTE_L_05}, {A1, NOTE_L_025},
					{C2, NOTE_L_025}, {C2, NOTE_L_025}, {A1, NOTE_L_0125}, {G1, NOTE_L_0125}, {C1, NOTE_L_0125}, {E1, NOTE_L_0125}, {G1, NOTE_L_0125}, {E1, NOTE_L_0125}, {A1, NOTE_L_025}, {G1, NOTE_L_0125}, {E1, NOTE_L_0125}, {D1, NOTE_L_0125}, {E1, NOTE_L_0125},
					{C1, NOTE_L_025}, {D1, NOTE_L_0125}, {DIS1, NOTE_L_0125}, {E1, NOTE_L_0125}, {G1, NOTE_L_0125}, {A1, NOTE_L_0125}, {G1, NOTE_L_0125}, {C2, NOTE_L_025}, {PAUSE, NOTE_L_0125}, {G1, NOTE_L_0125}, {A1, NOTE_L_0125}, {G1, NOTE_L_0125},
					{C2, NOTE_L_025}, {C2, NOTE_L_025}, {A1, NOTE_L_0125}, {G1, NOTE_L_0125}, {C1, NOTE_L_0125}, {E1, NOTE_L_0125}, {G1, NOTE_L_0125}, {E1, NOTE_L_0125}, {A1, NOTE_L_025}, {G1, NOTE_L_0125}, {E1, NOTE_L_0125}, {C1, NOTE_L_0125}, {E1, NOTE_L_0125},
					{G1, NOTE_L_025}, {G1, NOTE_L_0125}, {G1, NOTE_L_0125}, {H1, NOTE_L_0125}, {D2, NOTE_L_0125}, {H1, NOTE_L_025}, {G1, NOTE_L_05}, {PAUSE, NOTE_L_0125}, {G1, NOTE_L_0125}, {A1, NOTE_L_0125}, {G1, NOTE_L_0125},
					{C2, NOTE_L_025}, {C2, NOTE_L_0125}, {C2, NOTE_L_0125}, {C2, NOTE_L_025}, {C2, NOTE_L_0125}, {C2, NOTE_L_0125}, {C2, NOTE_L_025}, {C2, NOTE_L_025}, {A1, NOTE_L_0125}, {G1, NOTE_L_05},
					{F1, NOTE_L_025}, {F1, NOTE_L_025}, {F1, NOTE_L_025}, {F1, NOTE_L_025}, {A1, NOTE_L_0125}, {C2, NOTE_L_0125}, {D2, NOTE_L_0125}, {C2, NOTE_L_0125}, {DIS2, NOTE_L_0125}, {C2, NOTE_L_025}, {D2, NOTE_L_0125},
					{E2, NOTE_L_0125}, {DIS2, NOTE_L_0125}, {E2, NOTE_L_0125}, {D2, NOTE_L_0125}, {E2, NOTE_L_0125}, {G2, NOTE_L_025}, {D2, NOTE_L_0125}, {E2, NOTE_L_0125}, {G2, NOTE_L_0125}, {E2, NOTE_L_025}, {D2, NOTE_L_025}, {G1, NOTE_L_025},
					{A2, NOTE_L_0125}, {E2, NOTE_L_025}, {G1, NOTE_L_0125}, {D2, NOTE_L_025}, {C2, NOTE_L_025}, {C2, NOTE_L_05}, {PAUSE, NOTE_L_05},
					{0, 0}};

song_t songlist[SPK_SONG_NUM] =
{
	{"StillAlive\0", (note_tp)&song1},
	{"Futurama\0", (note_tp)&song2},
	{"Simpsons\0", (note_tp)&song3},
	{"A-Team\0", (note_tp)&song4},	
	{"Tetris A\0", (note_tp)&song5},
	{"Yakety Sax\0", (note_tp)&song6}
};

void speaker_init()
{
	spk_flags = 0;
	spk_songnum = 0;
}

void speaker_start(uint8_t song, uint8_t rep)
{
	if(song >= SPK_SONG_NUM)
		return;
	
	sbi(spk_flags, SPK_FLAG_ON);
	sbi(PSPEAKERON, SPEAKERON);
	spk_songnum = song;
	spk_songpos = 0;
	spk_len_cnt = 0;
	
	if(rep)
		sbi(spk_flags, SPK_FLAG_REPEAT);
	else
		cbi(spk_flags, SPK_FLAG_REPEAT);
}

void speaker_stop()
{
	cbi(spk_flags, SPK_FLAG_ON);
//	cbi(PSPEAKERON, SPEAKERON);
}

uint8_t speaker_isplaying()
{
	return isset(spk_flags, SPK_FLAG_ON);
}

void speaker_toggle(uint8_t song)
{
	if(isset(spk_flags, SPK_FLAG_ON))
	{
		speaker_stop();
	}
	else
	{
		speaker_start(song, 0);
	}
}

// mit 6,4kHz aufgerufen = alle 156µs
void speaker_speak()
{
	static uint16_t counter = 0, spk_pause_cnt = 0;
	
	if(!isset(spk_flags, SPK_FLAG_ON))
		return;
	
	counter++;
	
	if(spk_pause_cnt > 0)
		spk_pause_cnt--;
	
	if(counter % spk_steps[songlist[spk_songnum].noten[spk_songpos].ton] == 0 && songlist[spk_songnum].noten[spk_songpos].ton != PAUSE && spk_pause_cnt == 0)
	{
		if(isset(PSPEAKER, SPEAKER_OUT))
			SPEAKER_L;
		else
			SPEAKER_H;
	}
	
	if(counter % 800 == 0)
	{
		spk_len_cnt++;
	}
	
	if(spk_len_cnt == songlist[spk_songnum].noten[spk_songpos].len)	// nächster ton
	{
		spk_songpos++;
		if(songlist[spk_songnum].noten[spk_songpos].ton == 0 && songlist[spk_songnum].noten[spk_songpos].len == 0)	// ende
		{
			if(isset(spk_flags, SPK_FLAG_REPEAT))
				spk_songpos = 0;
			else
				speaker_stop();
		}
		SPEAKER_L;
		spk_len_cnt = 0;
		spk_pause_cnt = 250;
	}
	
	if(counter == 6400)
	{
		counter = 0;	
	}
}