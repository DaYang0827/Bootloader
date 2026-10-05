#ifndef __LED_H__
#define __LED_H__

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"

typedef struct 
{
    GPIO_TypeDef*   port;
    uint16_t         pin;
    uint32_t       clock;
}typedef_led;

extern typedef_led LED0;
extern typedef_led LED1;
extern typedef_led LED2;

void led_init(typedef_led* led);
void led_on(typedef_led* led);
void led_off(typedef_led* led);
void led_toggle(typedef_led* led);

#endif
