#ifndef __AT_ESP_H__
#define __AT_ESP_H__
#include <stdint.h>

#include "stm32f4xx.h"


#define u8 unsigned char
#define ESP8266_RX_BUFFER_LEN 128 // Number of bytes in the serial receive buffer
#define TIMEOUT 1000

typedef struct esp8266 {
	uint8_t (*CheckESP)(void);
	uint8_t (*SetEspMode)(char mode);
	uint8_t (*ConnectWiFi)(char *SSID,char *Passwd);
	uint8_t (*DisconnectWiFi)(void);
	uint8_t (*GetIpAddress)(char *RecvBuffer);
	uint8_t (*GetMACaddress)(char *Recvbuffer);
	uint8_t (*ConnectServer)(char *IPAddress,char *port,char mode);
	uint8_t (*CloseTCPOrUDPConnect)(void);
	uint8_t (*SendStringDataToServer)(char *Data);
	uint8_t (*SendDataToServer)(char *Data,int len);
	uint8_t (*ModemSleep_Getinto)(void);
	uint8_t (*ModemSleep_Quit)(void);
	uint8_t (*DeepSleep_Getinto)(void);
} esp8266;

void esp8266Init(esp8266 *handle);


#endif

