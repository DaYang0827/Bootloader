#include "stm32f4xx.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "Parser.h"
#include "RingBuffer.h"
#include "USART.h"
#include "CMD.h"

extern RingBuffer_t rb;
extern USART_t usart1;

ParserState_t Current_State = WAIT_AA;
Package_t package;

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
                    usart_send_string(&usart1, "Find 0xAA, waiting for 0x55...\r\n");
                }
                else
                {
                    Current_State = WAIT_AA;
                    usart_send_string(&usart1, "Need a package which head is 0xAA55\r\n");
                }
               break;
            }
            
            case WAIT_55:
            { 
                if(byte == 0x55)
                {
                    Current_State = WAIT_Len;
                    usart_send_string(&usart1, "Find head success!\r\n");
                }
                else if(byte == 0xAA)
                {
                    Current_State = WAIT_55;
                    usart_send_string(&usart1, "Double \r\n");
                }
                else
                {
                    Current_State = WAIT_AA;
                    usart_send_string(&usart1, "Find 0xAA, waiting for 0x55...\r\n");
                }                
                break;
            }

            case WAIT_Len:
            {
                package.len = byte;
                
                if(package.len <= PackageDataSize)
                {
                    Current_State = WAIT_CMD;

                    usart_send_string(&usart1, "Input length successful, the len is\r\n");
                    usart_sendbyte(&usart1, package.len);  
                }
                
                else if(package.len > PackageDataSize)
                {
                    data_index = 0;
                    
                    Current_State = WAIT_AA;
                    usart_send_string(&usart1, "The data length is too long! Drop this package!");
                }
                
                break;
            }

            case WAIT_CMD:
            {
                package.cmd = byte;

                if(package.len == 0)
                {
                    package.sum = package.len + package.cmd;
                    Current_State = WAIT_SUM;

                    usart_send_string(&usart1, "Input cmd success!\r\n");
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

                    usart_send_string(&usart1, "Input data success!\r\n");
                    usart_send_string(&usart1, "The sum of this package is \r\n");

                    package.sum = 0;
                    package.sum = package.cmd + package.len;

                    for(int i = 0; i < package.len; i++)
                    {
                        package.sum += package.data[i];
                    }

                    usart_sendbyte(&usart1, package.sum);

                    data_index = 0;
                }

                else if (data_index < package.len)
                {
                    usart_send_string(&usart1, "Waiting for more data...\r\n");
                }
                
                break;
            }

            case WAIT_SUM:
            {
                if(byte == package.sum)
                {
                    usart_send_string(&usart1, "Checksum success\r\n");
                    Current_State = WAIT_AA;

                    data_index = 0;

                    cmd_handle(&package);
                }
                
                else
                {
                    usart_send_string(&usart1, "Checksum error!\r\n");
                    Current_State = WAIT_AA;
                }

                break;
            }      
        }
       
    }
}

