#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"

#include "CMD.h"
#include "USART.h"
#include "Parser.h"

extern USART_t usart1;
CMD_t Current_CMD = CMD_NULL;

void get_version(void)
{
    usart_send_string(&usart1,"The Bootloader version is 0.1.0\r\n");
}

void flash_unlock(void)
{
    if (FLASH->CR & FLASH_CR_LOCK)
    {
        FLASH->KEYR = 0x45670123;
        FLASH->KEYR = 0xCDEF89AB;
    }
}

void flash_lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}

void erase_app(void)
{
    flash_unlock();

    FLASH_EraseSector(FLASH_Sector_4, VoltageRange_3);
    FLASH_EraseSector(FLASH_Sector_5, VoltageRange_3);
    FLASH_EraseSector(FLASH_Sector_6, VoltageRange_3);

    flash_lock();
}

void cmd_handle(Package_t* package)
{
	Current_CMD = (CMD_t)(package->cmd);
    
    switch(Current_CMD)
    {
        case GET_VERSION:
        {
            get_version();
            break;
        } 
        
        case ERASE_APP:
       {
            usart_send_string(&usart1, "Erase APP executed\r\n");
            erase_app();
            break;
       } 

       case WRITE_APP:
       {
            usart_send_string(&usart1, "CMD 03 executed\r\n");
            break;
       }
       
       case JUMP_APP:
       {
            usart_send_string(&usart1, "JUMP APP command received\r\n");
            break;
       }

       default:
       {
            usart_send_string(&usart1, "Unknown CMD!\r\n");
            break;
       }
    }
}


