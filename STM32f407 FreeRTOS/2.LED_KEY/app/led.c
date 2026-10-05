#include <stdint.h>
#include <stdbool.h>
#include "led.h"
#include "stm32f4xx.h"

typedef_led LED0 ={GPIOB, GPIO_Pin_0, RCC_AHB1Periph_GPIOB};
typedef_led LED1 ={GPIOB, GPIO_Pin_1, RCC_AHB1Periph_GPIOB};
typedef_led LED2 ={GPIOE, GPIO_Pin_9, RCC_AHB1Periph_GPIOE};

void led_init(typedef_led* led)
{
    RCC_AHB1PeriphClockCmd(led->clock, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = led->pin;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Medium_Speed;
    GPIO_Init(led->port,&GPIO_InitStruct);
}

void led_on(typedef_led* led)
{
    GPIO_WriteBit(led->port, led->pin, Bit_SET);
}

void led_off(typedef_led* led)
{
    GPIO_WriteBit(led->port, led->pin, Bit_RESET);
}

void led_toggle(typedef_led* led)
{
	GPIO_ToggleBits(led->port, led->pin);
}
