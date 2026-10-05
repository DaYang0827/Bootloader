#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"

#include "CMD.h"
#include "log.h"
#include "Parser.h"

static CMD_t Current_CMD = CMD_NULL;

static uint32_t app_write_add = APP_Start_ADD;

static bool app_verified = false;

uint32_t app_size = 0x0U;

typedef void (*pFunction)(void);

void get_version(void)
{
    log_info("The Bootloader version is 0.1.0");
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
	
	app_size = 0;

    app_verified = false;

    return true;
}

bool write_app(Package_t* package)
{
    if(app_write_add + package->len > APP_END_ADD)
    {
        log_error("The data is overflow!");
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
	
	app_size += package->len;

    app_verified = false;

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
    if((app_stack < 0x20000000U) || app_stack >= 0x20020000U)
    {
        return false;
    }

    if((app_reset_handler & 0x1U) == 0U)
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

    pFunction app_entry = (pFunction)app_reset_handler;

    __set_MSP(app_stack);

    app_entry();

    return true;
}

uint32_t app_crc(void)
{
    uint32_t add = APP_Start_ADD;
    uint32_t  count = app_size / 4;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);

    CRC_ResetDR();

    for(uint32_t i = 0; i < count; i++)
    {
        uint32_t data = *(volatile uint32_t *)add;

        CRC_CalcCRC(data);
            
        add += 4;
    }

    if(app_size % 4 != 0)
    {
        uint8_t remain = app_size % 4;
        uint32_t last_data = 0xFFFFFFFFU;
        
        for(uint8_t i = 0; i < remain; i++)
        {
            uint32_t byte = *(volatile uint8_t *)(add + i);

        /* 清掉对应的 8 bit */
            last_data &= ~(0xFFU << (8U * i));

        /* 把实际数据放进去 */
            last_data |= (byte << (8U * i));
        }

        CRC_CalcCRC(last_data);
    }

    uint32_t crc = CRC_GetCRC();

    return crc;
}

bool verify_crc(uint32_t expected_crc)
{
    uint32_t actual_crc = app_crc();

    return actual_crc == expected_crc;
}

void cmd_handle(Package_t* package)
{
	Current_CMD = (CMD_t)(package->cmd);
    
    switch(Current_CMD)
    {
        case GET_VERSION:
        {
            log_info("The command is to get version");
            get_version();
            break;
        } 
        
        case ERASE_APP:
       {
            if(erase_app())    
            {
                log_info("The command is to erase flash");
                log_info("Erase APP executed");
            }

            break;
       } 

       case WRITE_APP:
       {
            if(write_app(package))
            {
                log_info("Write data in APP Address success!");
            }
            else
            {
                log_error("Write data in APP Address False!");
            }

            break;
       }

       case VERIFY_APP:
       {
            if(package->len != 4U)
            {
                log_error("CRC length invalid!");

                break;
            }

            uint32_t expected_crc =
                ((uint32_t)package->data[0])
                | ((uint32_t)package->data[1] << 8)
                | ((uint32_t)package->data[2] << 16)
                | ((uint32_t)package->data[3] << 24);

            log_info("Verify APP CRC...");

            if(verify_crc(expected_crc))
            {
                app_verified = true;
                log_info("Firmware CRC success!");
            }
            else
            {
                app_verified = false;
                log_error("Firmware CRC failed!");
            }

            break;
       }
       
       case JUMP_APP:
       {
            if(!app_verified)
            {
                log_error("APP has not passed CRC verification!");
                break;
            }

            log_info("Jump to APP success");

            if(!jump_app())
            {
                log_error("APP invalid!");
            }

             break;
       }

       default:
       {
            log_error("Unknown CMD!");
            
            break;
       }
    }
}




