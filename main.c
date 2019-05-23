#include "LED/utils_led.h"
#include "RTC/i2c.h"
#include "RTC/rtc.h"

int type = 0;

int main()
{
	
	DDRB &= ~(1 << PB2);
	PORTB |= (1 << PB2);

	DDRD |= (1 << PD7);
	PORTD &= ~(1 << PD7);


	while(1)
	{
		/* Check if button was pressed first */
		if ((PINB & (1 << PB2)) == 0) {
			type = (type == 0) ? 1 : 0;
			//delay is necessary for fast changes 
			 _delay_ms(600);
		} 
		
		switch(type) {
			case 0: {
				PORTD &= ~(1 << PD7);

				RGB screen = (RGB) { 255, 95, 85 };
				RGB sec_color = {255, 127, 0};
				RGB minute_color = (RGB) { 2, 87, 108 };
				RGB hour_color = (RGB) { 255, 10, 10 };
				clock_time_color(screen, sec_color, minute_color, hour_color);
				break;
			}
			case 1: {
				color_leds(&type);
				break;
			}
			default: {};
		}
	}
	return 0;
}