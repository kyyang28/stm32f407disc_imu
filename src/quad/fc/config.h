#ifndef __CONFIG_H
#define __CONFIG_H

#include "configMaster.h"

void ResetEEPROM(void);
void CheckEEPROMContainsValidData(void);
void ResetLedStatusConfig(LedStatusConfig_t *ledStatusConfig);
void CreateDefaultConfig(master_t *config);

void beeperOffSet(uint32_t mask);
void beeperOffSetAll(uint8_t beeperCount);
void beeperOffClear(uint32_t mask);
void beeperOffClearAll(void);
uint32_t getBeeperOffMask(void);
void setBeeperOffMask(uint32_t mask);
uint32_t getPreferredBeeperOffMask(void);
void setPreferredBeeperOffMask(uint32_t mask);

#endif	// __CONFIG_H
