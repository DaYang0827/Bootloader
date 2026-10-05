#ifndef __PARSER_H__
#define __PARSER_H__

#include "stm32f4xx.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define PackageDataSize 32

typedef enum
{
    WAIT_AA,
    WAIT_55,
    WAIT_Len,
    WAIT_CMD,
    WAIT_DATA,
    WAIT_SUM,
} ParserState_t;

typedef struct
{
    uint8_t len;
    uint8_t cmd;
    uint8_t data[PackageDataSize];

    uint16_t sum;
}Package_t;

void protocol_process(void);

#endif
