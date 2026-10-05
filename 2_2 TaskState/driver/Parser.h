#ifndef __PARSER_H__
#define __PARSER_H__

#include "stm32f4xx.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define Package_Data_Size 32U

typedef enum
{
    WAIT_AA,
    WAIT_55,
    WAIT_LEN,
    WAIT_CMD,
    WAIT_DATA,
    WAIT_SUM,
} ParserState_t;

typedef struct
{
    uint8_t len;
    uint8_t cmd;
    uint8_t data[Package_Data_Size];
    uint8_t sum;
}Package_t;

void protocol_process(void);

#endif
