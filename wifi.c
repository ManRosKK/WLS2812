/*
 * wifi.c
 *
 *  Created on: Dec 30, 2014
 *      Author: Minh
 */
#include "wifi.h"
#include "user_interface.h"
#include "osapi.h"
#include "espconn.h"
#include "os_type.h"
#include "mem.h"
#include "user_config.h"
#include "driver/uart.h"

//#include "config.h"

static ETSTimer WiFiLinker;
WifiCallback wifiCb = NULL;
static uint8_t wifiStatus = STATION_IDLE, lastWifiStatus = STATION_IDLE;
struct station_config stationConf;
struct softap_config softapConf;
static void ICACHE_FLASH_ATTR wifi_check_ip(void *arg)
{
	struct ip_info ipConfig;
	int softap_flag = 0;

	os_timer_disarm(&WiFiLinker);

	struct station_info* station = wifi_softap_get_station_info();
	while(station) {
		softap_flag = 1;
		os_printf("bssid: %s, ip: %s/n",	
			MAC2STR(station->bssid),	IP2STR(&station->ip));
		station	= STAILQ_NEXT(station,	next);
	}
	wifi_softap_free_station_info();	

	if (softap_flag)
		wifiCb(CLIENT_CONNECTED);

	wifi_get_ip_info(STATION_IF, &ipConfig);
	wifiStatus = wifi_station_get_connect_status();
	if (wifiStatus == STATION_GOT_IP && ipConfig.ip.addr != 0)
	{
		os_printf("GOT_IP");
		os_timer_setfn(&WiFiLinker, (os_timer_func_t *)wifi_check_ip, NULL);
		os_timer_arm(&WiFiLinker, 2000, 0);


	}
	else
	{
		if(wifi_station_get_connect_status() == STATION_WRONG_PASSWORD)
		{

			uart0_sendStr("STATION_WRONG_PASSWORD\r\n");
			wifi_station_connect();


		}
		else if(wifi_station_get_connect_status() == STATION_NO_AP_FOUND)
		{

			uart0_sendStr("STATION_NO_AP_FOUND\r\n");
			wifi_station_connect();


		}
		else if(wifi_station_get_connect_status() == STATION_CONNECT_FAIL)
		{

			uart0_sendStr("STATION_CONNECT_FAIL\r\n");
			wifi_station_connect();

		}
		else
		{
			uart0_sendStr("STATION_IDLE\r\n");
		}

		os_timer_setfn(&WiFiLinker, (os_timer_func_t *)wifi_check_ip, NULL);
		os_timer_arm(&WiFiLinker, 500, 0);
	}
	if(wifiStatus != lastWifiStatus){
		lastWifiStatus = wifiStatus;
		if (wifiStatus == STATION_GOT_IP && ipConfig.ip.addr != 0)
			wifi_station_set_config(&stationConf);

		if(wifiCb)
			wifiCb(wifiStatus);
	}
}

void ICACHE_FLASH_ATTR WIFI_Connect(uint8_t* ssid, uint8_t* pass, uint8_t* softap_ssid, uint8_t* softap_pass,  WifiCallback cb)
{

	uart0_sendStr("WIFI_INIT\r\n");
	wifi_set_opmode(STATIONAP_MODE);

	//wifi_station_set_auto_connect(FALSE);
	wifiCb = cb;

//	os_memset(&stationConf, 0, sizeof(struct station_config));

/*	if (ssid[0] == 0)
		os_memcpy(ssid, "TEST1", strlen("TEST1")+1);*/
/*
	if (ssid) {
		os_sprintf(stationConf.ssid, "%s", ssid);
		os_sprintf(stationConf.password, "%s", pass);
	}

	wifi_station_set_config_current(&stationConf);
*/
	if (softap_ssid != NULL) {
		os_memcpy(softap_ssid, "LED_CONTROLLER", strlen("LED_CONTROLLER")+1);
		os_memcpy(softap_pass, "1234567890", strlen("1234567890")+1);

		wifi_softap_get_config(&softapConf);
/*
struct softap_config {
    uint8 ssid[32];
    uint8 password[64];
    uint8 ssid_len;     // Note: Recommend to set it according to your ssid
    uint8 channel;      // Note: support 1 ~ 13
    AUTH_MODE authmode; // Note: Don't support AUTH_WEP in softAP mode.
    uint8 ssid_hidden;  // Note: default 0
    uint8 max_connection;       // Note: default 4, max 4
    uint16 beacon_interval;     // Note: support 100 ~ 60000 ms, default 100
};


*/
		wifi_softap_dhcps_stop();
		os_sprintf(softapConf.ssid, "%s", softap_ssid);
        	os_sprintf(softapConf.password, "%s", softap_pass);
		softapConf.ssid_len = strlen(softap_ssid);
		softapConf.authmode = AUTH_WPA_WPA2_PSK;
		softapConf.ssid_hidden = 0;// zmienic na 1;
		wifi_softap_set_config(&softapConf);

		struct ip_info ipinfo;
		wifi_get_ip_info(SOFTAP_IF, &ipinfo);
		IP4_ADDR(&ipinfo.ip, 192, 168, 4, 1);
		IP4_ADDR(&ipinfo.gw, 192, 168, 4, 1);
		IP4_ADDR(&ipinfo.netmask, 255, 255, 255, 0);
		wifi_set_ip_info(SOFTAP_IF, &ipinfo);
		wifi_softap_dhcps_start();
	}

	if (ssid) {
		wifi_station_connect();
		wifi_station_set_config_current(&stationConf);
		os_sprintf(stationConf.ssid, "%s", ssid);
		os_sprintf(stationConf.password, "%s", pass);

		wifi_station_set_config_current(&stationConf);

		wifi_station_set_auto_connect(TRUE);
		wifi_station_connect();
	}

	os_timer_disarm(&WiFiLinker);
	os_timer_setfn(&WiFiLinker, (os_timer_func_t *)wifi_check_ip, NULL);
	os_timer_arm(&WiFiLinker, 1000, 0);


//	os_memset(&stationConf, 0, sizeof(struct station_config));

/*	if (ssid[0] == 0)
		os_memcpy(ssid, "TEST1", strlen("TEST1")+1);*/

}

