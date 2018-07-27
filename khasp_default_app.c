#include "storage.h"
#include "khasp_default_app.h"
#include "khasp.h"

typedef struct {
	uint64_t requested_services;
} discovery_packet_t;

#define SETTINGS_REQUEST_READ 0
#define SETTINGS_REQUEST_WRITE 1

#define SETTINGS_FLAGS_RUNTIME_UPDATE 1
#define SETTINGS_FLAGS_RESET 2

typedef struct {
        uint8_t request;
        uint32_t flags;
	uint8_t mask[sizeof(storage_common_params_t)]
	storage_common_params_t settings
} settings_packet_t;


int khasp_parse_discovery (khasp_prot_connection* conn, uint32_t* buff, uint32_t len)
{
	discovery_packet_t* discovery = (discovery_packet_t*)buff;
        discovery->requested_services = discovery->requested_services & khasp_get_services_mask();
}

int khasp_parse_settings (khasp_prot_connection* conn, uint32_t* buff, uint32_t len)
{
	settings_packet_t* settings = (settings_packet_t*)buff;
}
 
