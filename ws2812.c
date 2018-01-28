#include "ws2812.h"
#include "ets_sys.h"
#include "data.h"
#include "osapi.h"

/*#define GPIO_OUTPUT_SET(gpio_no, bit_value) \
	gpio_output_set(bit_value<<gpio_no, ((~bit_value)&0x01)<<gpio_no, 1<<gpio_no,0)
*/

//I just used a scope to figure out the right time periods.

void  SEND_WS_0()
{
	uint8_t time;
	time = 4; while(time--) WRITE_PERI_REG( PERIPHS_GPIO_BASEADDR + GPIO_ID_PIN(WSGPIO), 1 );
	time = 9; while(time--) WRITE_PERI_REG( PERIPHS_GPIO_BASEADDR + GPIO_ID_PIN(WSGPIO), 0 );
}

void  SEND_WS_1()
{
	uint8_t time; 
	time = 8; while(time--) WRITE_PERI_REG( PERIPHS_GPIO_BASEADDR + GPIO_ID_PIN(WSGPIO), 1 );
	time = 6; while(time--) WRITE_PERI_REG( PERIPHS_GPIO_BASEADDR + GPIO_ID_PIN(WSGPIO), 0 );
}

void   WS2812OutBuffer( uint8_t * buffer, uint16_t length )
{
	uint16_t i;
//uart0_sendStr("\r\nCustom Server-1\r\n");
	GPIO_OUTPUT_SET(GPIO_ID_PIN(WSGPIO), 0);
uart0_sendStr("\r\nCustom Server-2\r\n");
	ets_intr_lock(); 
//uart0_sendStr("\r\nCustom Server-3\r\n");
	for( i = 0; i < length; i++ )
	{
		uint8_t mask = 0x80;
		uint8_t byte = buffer[i];
//uart0_sendStr("\r\nCustom Server-4\r\n");
		while (mask) 
		{
			if( byte & mask ) SEND_WS_1(); else SEND_WS_0();
			mask >>= 1;
        }
	}

	ets_intr_unlock(); 
}



