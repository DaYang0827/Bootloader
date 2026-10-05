#ifndef __CMD_H__
#define __CMD_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"

#include "Parser.h"

#define BootLoaderADD       0x08000000U
#define APP_Start_ADD       0x08010000U
#define APP_END_ADD  		0x08060000U

typedef enum
{
    CMD_NULL,
	GET_VERSION = 0x01,
    ERASE_APP = 0x02,
    WRITE_APP = 0x03,
    JUMP_APP = 0x04,
}CMD_t;

void cmd_handle(Package_t* package);

void get_version(void);

void flash_unlock(void);
void flash_lock(void);
bool erase_app(void);

bool write_app(Package_t* package);
bool jump_app(void);

#endif
