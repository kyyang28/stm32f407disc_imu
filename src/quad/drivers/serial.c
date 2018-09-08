
#include <string.h>
#include "serial.h"
#include "utils.h"
#include "serial_uart.h"

#if defined(USE_VCP)
#include "serial_usb_vcp.h"
#endif

static serialConfig_t *serialConfig;
static serialPortUsage_t serialPortUsageList[SERIAL_PORT_COUNT];

const serialPortIdentifier_e serialPortIdentifiers[SERIAL_PORT_COUNT] = {
#ifdef USE_VCP	
	SERIAL_PORT_USB_VCP,
#endif
#ifdef USE_UART1
	SERIAL_PORT_USART1,
#endif
#ifdef USE_UART2
	SERIAL_PORT_USART2,
#endif
#ifdef USE_UART3
	SERIAL_PORT_USART3,
#endif
#ifdef USE_UART4
	SERIAL_PORT_USART4,
#endif
#ifdef USE_UART5
	SERIAL_PORT_USART5,
#endif
#ifdef USE_UART6
	SERIAL_PORT_USART6,
#endif
#ifdef USE_UART7
	SERIAL_PORT_USART7,
#endif
#ifdef USE_UART8
	SERIAL_PORT_USART8,
#endif
#ifdef USE_SOFTSERIAL1
	SERIAL_PORT_SOFTSERIAL1,
#endif
#ifdef USE_SOFTSERIAL2
	SERIAL_PORT_SOFTSERIAL2,
#endif
};

/* see baudRate_e */
const uint32_t baudRates[] = {0, 9600, 19200, 38400, 57600, 115200, 230400, 250000,
        400000, 460800, 500000, 921600, 1000000, 1500000, 2000000, 2470000};

typedef struct findSerialPortConfigState_s {
	uint8_t lastIndex;
}findSerialPortConfigState_t;

static findSerialPortConfigState_t findSerialPortConfigState;

void serialInit(serialConfig_t *initSerialConfig)
{
	serialConfig = initSerialConfig;
	memset(&serialPortUsageList, 0, sizeof(serialPortUsageList));
	
	for (int index = 0; index < SERIAL_PORT_COUNT; index++) {
		serialPortUsageList[index].identifier = serialPortIdentifiers[index];
	}
}

serialPortConfig_t *findNextSerialPortConfig(serialPortFunction_e function)
{
	while (findSerialPortConfigState.lastIndex < SERIAL_PORT_COUNT) {
		serialPortConfig_t *candidate = &serialConfig->portConfigs[findSerialPortConfigState.lastIndex++];
		
		if (candidate->functionMask & function) {
			return candidate;
		}
	}
	
	return NULL;
}

serialPortConfig_t *findSerialPortConfig(serialPortFunction_e function)
{
	memset(&findSerialPortConfigState, 0, sizeof(findSerialPortConfigState));
	return findNextSerialPortConfig(function);
}

serialPortUsage_t *findSerialPortUsageByIdentifier(serialPortIdentifier_e identifier)
{
	uint8_t index;
	for (index = 0; index < SERIAL_PORT_COUNT; index++) {
		serialPortUsage_t *candidate = &serialPortUsageList[index];
		if (candidate->identifier == identifier) {
			return candidate;
		}
	}
	
	return NULL;
}

int findSerialPortIndexByIdentifier(serialPortIdentifier_e identifier)
{
	for (int index = 0; index < SERIAL_PORT_COUNT; index++) {
		if (serialPortIdentifiers[index] == identifier) {
			return index;
		}
	}
	
	return -1;
}

serialPort_t *openSerialPort(
	serialPortIdentifier_e identifier,
	serialPortFunction_e function,
	serialReceiveCallbackPtr rxCallback,
	uint32_t baudRate,
	portMode_t mode,
	portOptions_t options)
{
#if (!defined(USE_UART1) && !defined(USE_UART2) && !defined(USE_UART3) && !defined(USE_UART4) && !defined(USE_UART5) && !defined(USE_UART6) && !defined(USE_SOFTSERIAL1) && !defined(USE_SOFTSERIAL2))
	UNUSED(rxCallback);
	UNUSED(baudRate);
	UNUSED(mode);
	UNUSED(options);
#endif
	
	serialPortUsage_t *serialPortUsage = findSerialPortUsageByIdentifier(identifier);
	if (!serialPortUsage || serialPortUsage->function != FUNCTION_NONE) {
		/* not available or already in use */
		return NULL;
	}
	
	serialPort_t *serialPort = NULL;
	
	switch (identifier) {
#ifdef USE_VCP
		case SERIAL_PORT_USB_VCP:
			serialPort = usbVcpOpen();
			break;
#endif
#ifdef USE_UART1
		case SERIAL_PORT_USART1:
			serialPort = uartOpen(USART1, rxCallback, baudRate, mode, options);
			break;
#endif
#ifdef USE_UART2
		case SERIAL_PORT_USART2:
			serialPort = uartOpen(USART2, rxCallback, baudRate, mode, options);
			break;
#endif
#ifdef USE_UART3
		case SERIAL_PORT_USART3:
			serialPort = uartOpen(USART3, rxCallback, baudRate, mode, options);
			break;
#endif
#ifdef USE_UART4
		case SERIAL_PORT_USART4:
			break;
#endif
#ifdef USE_UART5
		case SERIAL_PORT_USART5:
			break;
#endif
#ifdef USE_UART6
		case SERIAL_PORT_USART6:
			serialPort = uartOpen(USART6, rxCallback, baudRate, mode, options);
			break;
#endif
#ifdef USE_UART7
		case SERIAL_PORT_USART7:
			break;
#endif
#ifdef USE_UART8
		case SERIAL_PORT_USART8:
			break;
#endif
#ifdef USE_SOFTSERIAL1
		case SERIAL_PORT_SOFTSERIAL1:
			break;
#endif
#ifdef USE_SOFTSERIAL2
		case SERIAL_PORT_SOFTSERIAL2:
			break;
#endif
		default:
			break;
	}	// end of switch
	
	if (!serialPort) {
		return NULL;
	}
	
	serialPort->identifier = identifier;
	serialPortUsage->function = function;
	serialPortUsage->serialPort = serialPort;
	
	return serialPort;
}

void serialWrite(serialPort_t *instance, uint8_t ch)
{
	instance->vTable->serialWrite(instance, ch);
}

bool isSerialTransmitBufferEmpty(const serialPort_t *instance)
{
    return instance->vTable->isSerialTransmitBufferEmpty(instance);
}

void serialPrint(serialPort_t *instance, const char *str)
{
    uint8_t ch;
    while ((ch = *(str++)) != 0) {
        serialWrite(instance, ch);
    }
}
