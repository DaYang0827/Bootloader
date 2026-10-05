#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "key.h"

typedef_key KEY0 = {GPIOA, GPIO_Pin_0, RCC_AHB1Periph_GPIOA, Bit_SET};
typedef_key KEY1 = {GPIOC, GPIO_Pin_4, RCC_AHB1Periph_GPIOC, Bit_SET};
typedef_key KEY2 = {GPIOC, GPIO_Pin_5, RCC_AHB1Periph_GPIOC, Bit_SET};


void key_init(typedef_key* key)
{
    RCC_AHB1PeriphClockCmd(key->clock,ENABLE);
	
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = key->pin;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Medium_Speed;
    GPIO_Init(key->port, &GPIO_InitStruct);
}

uint8_t key_is_pressed(typedef_key *key)
{
    BitAction level;

    level = (BitAction)GPIO_ReadInputDataBit(key->port, key->pin);

    if (level == key->active_level)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
