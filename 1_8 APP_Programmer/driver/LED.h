#ifndef __LED_H__
#define __LED_H__

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct 
{
    uint32_t RCC_AHB1Periph_led1;
    uint32_t RCC_AHB1Periph_led2;
    uint32_t RCC_AHB1Periph_led3;

    GPIO_TypeDef* GPIOx_led1;
    GPIO_TypeDef* GPIOx_led2;
    GPIO_TypeDef* GPIOx_led3;

    uint16_t led1_GPIO_Pin;
    uint16_t led2_GPIO_Pin;
    uint16_t led3_GPIO_Pin;

}LED_t;

typedef struct 
{
    uint32_t RCC_AHB1Periph_key1;
    uint32_t RCC_AHB1Periph_key2;
    uint32_t RCC_AHB1Periph_key3;

    GPIO_TypeDef* GPIOx_key1;
    GPIO_TypeDef* GPIOx_key2;
    GPIO_TypeDef* GPIOx_key3;

    uint16_t key1_GPIO_Pin;
    uint16_t key2_GPIO_Pin;
    uint16_t key3_GPIO_Pin;
}KEY_t;

typedef struct
{
    LED_t* led;
    KEY_t* key;
}KEY_LED_t;

typedef enum 
{
    Key1_ON,
    Key1_OFF,
    
    Key2_ON,
    Key2_OFF,
    
    Key3_ON,
    Key3_OFF,
}KEY_LED_Switch;

void led_init(LED_t* led);
void key_init(KEY_t* key);

void led1_on(void);
void led2_on(void);
void led3_on(void);
void led1_off(void);
void led2_off(void);
void led3_off(void);

void key_led(KEY_LED_t* keys_leds);

#endif
