#include "../new_common.h"
#include "../quicktick.h"
#include "../logging/logging.h"
#include "drv_uart.h"
#include "drv_tuyaMCU.h"

#define DEFAULT_UART_BAUD_RATE		9600

#define MIN_TUYAMCU_PACKET_SIZE		7
#define MAX_TUYAMCU_PACKET_SIZE		192

typedef struct 
{
	int BaudRate;
	int TxTimer;
} tuyaMCUUart_t;

typedef struct 
{
	tuyaMCUUart_t UART;
} tuyaMCUContext_t;

tuyaMCUContext_t tuyaMCUContext = { 0 };



void TuyaMCU_PrintPacket(byte *data, int len) {
}

int TuyaMCU_GetTuyaMcuPacket(byte* rxPacketData, int maxPacketSize)
{
	int i, len, ret;
	int rxDataSize;
	char printfSkipDebug[256] = {0};
	char tmpBuf[4] = {0};

	ret = 0;
	len = 0;

	rxDataSize = UART_GetDataSize();
	if (rxDataSize < MIN_TUYAMCU_PACKET_SIZE) {
		return ret;
	}

	while (rxDataSize > 0)
	{
		rxPacketData[0] = UART_GetByte(0);
		rxPacketData[1] = UART_GetByte(1);
		if (rxPacketData[0] != 0x55 || rxPacketData[1] != 0xAA) {
			UART_ConsumeBytes(1);
			if (len + 2 < sizeof(printfSkipDebug)) {
				snprintf(tmpBuf, sizeof(tmpBuf), "%02X ", rxPacketData[0]);
				strcat_safe(printfSkipDebug, tmpBuf, sizeof(printfSkipDebug));
			}
			len++;
			rxDataSize--;
		}
		else {
			break;
		}
	}

	if (len > 0) {
		addLogAdv(LOG_INFO, LOG_FEATURE_TUYAMCU, "Consumed %i unwanted non-header byte in Tuya MCU buffer\n", len);
		addLogAdv(LOG_INFO, LOG_FEATURE_TUYAMCU, "Skipped data (part) %s\n", printfSkipDebug);
	}

	if (rxDataSize < MIN_TUYAMCU_PACKET_SIZE || rxPacketData[0] != 0x55 || rxPacketData[1] != 0xAA) {
		return ret;
	}

	rxPacketData[2] = UART_GetByte(2); // Version
	rxPacketData[3] = UART_GetByte(3); // Command
	rxPacketData[4] = UART_GetByte(4); // Length Hi
	rxPacketData[5] = UART_GetByte(5); // Length Lo

	len = ((rxPacketData[4] << 8) | rxPacketData[5]) + MIN_TUYAMCU_PACKET_SIZE;

	if (rxDataSize >= len) {
		if (len <= maxPacketSize) {
			for (i = 6; i < len; i++) {
				rxPacketData[i] = UART_GetByte(i);
			}
			ret = len;
		}
		else {
			addLogAdv(LOG_INFO, LOG_FEATURE_TUYAMCU, "TuyaMCU packet too large, %i > %i\n", len, maxPacketSize);
		}
		UART_ConsumeBytes(len);
	}

	return ret;
}

bool TuyaMCU_SendFromQueue() {
	return true;
}

void TuyaMCU_ReceiveData() {
	byte rxPacketData[MAX_TUYAMCU_PACKET_SIZE];
	int rxPacketLen;

	while (1)
	{
		rxPacketLen = TuyaMCU_GetTuyaMcuPacket(rxPacketData, sizeof(rxPacketData));
		if (rxPacketLen > 0) {
			TuyaMCU_PrintPacket(rxPacketData, rxPacketLen);
		}
		else {
			break;
		}
	}
}

/*************************************************************  
* Driver Functions 
**************************************************************/

void TuyaMCU_Init() {
	tuyaMCUContext.UART.BaudRate = DEFAULT_UART_BAUD_RATE;
	UART_InitUART(tuyaMCUContext.UART.BaudRate, 0, false);
	UART_InitReceiveRingBuffer(1024);
}

void TuyaMCU_RunFrame() {
	TuyaMCU_ReceiveData();
	
	if (tuyaMCUContext.UART.TxTimer > 0) {
		tuyaMCUContext.UART.TxTimer -= g_deltaTimeMS;
	} else {
		if (TuyaMCU_SendFromQueue()) {
			tuyaMCUContext.UART.TxTimer = 100;
		}
	}
}

void TuyaMCU_OnEverySecond() {
}

void TuyaMCU_Shutdown() {
}


/*************************************************************  
* Functions called from external code 
**************************************************************/

bool TuyaMCU_IsChannelUsedByTuyaMCU(int channel) {
	return false;
}

void TuyaMCU_OnChannelChanged(int channel, int iVal) {
}

bool TuyaMCU_IsLEDRunning() {
	return false;
}

void TuyaMCU_OnRGBCWChange(const float *rgbcw, int bLightEnableAll, int iLightMode, 
							float brightnessRange01, float temperatureRange01) {
}

// what the fuck ? Used in: ../httpserver/json_interface.c
int http_obk_json_dps(int id, void* request, jsonCb_t printer) {
	return 0;
}
