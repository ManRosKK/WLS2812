#include "ets_sys.h"
#include "os_type.h"
#include "mem.h"
#include "osapi.h"
#include "user_interface.h"
#include "espconn.h"
#include "storage.h"



storage_params_t storage_params;

void storage_save()
{
	system_param_save_with_protect(STORAGE_SECTOR, &storage_params, sizeof(storage_params));
}

void storage_load()
{
	system_param_load(STORAGE_SECTOR, 0, &storage_params, sizeof(storage_params));
}
