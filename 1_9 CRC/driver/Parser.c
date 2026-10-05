#include "stm32f4xx.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "Parser.h"
#include "RingBuffer.h"
#include "CMD.h"
#include "log.h"

extern RingBuffer_t rb;

static ParserState_t Current_State = WAIT_AA;
static Package_t package;

void protocol_process(void)
{
    uint8_t byte;

    static uint8_t data_index = 0;

    while(rb_read(&rb, &byte))
    {
       switch (Current_State)
       {
            case WAIT_AA:
            { 
                if(byte == 0xAA)
                {
                    Current_State = WAIT_55;
                    // log_info("Wait for 0x55");
                }
                else
                {
                    Current_State = WAIT_AA;
                    // log_error("wrong head");
                }
               break;
            }
            
            case WAIT_55:
            { 
                if(byte == 0x55)
                {
                    Current_State = WAIT_LEN;
                    // log_info("Wait for data length");
                }
                else if(byte == 0xAA)
                {
                    Current_State = WAIT_55;
                    // log_info("New data frame");
                }
                else
                {
                    Current_State = WAIT_AA;
                    // log_error("Woring head");
                }                
                break;
            }

            case WAIT_LEN:
            {
                package.len = byte;
                
                if(package.len <= Package_Data_Size)
                {
                    Current_State = WAIT_CMD;

                    // log_info("receive length success");
                }
                
                else if(package.len > Package_Data_Size)
                {
                    data_index = 0;
                    
                    Current_State = WAIT_AA;
                    log_error("The data length is too long! Drop this package!");
                }
                
                break;
            }

            case WAIT_CMD:
            {
                package.cmd = byte;

                // log_info("Input cmd success!");

                if(package.len == 0)
                {
                    package.sum = package.len + package.cmd;
                    Current_State = WAIT_SUM;
                }
                else
                {
                    Current_State = WAIT_DATA;
                }

                break;
            }

            case WAIT_DATA:
            {
                package.data[data_index++] = byte;
                
                if(data_index == package.len)
                {
                    Current_State = WAIT_SUM;

                    // log_info("Input data success!");

                    package.sum = 0;
                    package.sum = package.cmd + package.len;

                    for(int i = 0; i < package.len; i++)
                    {
                        package.sum += package.data[i];
                    }

                    data_index = 0;
                }

                else if (data_index < package.len)
                {
                    // log_info("Waiting for more data...");
                }
                
                break;
            }

            case WAIT_SUM:
            {
                if(byte == (uint8_t) package.sum)
                {
                    // log_info("Checksum success");

                    cmd_handle(&package);

                    Current_State = WAIT_AA;
                    data_index = 0;
                }
                
                else
                {
                    log_error("Checksum error!");
                    Current_State = WAIT_AA;

                    data_index = 0;
                }

                break;
            } 
        }
       
    }
}

