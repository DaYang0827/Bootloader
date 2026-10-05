#include "LED.h"

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "LED.h"

LED_t my_leds =
{
    .RCC_AHB1Periph_led1 = RCC_AHB1Periph_GPIOB,
    .RCC_AHB1Periph_led2 = RCC_AHB1Periph_GPIOB,
    .RCC_AHB1Periph_led3 = RCC_AHB1Periph_GPIOE,

    .GPIOx_led1 = GPIOB,
    .GPIOx_led2 = GPIOB,
    .GPIOx_led3 = GPIOE,

    .led1_GPIO_Pin = GPIO_Pin_0,
    .led2_GPIO_Pin = GPIO_Pin_1,
    .led3_GPIO_Pin = GPIO_Pin_9,
};

KEY_t my_keys =
{
    .RCC_AHB1Periph_key1 = RCC_AHB1Periph_GPIOA,
    .RCC_AHB1Periph_key2 = RCC_AHB1Periph_GPIOC,
    .RCC_AHB1Periph_key3 = RCC_AHB1Periph_GPIOC,

    .GPIOx_key1 = GPIOA,
    .GPIOx_key2 = GPIOC,
    .GPIOx_key3 = GPIOC,

    .key1_GPIO_Pin = GPIO_Pin_0,
    .key2_GPIO_Pin = GPIO_Pin_4,
    .key3_GPIO_Pin = GPIO_Pin_5,
};

KEY_LED_t my_key_led = 
{
    .key = &my_keys,
    .led = &my_leds,
};

void led_init(LED_t* led)
{
    RCC_AHB1PeriphClockCmd(led->RCC_AHB1Periph_led1, ENABLE);
    RCC_AHB1PeriphClockCmd(led->RCC_AHB1Periph_led2, ENABLE);
    RCC_AHB1PeriphClockCmd(led->RCC_AHB1Periph_led3, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = led->led1_GPIO_Pin | led->led2_GPIO_Pin;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(led->GPIOx_led1, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = led->led3_GPIO_Pin;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(led->GPIOx_led3, &GPIO_InitStruct);
}

void key_init(KEY_t* key)
{
    RCC_AHB1PeriphClockCmd(key->RCC_AHB1Periph_key1, ENABLE);
    RCC_AHB1PeriphClockCmd(key->RCC_AHB1Periph_key2, ENABLE);
    RCC_AHB1PeriphClockCmd(key->RCC_AHB1Periph_key3, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = key->key1_GPIO_Pin;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(key->GPIOx_key1, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = key->key2_GPIO_Pin | key->key3_GPIO_Pin;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(key->GPIOx_key2, &GPIO_InitStruct);
}

void led1_on(void)
{
    GPIO_WriteBit(my_leds.GPIOx_led1, my_leds.led1_GPIO_Pin, Bit_SET);
}

void led2_on(void)
{
    GPIO_WriteBit(my_leds.GPIOx_led2, my_leds.led2_GPIO_Pin, Bit_SET);
}

void led3_on(void)
{
    GPIO_WriteBit(my_leds.GPIOx_led3, my_leds.led3_GPIO_Pin, Bit_SET);
}

void led1_off(void)
{
    GPIO_WriteBit(my_leds.GPIOx_led1, my_leds.led1_GPIO_Pin, Bit_RESET);
}

void led2_off(void)
{
    GPIO_WriteBit(my_leds.GPIOx_led2, my_leds.led2_GPIO_Pin, Bit_RESET);
}
void led3_off(void)
{
   GPIO_WriteBit(my_leds.GPIOx_led3, my_leds.led3_GPIO_Pin, Bit_RESET);
}

void key_led(KEY_LED_t* keys_leds)
{
    if(GPIO_ReadInputDataBit(keys_leds->key->GPIOx_key1, keys_leds->key->key1_GPIO_Pin) == Bit_SET)
    {
        led1_on();
    }

    if(GPIO_ReadInputDataBit(keys_leds->key->GPIOx_key2, keys_leds->key->key2_GPIO_Pin) == Bit_SET)
    {
        led2_on();
    }

    if(GPIO_ReadInputDataBit(keys_leds->key->GPIOx_key3, keys_leds->key->key3_GPIO_Pin) == Bit_SET)
    {
        led3_on();
    }

    if(GPIO_ReadInputDataBit(keys_leds->key->GPIOx_key1, keys_leds->key->key1_GPIO_Pin) == Bit_RESET)
    {
        led1_off();
    }

    if(GPIO_ReadInputDataBit(keys_leds->key->GPIOx_key2, keys_leds->key->key2_GPIO_Pin) == Bit_RESET)
    {
        led2_off();
    }

    if(GPIO_ReadInputDataBit(keys_leds->key->GPIOx_key3, keys_leds->key->key3_GPIO_Pin) == Bit_RESET)
    {
        led3_off();
    }

}
