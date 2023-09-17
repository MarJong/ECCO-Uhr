/*
 * config.c
 *
 * Created: 10.06.2012 18:25:19
 *  Author: Marcel
 */ 

#include <avr/io.h>
#include <avr/eeprom.h>
#include <string.h>
#include "ECCO Uhr.h"
#include "LED.h"

#define MBYTE1		0xEC
#define MBYTE2		0xC0
#define MBYTE3		0xE0
#define MBYTE4		0x0E

#define CFG_EEPROM_START	0

// MBYTE1 MBYTE2 config daten MBYTE3 MBYTE4

typedef struct {
	uint8_t mb1, mb2;
	
	// zeiger
	led_zeiger_t zstd, zmin, zsek;
	led_zeiger_t zstd2, zmin2, zsek2;
	
	// markierungen
	led_t m5, m15;
	
	//wecker
	zeit_t weckzeit;
	uint8_t weckflag, wecksong;
	
	uint8_t mb3, mb4;
} config_t;

extern volatile led_t marker_5, marker_15;
extern volatile led_zeiger_t zsek1, zmin1, zstd1;
extern volatile led_zeiger_t zsek2, zmin2, zstd2;
extern volatile zeit_t g_Weckzeit; // 5 + 6 wecker; 0 + 1 + 2 zeiger
extern volatile uint8_t g_Flags, wecksong_num;
extern volatile uint8_t led_flags;

config_t cfg;

uint8_t config_read()
{
	eeprom_read_block(&cfg, CFG_EEPROM_START, sizeof(config_t));
	
	if(cfg.mb1 != MBYTE1 || cfg.mb2 != MBYTE2 || cfg.mb3 != MBYTE3 || cfg.mb4 != MBYTE4)
		return 0;
		
	memcpy((void*)&zsek1, (void*)&cfg.zsek, sizeof(led_zeiger_t));
	memcpy((void*)&zmin1, (void*)&cfg.zmin, sizeof(led_zeiger_t));
	memcpy((void*)&zstd1, (void*)&cfg.zstd, sizeof(led_zeiger_t));

	memcpy((void*)&zsek2, (void*)&cfg.zsek2, sizeof(led_zeiger_t));
	memcpy((void*)&zmin2, (void*)&cfg.zmin2, sizeof(led_zeiger_t));
	memcpy((void*)&zstd2, (void*)&cfg.zstd2, sizeof(led_zeiger_t));
		
	memcpy((void*)&marker_5, (void*)&cfg.m5, sizeof(led_t));
	memcpy((void*)&marker_15, (void*)&cfg.m15, sizeof(led_t));
	
	memcpy((void*)&g_Weckzeit, (void*)&cfg.weckzeit, sizeof(zeit_t));
	if(isset(cfg.weckflag, FLAG_WECKER_REP))
	{
		sbi(g_Flags, FLAG_WECKER_REP);
		if(isset(cfg.weckflag, FLAG_WECKER_AKTIV))
			sbi(g_Flags, FLAG_WECKER_AKTIV);
	}
	wecksong_num = cfg.wecksong;
	
	if(isset(cfg.weckflag, LED_FLAG_ZEIGER1))
	{
		led_zeiger_set_active(LED_ZEIGER1);
	}
	else if(isset(cfg.weckflag, LED_FLAG_ZEIGER2))
	{
		led_zeiger_set_active(LED_ZEIGER2);
	}
	else if(isset(cfg.weckflag, LED_FLAG_FADE))
	{
		led_zeiger_set_active(LED_FADE);
	}
	
	return 1;
}

void config_write()
{
	cfg.mb1 = MBYTE1;
	cfg.mb2 = MBYTE2;
	cfg.mb3 = MBYTE3;
	cfg.mb4 = MBYTE4;
	
	memcpy((void*)&cfg.zsek, (void*)&zsek1, sizeof(led_zeiger_t));
	memcpy((void*)&cfg.zmin, (void*)&zmin1, sizeof(led_zeiger_t));
	memcpy((void*)&cfg.zstd, (void*)&zstd1, sizeof(led_zeiger_t));

	memcpy((void*)&cfg.zsek2, (void*)&zsek2, sizeof(led_zeiger_t));
	memcpy((void*)&cfg.zmin2, (void*)&zmin2, sizeof(led_zeiger_t));
	memcpy((void*)&cfg.zstd2, (void*)&zstd2, sizeof(led_zeiger_t));

	memcpy((void*)&cfg.m5, (void*)&marker_5, sizeof(led_t));
	memcpy((void*)&cfg.m15, (void*)&marker_15, sizeof(led_t));
	
	memcpy((void*)&cfg.weckzeit, (void*)&g_Weckzeit, sizeof(zeit_t));
	cfg.weckflag = 0;
	if(isset(g_Flags, FLAG_WECKER_AKTIV))
		sbi(cfg.weckflag, FLAG_WECKER_AKTIV);
	if(isset(g_Flags, FLAG_WECKER_REP))
		sbi(cfg.weckflag, FLAG_WECKER_REP);
	if(isset(led_flags, LED_FLAG_ZEIGER1))
		sbi(cfg.weckflag, LED_FLAG_ZEIGER1);
	if(isset(led_flags, LED_FLAG_ZEIGER2))
		sbi(cfg.weckflag, LED_FLAG_ZEIGER2);
	if(isset(led_flags, LED_FLAG_FADE))
		sbi(cfg.weckflag, LED_FLAG_FADE);
	cfg.wecksong = wecksong_num;
		
	eeprom_write_block(&cfg, CFG_EEPROM_START, sizeof(config_t));
}