#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"

#include "CMD.h"
#include "USART.h"
#include "Parser.h"

extern USART_t usart1;
CMD_t Current_CMD = CMD_NULL;

static uint32_t app_write_add = APP_Start_ADD;

typedef void (*pFunction)(void);

pFunction app_entry;

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

bool erase_app(void)
{
    flash_unlock();

    FLASH_Status state = FLASH_EraseSector(FLASH_Sector_4, VoltageRange_3);
    if(state != FLASH_COMPLETE)
    {
        flash_lock();
        return false;
    }

    state = FLASH_EraseSector(FLASH_Sector_5, VoltageRange_3);
    if(state != FLASH_COMPLETE)
    {
        flash_lock();
        return false;
    }

    state = FLASH_EraseSector(FLASH_Sector_6, VoltageRange_3);
    if(state != FLASH_COMPLETE)
    {
        flash_lock();
        return false;
    }

    flash_lock();

    app_write_add = APP_Start_ADD;

    return true;
}

bool write_app(Package_t* package)
{
    if(app_write_add + package->len > APP_END_ADD)
    {
        usart_send_string(&usart1, "The data is overflow!\r\n");
        return false;
    }

    flash_unlock();

    for(uint8_t i = 0 ; i < package->len; i++)
    {
        if(FLASH_ProgramByte(app_write_add,package->data[i]) == FLASH_COMPLETE)
        {
            app_write_add++;
        }

        else
        {
            flash_lock();
            return false;
        }
    } 

    flash_lock();

    return true;
}

bool jump_app(void)
{
    uint32_t app_stack;
    uint32_t app_reset_handler;

    app_stack = *(uint32_t *)APP_Start_ADD;
    app_reset_handler = *(uint32_t *)(APP_Start_ADD + 4U);

    //chect the stack of app is empty and the vector is empty
    if((app_stack == 0xFFFFFFFFU) || (app_reset_handler == 0xFFFFFFFFU))
    {
        return false;
    }
    
    //check the stack of app is in the right SRAM
    if((app_stack < 0x20000000U) && app_stack > 0x20020000U)
    {
        return false;
    }

    //check the vector is in the app flash
    if(app_reset_handler < APP_Start_ADD ||
       app_reset_handler >= APP_END_ADD)
    {
        return false;
    }

    __disable_irq();

    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    SCB->VTOR = APP_Start_ADD;

    app_entry = (pFunction)app_reset_handler;

    __set_MSP(app_stack);

    app_entry();

    return true;
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
            if(erase_app())    
            {
                usart_send_string(&usart1, "Erase APP executed\r\n");
            }
            break;
       } 

       case WRITE_APP:
       {
            if(write_app(package))
            {
                usart_send_string(&usart1, "Write data in APP Address success!\r\n");
            }
            else
            {
                usart_send_string(&usart1, "Write data in APP Address False!\r\n");
            }
            break;
       }
       
       case JUMP_APP:
       {
            usart_send_string(&usart1, "Jump to APP...\r\n");

            if(!jump_app())
            {
                usart_send_string(&usart1, "APP invalid!\r\n");
            }

             break;
       }

       default:
       {
            usart_send_string(&usart1, "Unknown CMD!\r\n");
            
            break;
       }
    }
}




