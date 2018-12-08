#ifndef __STORAGE_H__
#define __STORAGE_H__

#define STORAGE_SECTOR 0x50

//gdy moduł jest w trybie serwisowym (brak ustawien/3 próby niepodłączenia do sieci lub komenda zerujaca/przełącznik? to ustawia się na domyślny STATION_MODE inaczej łączy się 

typedef struct {
	char softap_ssid[32];
	char softap_pass[32];
	char station_ssid[32];
	char station_pass[32];
        char reserved[256-128];
} storage_common_params_t;

typedef struct {
	storage_common_params_t  common_settings;
	int led_num;
	char last_led_status[4096-sizeof(storage_common_params_t)-sizeof(int)];
} storage_params_t;

extern storage_params_t storage_params;

void storage_save();
void storage_load();
storage_common_params_t* storage_get_common();
#endif
