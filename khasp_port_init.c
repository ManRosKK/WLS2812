
void init_khasp_default()
{
        khasp_register_packet_type (khasp_parse_discovery, KHASP_PROT_TYPE_DISCOVERY);
        khasp_register_packet_type (khasp_parse_settings, KHASP_PROT_TYPE_SETTINGS);
        //do something call broadcast when it is needed
}

void init_khasp_led_strip()
{
        khasp_register_packet_type (khasp_parse_led_strip, KHASP_PROT_TYPE_LED_STRIP);
}

