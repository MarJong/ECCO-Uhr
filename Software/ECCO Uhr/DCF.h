/*
 * dcf.h
 *
 * Created: 19.05.2012 20:50:12
 *  Author: Marcel
 */ 


#ifndef DCF_H_
#define DCF_H_

void dcf_init();
void dcf_signal(uint8_t sig);
uint8_t dcf_decode(); // return 0 = fehlschlag, 1 = ok, 2 = mindestens 1 fehler

// DCF Flags
#define DCF_SYNC_WAIT		1	// für init. auf sync warten
#define DCF_SYNC			2	// sync erhalten

#define DCF_MAX				60

#define DCF_RES_MEZ			0	// 1 = MEZ, 0 = MESZ
#define DCF_RES_MEZW		1	// 1 = MEZ/MESZ wechsel
#define DCF_RES_SS			2	// 1 = schaltsekunde
#define DCF_RES_MINUTE		3	// 1 = Zeit ok, 0 = fehlerhaft
#define DCF_RES_STUNDE		4	// wie oben
#define DCF_RES_DATUM		5	// wie oben

#define DCF_UPDATE_ZEIT_DIFFERENZ	5	// wenn dcf resultat innerhalb x minuten von µc uhrzeit dann updaten. vehindert extreme umstellung

typedef struct
{
	uint8_t wetter[14];
	uint8_t minute, stunde;
	uint8_t ktag, wtag, monat;
	uint16_t jahr; // KalenderTAG, WochenTAG
	
	uint8_t flags; // DCF_RES_x
} dcf_result_t;

#endif /* DCF_H_ */