void TuyaMCU_Init();
void TuyaMCU_OnEverySecond();
void TuyaMCU_RunFrame();
void TuyaMCU_Shutdown();

bool TuyaMCU_IsChannelUsedByTuyaMCU(int channelIndex);
void TuyaMCU_OnChannelChanged(int channel,int iVal);
