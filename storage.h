#ifndef __STORAGE_H__
#define __STORAGE_H__

#define STORAGE_SECTOR 0x50

typedef struct {
	char softap_ssid[32];
	char softap_pass[32];
	char station_ssid[32];
	char station_pass[32];
	int led_num;
	char last_led_status[4096-64-4];
} storage_params_t;

extern storage_params_t storage_params;

void storage_save();
void storage_load();

#endif
