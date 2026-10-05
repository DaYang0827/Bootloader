#include "stm32f4xx.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "LED.h"

extern LED_t my_leds;
extern KEY_t my_keys;
extern KEY_LED_t my_key_led;

int main(void)
{
	led_init(&my_leds);
	key_init(&my_keys);

	while(1)
	{
		key_led(&my_key_led);	
	}
}
