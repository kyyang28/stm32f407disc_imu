
#include <string.h>
#include "configMaster.h"
#include "led.h"
#include "sound_beeper.h"
#include "target.h"
#include "serial.h"
#include "sensor.h"
#include "accgyro.h"
#include "filter.h"

/* master config structure with data independent from profiles */
master_t masterConfig;

void ResetSerialPinConfig(serialPinConfig_t *pSerialPinConfig)
{
	for (int port = 0; port < SERIAL_PORT_MAX_INDEX; port++) {
		pSerialPinConfig->ioTagRx[port] = IO_TAG(NONE);
		pSerialPinConfig->ioTagTx[port] = IO_TAG(NONE);
	}
	
	for (int index = 0; index < SERIAL_PORT_COUNT; index++) {
		switch (serialPortIdentifiers[index]) {
			case SERIAL_PORT_USART1:
//				pSerialPinConfig->ioTagRx[SERIAL_PORT_IDENTIFIER_TO_RESOURCE_INDEX(SERIAL_PORT_USART1)] = IO_TAG(UART1_RX_PIN);
//				pSerialPinConfig->ioTagTx[SERIAL_PORT_IDENTIFIER_TO_RESOURCE_INDEX(SERIAL_PORT_USART1)] = IO_TAG(UART1_TX_PIN);
				break;

			case SERIAL_PORT_USART2:
				pSerialPinConfig->ioTagRx[SERIAL_PORT_IDENTIFIER_TO_RESOURCE_INDEX(SERIAL_PORT_USART2)] = IO_TAG(UART2_RX_PIN);
				pSerialPinConfig->ioTagTx[SERIAL_PORT_IDENTIFIER_TO_RESOURCE_INDEX(SERIAL_PORT_USART2)] = IO_TAG(UART2_TX_PIN);
				break;

			case SERIAL_PORT_USART3:
				pSerialPinConfig->ioTagRx[SERIAL_PORT_IDENTIFIER_TO_RESOURCE_INDEX(SERIAL_PORT_USART3)] = IO_TAG(UART3_RX_PIN);
				pSerialPinConfig->ioTagTx[SERIAL_PORT_IDENTIFIER_TO_RESOURCE_INDEX(SERIAL_PORT_USART3)] = IO_TAG(UART3_TX_PIN);
				break;

			case SERIAL_PORT_USART4:
				break;

			case SERIAL_PORT_USART5:
				break;

			case SERIAL_PORT_USART6:
				pSerialPinConfig->ioTagRx[SERIAL_PORT_IDENTIFIER_TO_RESOURCE_INDEX(SERIAL_PORT_USART6)] = IO_TAG(UART6_RX_PIN);
				pSerialPinConfig->ioTagTx[SERIAL_PORT_IDENTIFIER_TO_RESOURCE_INDEX(SERIAL_PORT_USART6)] = IO_TAG(UART6_TX_PIN);
				break;

			case SERIAL_PORT_USART7:
				break;

			case SERIAL_PORT_USART8:
				break;

			case SERIAL_PORT_SOFTSERIAL1:
				break;

			case SERIAL_PORT_SOFTSERIAL2:
				break;
			
			case SERIAL_PORT_USB_VCP:
				break;
			
			case SERIAL_PORT_NONE:
				break;
		}
	}
}

void ResetSerialConfig(serialConfig_t *serialConfig)
{
	memset(serialConfig, 0, sizeof(serialConfig_t));
	serialConfig->serial_update_rate_hz = 100;
	serialConfig->reboot_character		= 'R';
	
	for (int index = 0; index < SERIAL_PORT_COUNT; index++) {
		serialConfig->portConfigs[index].identifier			= serialPortIdentifiers[index];
		serialConfig->portConfigs[index].msp_baudrateIndex	= BAUD_115200;
		serialConfig->portConfigs[index].gps_baudrateIndex	= BAUD_115200;		// gps port for debugging purposes
//		serialConfig->portConfigs[index].gps_baudrateIndex	= BAUD_57600;
		serialConfig->portConfigs[index].blackbox_baudrateIndex	= BAUD_115200;
		serialConfig->portConfigs[index].telemetry_baudrateIndex = BAUD_AUTO;
	}
	
	serialConfig->portConfigs[0].functionMask = FUNCTION_MSP;
}

void ResetLedStatusConfig(LedStatusConfig_t *ledStatusConfig)
{
	for (int i = 0; i < LED_NUMBER; i++) {
		ledStatusConfig->ledTags[i] = IO_TAG_NONE;
	}
	
#ifdef LED3
	ledStatusConfig->ledTags[0] = IO_TAG(LED3);	// LED3 = PD13 ==> DEFIO_TAG__PD13 ==> 4D
#endif

#ifdef LED4
	ledStatusConfig->ledTags[1] = IO_TAG(LED4);	// LED4 = PD12 ==> DEFIO_TAG__PD12 ==> 4C
#endif

#ifdef LED5
	ledStatusConfig->ledTags[2] = IO_TAG(LED5);	// LED5 = PD14 ==> DEFIO_TAG__PD14 ==> 4E
#endif

#ifdef LED6
	ledStatusConfig->ledTags[3] = IO_TAG(LED6);	// LED6 = PD15 ==> DEFIO_TAG__PD15 ==> 4F
#endif
	
	ledStatusConfig->polarity = 0;
}

void targetConfiguration(master_t *config)
{
	/* USART2 */
	int index = findSerialPortIndexByIdentifier(SERIAL_PORT_USART2);
	config->serialConfig.portConfigs[index].functionMask = FUNCTION_GPS;
	
	/* USART1 */
	index = findSerialPortIndexByIdentifier(SERIAL_PORT_USART1);
	config->serialConfig.portConfigs[index].functionMask = FUNCTION_TELEMETRY_FRSKY;
	
	/* USART6 */
	index = findSerialPortIndexByIdentifier(SERIAL_PORT_USART6);
	config->serialConfig.portConfigs[index].functionMask = FUNCTION_RX_SERIAL;
	
	/* USART3 */
	index = findSerialPortIndexByIdentifier(SERIAL_PORT_USART3);
	config->serialConfig.portConfigs[index].functionMask = FUNCTION_BLACKBOX;
}

#ifdef BEEPER
void ResetBeeperConfig(beeperConfig_t *beeperConfig)
{
#ifdef BEEPER_INVERTED
	beeperConfig->isOpenDrain = false;
	beeperConfig->isInverted = true;
#else
	beeperConfig->isOpenDrain = false;			// use IO push-pull, with both transistors connected to the VCC and GND
//	beeperConfig->isOpenDrain = true;			// use IO open-drain, no transistor connected to VCC, only with transistor connected to the GND
	beeperConfig->isInverted = false;
#endif
	beeperConfig->ioTag = IO_TAG(BEEPER);
}
#endif

void CreateDefaultConfig(master_t *config)
{
	/* Global settings */
	config->gyroConfig.gyro_lpf = GYRO_LPF_256HZ;			// 256Hz default
#ifdef STM32F10X
	config->gyroConfig.gyro_sync_denom = 8;
//	config->pidConfig.pid_process_denom = 1;
#elif defined(USE_GYRO_SPI_MPU6000) || defined(USE_GYRO_SPI_MPU6500) || defined(USE_GYRO_SPI_MPU9250)
	config->gyroConfig.gyro_sync_denom = 1;
//	config->pidConfig.pid_process_denom = 4;
#else
	config->gyroConfig.gyro_sync_denom = 4;
//	config->pidConfig.pid_process_denom = 2;
#endif	
	config->gyroConfig.gyro_soft_lpf_type = FILTER_PT1;
	config->gyroConfig.gyro_soft_lpf_hz = 90;
	config->gyroConfig.gyro_soft_notch_hz_1 = 400;
	config->gyroConfig.gyro_soft_notch_cutoff_1 = 300;
	config->gyroConfig.gyro_soft_notch_hz_2 = 200;
	config->gyroConfig.gyro_soft_notch_cutoff_2 = 100;
	
	config->gyroConfig.gyro_align = ALIGN_DEFAULT;
	config->accelerometerConfig.acc_align = ALIGN_DEFAULT;
	
	/* This threshold means how much average gyro reading could differ before re-calibration is triggered */
	config->gyroConfig.gyroMovementCalibrationThreshold = 48;		// moron_threshold of CLI, range from 0 to 200
	config->accelerometerConfig.acc_hardware = ACC_DEFAULT;
	
	ResetSerialPinConfig(&config->serialPinConfig);
	ResetSerialConfig(&config->serialConfig);
	ResetLedStatusConfig(&config->ledStatusConfig);

#ifdef BEEPER
	ResetBeeperConfig(&config->beeperConfig);
#endif

	targetConfiguration(config);
}

static void ResetConfig(void)
{
	CreateDefaultConfig(&masterConfig);
}

void ResetEEPROM(void)
{
	ResetConfig();
}

void CheckEEPROMContainsValidData(void)
{
#if 0
	if (isEEPROMContentValid()) {
		return;
	}
#endif
	
	ResetEEPROM();
}

void beeperOffSet(uint32_t mask)
{
	masterConfig.beeper_off_flags |= mask;
}

void beeperOffSetAll(uint8_t beeperCount)
{
	masterConfig.beeper_off_flags = (1 << beeperCount) - 1;
}

void beeperOffClear(uint32_t mask)
{
	masterConfig.beeper_off_flags &= ~(mask);
}

void beeperOffClearAll(void)
{
	masterConfig.beeper_off_flags = 0;
}

uint32_t getBeeperOffMask(void)
{
	return masterConfig.beeper_off_flags;
}

void setBeeperOffMask(uint32_t mask)
{
	masterConfig.beeper_off_flags = mask;
}

uint32_t getPreferredBeeperOffMask(void)
{
	return masterConfig.preferred_beeper_off_flags;
}

void setPreferredBeeperOffMask(uint32_t mask)
{
	masterConfig.preferred_beeper_off_flags = mask;
}
