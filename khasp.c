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

#define KHASP_PROT_MAGIC 0x8420D5F1
#define KHASP_PROT_VERSION 1
#define KHASP_PROT_TYPE_DISCOVERY 0
#define KHASP_PROT_TYPE_SETTINGS 1

#define KHASP_PROT_TYPE_LED_STRIP 2

#define KHASP_APPLICATION_NUM_MAX 64

typedef struct _khasp_prot_header {
	uint32_t magic;
	uint32_t packet_type;
	uint32_t id;
        uint32_t version;
        uint32_t auth;
	uint32_t len;
	uint8_t  payload[];
} khasp_prot_header;

typedef struct _khasp_prot_connection {
	struct espconn *pespconn;
	uint32_t id;
	uint32_t packet_type;
} khasp_prot_connection;

enum packet_result {
	KHASP_ACK = 0,
	KHASP_NACK_NOT_SUPPORTED = -1,
	KHASP_NACK_BAD_FORMAT = -2,
	KHASP_NACK_ERROR = -3
}

static int (*khasp_parse_application_cb[KHASP_APPLICATION_NUM_MAX])(khasp_prot_connection* conn, uint8_t* buff, uint32_t len);

int khasp_send_packet_from_conn (struct espconn *pespconn, uint8_t* buff, uint32_t len)
{

}

/**
network STATION or SOFTAP
*/
int khasp_send_packet_broadcast (int32_t network, uint8_t* buff, uint32_t len)
{

}

int khasp_send_packet_from_ip (uint8_t* ip, uint8_t* buff, uint32_t len)
{

}

int khasp_send_ack (struct espconn *pespconn, uint32_t id, uint32_t packet_type, int32_t result)
{

}

int khasp_parse_discovery (khasp_prot_connection* conn, uint32_t* buff, uint32_t len)
{

}

int khasp_parse_settings (khasp_prot_connection* conn, uint32_t* buff, uint32_t len)
{

}

int khasp_parse_application (khasp_prot_connection* conn, uint8_t* buff, uint32_t len)
{
	if (packet_type >= KHASP_APPLICATION_NUM_MAX || !khasp_parse_application_cb[packet_type])
	{
		return KHASP_NACK_NOT_SUPPORTED;
	}

	return khasp_parse_application_cb[packet_type] (conn, buff, len);
}

int khasp_parse_packet (struct espconn *pespconn, uint8_t* buff, uint32_t len)
{
	khasp_prot_header* header = (khasp_prot_header*)buff;
	uint32_t res;
	uint8_t* payload;
	uint32_t payload_len;
	khasp_prot_connection conn;
	
	if (len < sizeof(khasp_prot_header))
		return KHASP_NACK_BAD_FORMAT;

	if (header->magic != KHASP_PROT_MAGIC)
		return KHASP_NACK_BAD_FORMAT;

	if (header->version > KHASP_PROT_VERSION) {
		khasp_send_ack(pespconn, header->id, header->packet_type, KHASP_NACK_NOT_SUPPORTED);
		return KHASP_NACK_NOT_SUPPORTED;
	}

	conn.pespconn = pespconn;
	conn.id = header->id;
	conn.packet_type = header->packet_type;
	payload = header->payload;
	payload_len = header->len;

	switch (header->packet_type) {
		case KHASP_PROT_TYPE_DISCOVERY:
			res = khasp_parse_discovery(conn, payload, payload_len);
			break;
		case KHASP_PROT_TYPE_SETTINGS:
			res = khasp_parse_settings(conn, payload, payload_len);
			break;
		default:
			res = khasp_parse_application(conn, payload, payload_len);
	}

	khasp_send_ack(conn.pespconn, conn.id, conn.packet_type, res);
	return res;
}

int khasp_register_packet_type(int (*cb)(khasp_prot_connection* , uint8_t* , uint32_t), uint32_t packet_type)
{
	if (packet_type >= KHASP_APPLICATION_NUM_MAX)
        {
                return KHASP_NACK_NOT_SUPPORTED;
        }

        khasp_parse_application_cb[packet_type]  = cb;
	return 0;
}
