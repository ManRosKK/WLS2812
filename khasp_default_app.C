

int khasp_parse_discovery (khasp_prot_connection* conn, uint32_t* buff, uint32_t len)
{

}

int khasp_discovery_broadcast_request()
{

}

int khasp_discovery_broadcast_welcome()
{

}

int khasp_parse_settings (khasp_prot_connection* conn, uint32_t* buff, uint32_t len)
{

}

void init_khasp_default()
{
	khasp_register_packet_type (khasp_parse_discovery, KHASP_PROT_TYPE_DISCOVERY);
	khasp_register_packet_type (khasp_parse_settings, KHASP_PROT_TYPE_SETTINGS);
	//do something call broadcast when it is needed
}

