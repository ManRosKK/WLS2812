#include "mem.h"
#include "c_types.h"
#include "user_interface.h"
#include "ets_sys.h"
#include "driver/uart.h"
#include "osapi.h"
#include "espconn.h"
#include "data.h"
#include "wifi.h"
#include "storage.h"
#include "gpio.h"

#define PORT 7777
#define SERVER_TIMEOUT 1500
#define MAX_CONNS 5
#define MAX_FRAME 2000

#define procTaskPrio        0
#define procTaskQueueLen    1

static volatile os_timer_t some_timer;
static struct espconn *pUdpServer;

/*void user_rf_pre_init(void)
{
	//nothing.
}*/


/*char * strcat( char * dest, char * src )
{
	return strcat(dest, src );
}*/



//Tasks that happen all the time.

os_event_t    procTaskQueue[procTaskQueueLen];
static uint8_t printed_ip = 0;
static void ICACHE_FLASH_ATTR
procTask(os_event_t *events)
{
	system_os_post(procTaskPrio, 0, 0 );

	if( events->sig == 0 && events->par == 0 )
	{
		//Idle Event.
		struct station_config wcfg;
		char stret[256];
		char *stt = &stret[0];
		struct ip_info ipi;

		int stat = wifi_station_get_connect_status();

//		printf( "STAT: %d\n", stat );

		if( stat == STATION_WRONG_PASSWORD || stat == STATION_NO_AP_FOUND || stat == STATION_CONNECT_FAIL )
		{
			wifi_set_opmode_current( 2 );
			stt += ets_sprintf( stt, "Connection failed: %d\n", stat );
			uart0_sendStr(stret);
		}

		if( stat == STATION_GOT_IP && !printed_ip )
		{
			wifi_station_get_config( &wcfg );
			wifi_get_ip_info(0, &ipi);
			stt += ets_sprintf( stt, "STAT: %d\n", stat );
			stt += ets_sprintf( stt, "IP: %d.%d.%d.%d\n", (ipi.ip.addr>>0)&0xff,(ipi.ip.addr>>8)&0xff,(ipi.ip.addr>>16)&0xff,(ipi.ip.addr>>24)&0xff );
			stt += ets_sprintf( stt, "NM: %d.%d.%d.%d\n", (ipi.netmask.addr>>0)&0xff,(ipi.netmask.addr>>8)&0xff,(ipi.netmask.addr>>16)&0xff,(ipi.netmask.addr>>24)&0xff );
			stt += ets_sprintf( stt, "GW: %d.%d.%d.%d\n", (ipi.gw.addr>>0)&0xff,(ipi.gw.addr>>8)&0xff,(ipi.gw.addr>>16)&0xff,(ipi.gw.addr>>24)&0xff );
			stt += ets_sprintf( stt, "WCFG: /%s/%s/\n", wcfg.ssid, wcfg.password );
			uart0_sendStr(stret);
			printed_ip = 1;
		}
	}

}

//Timer event.
char T[]= {0x0, 0x0, 0x0, 0x00, 0xFF,0x00, 0x00,0x00,0xFF, 0xFF,0x00,0x00, 0x00,0xFF,0x00, 0x00,0x00,0x00};
char I = 0;
char P[] = {0x00,0x00,0xFF};
static void ICACHE_FLASH_ATTR
 myTimer(void *arg)
{
//	T[15]++; T[16]+=2; T[17]+=3;
//	T[I++] =0xFF;
//	I%=3;
//	WS2812OutBuffer( T, 3*6 );
WS2812OutBufferPattern(P,3, 160);
	uart0_sendStr(".");
	

}


//Called when new packet comes in.
static void ICACHE_FLASH_ATTR
udpserver_recv(void *arg, char *pusrdata, unsigned short len)
{
	struct espconn *pespconn = (struct espconn *)arg;
	uint8_t buffer[MAX_FRAME];

//Seems to be optional, still can cause crashes.
	WS2812OutBuffer( pusrdata, len );
	ets_sprintf( buffer, "%03x", len );
	uart0_sendStr("---------");
}

void ICACHE_FLASH_ATTR charrx( uint8_t c )
{
	//Called from UART.
}

void WifiCb(uint8_t status)
{
	switch (status) {
	case STATION_IDLE:
		uart0_sendStr("STATION_IDLE\r\n");
		break;
	case STATION_CONNECTING:
		uart0_sendStr("STATION_CONNECTING\r\n");
		break;
	case STATION_WRONG_PASSWORD:
		uart0_sendStr("STATION_WRONG_PASSWORD\r\n");
		break;
	case STATION_NO_AP_FOUND:
		uart0_sendStr("STATION_NO_AP_FOUND\r\n");
		break;
	case STATION_CONNECT_FAIL:
		uart0_sendStr("STATION_CONNECT_FAIL\r\n");
		break;
	case STATION_GOT_IP:
		uart0_sendStr("STATION_GOT_IP\r\n");
		break;
	case CLIENT_CONNECTED:
		os_printf("CLIENT_CONNECTED\r\n");
		break;
	}
}

void user_init(void)
{
	uart_init(BIT_RATE_115200, BIT_RATE_115200);
	int wifiMode = wifi_get_opmode();
	storage_load();
	uart0_sendStr("\r\nCustom Server\r\n");
/*	os_printf("-------ssid:  %s\r\n", storage_params.softap_ssid);
	os_sprintf(storage_params.softap_ssid, "%s", "");
	storage_save();*/
	WIFI_Connect(/*"POZIOMKA24"*/NULL, /*"logarytm1"*/NULL, "LED_DRIVER", "test1234", WifiCb);
//	wifi_set_opmode( 2 ); //We broadcast our ESSID, wait for peopel to join.

/*
	struct station_config stationConf;
	wifi_set_opmode( 1 ); //We broadcast our ESSID, wait for peopel to join.
	os_memcpy(&stationConf.ssid, "xxx", ets_strlen( "xxx" ) + 1);
	os_memcpy(&stationConf.password, "yyy", ets_strlen( "yyy" ) + 1);

	wifi_set_opmode( 1 );
	wifi_station_set_config(&stationConf);
	wifi_station_connect();**/

	pUdpServer = (struct espconn *)os_zalloc(sizeof(struct espconn));
	ets_memset( pUdpServer, 0, sizeof( struct espconn ) );
//	espconn_create( pUdpServer );
	pUdpServer->type = ESPCONN_UDP;
	pUdpServer->proto.udp = (esp_udp *)os_zalloc(sizeof(esp_udp));
	pUdpServer->proto.udp->local_port = 7777;

/*	wifi_station_dhcpc_start();
*/
	if( espconn_create( pUdpServer ) )
	{
		while(1) { uart0_sendStr( "\r\nFAULT\r\n" ); }
	}
        if( espconn_regist_recvcb(pUdpServer, udpserver_recv) )
        {
                while(1) { uart0_sendStr( "\r\nFAULT2\r\n" ); }
        }

	int j = 100;
	char outbuffer[] = { 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff,0xff,0xff, 0x00,0xff,0x00 };
uart0_sendStr("\r\nCustom Server2\r\n");
	while(j--) {
		int k = 1000;
		WS2812OutBuffer( outbuffer, 3 ); //Initialize the output.
		while(k--);
	}
	uart0_sendStr("\r\nCustom Server4\r\n");
	//Add a process
//	system_os_task(procTask, procTaskPrio, procTaskQueue, procTaskQueueLen);

//	uart0_sendStr("\r\nBooting\r\n");
//	WS2812OutBuffer( outbuffer, sizeof(outbuffer) );

	//Timer example
	os_timer_disarm(&some_timer);
	os_timer_setfn(&some_timer, (os_timer_func_t *)myTimer, NULL);
	os_timer_arm(&some_timer, 100, 1);

//	system_os_post(procTaskPrio, 0, 0 );
}


