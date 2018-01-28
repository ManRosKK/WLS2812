


int khasp_parse_led_strip (khasp_prot_connection* conn, uint32_t* buff, uint32_t len)
{

}


void init_khasp_led_strip()
{
        khasp_register_packet_type (khasp_parse_led_strip, KHASP_PROT_TYPE_LED_STRIP);
}

