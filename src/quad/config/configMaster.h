#ifndef __CONFIGMASTER_H
#define __CONFIGMASTER_H

#include "led.h"
#include "sound_beeper.h"
#include "serial.h"
#include "gyro.h"
#include "acceleration.h"

typedef struct master_s {
	uint8_t version;
	uint16_t size;
	uint8_t magic_be;			// magic number, should be 0xBE
	serialPinConfig_t serialPinConfig;
	serialConfig_t	serialConfig;
	gyroConfig_t gyroConfig;
	accelerometerConfig_t accelerometerConfig;
	LedStatusConfig_t ledStatusConfig;
	
#ifdef BEEPER
	beeperConfig_t beeperConfig;
#endif
	
	uint32_t beeper_off_flags;
	uint32_t preferred_beeper_off_flags;
}master_t;

extern master_t masterConfig;

#define LedStatusConfig(x)					(&masterConfig.ledStatusConfig)
#define BeeperConfig(x)						(&masterConfig.beeperConfig)
#define SerialPinConfig(x) 					(&masterConfig.serialPinConfig)
#define SerialConfig(x)						(&masterConfig.serialConfig)
#define GyroConfig(x)						(&masterConfig.gyroConfig)
#define AccelerometerConfig(x)				(&masterConfig.accelerometerConfig)

#endif	// __CONFIGMASTER_H
