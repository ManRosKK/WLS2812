/*
 * wifi.h
 *
 *  Created on: Dec 30, 2014
 *      Author: Minh
 */

#ifndef USER_WIFI_H_
#define USER_WIFI_H_
#include "os_type.h"
typedef void (*WifiCallback)(uint8_t);
void ICACHE_FLASH_ATTR WIFI_Connect(uint8_t* ssid, uint8_t* pass, uint8_t* softap_ssid, uint8_t* softap_pass, WifiCallback cb);

#define CLIENT_CONNECTED 0x06

#endif /* USER_WIFI_H_ */
