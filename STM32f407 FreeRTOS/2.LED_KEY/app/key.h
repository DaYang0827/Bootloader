#ifndef __KEY_H__
#define __KEY_H__

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"

typedef struct key
{
    GPIO_TypeDef* port;
	uint16_t      pin;
	uint32_t     clock;
	BitAction   active_level;
}typedef_key;

extern typedef_key KEY0;
extern typedef_key KEY1;
extern typedef_key KEY2;

void key_init(typedef_key *key);
uint8_t key_is_pressed(typedef_key *key);

#endif

