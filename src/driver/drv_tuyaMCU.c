#include "../new_common.h"

#include "drv_tuyaMCU.h"

void TuyaMCU_Init() {
}

void TuyaMCU_RunFrame() {
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

void TuyaMCU_OnRGBCWChange(const float *rgbcw, int bLightEnableAll, int iLightMode, float brightnessRange01, float temperatureRange01) {
}

// what the fuck ? Used in: ../httpserver/json_interface.c
int http_obk_json_dps(int id, void* request, jsonCb_t printer) {
	return 0;
}
