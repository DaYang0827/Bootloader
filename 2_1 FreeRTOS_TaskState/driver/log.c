#include "stm32f4xx.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "USART.h"

extern USART_t usart1;

void log_info(char *msg)
{
    usart_send_string(&usart1, "[INFO] ");
    usart_send_string(&usart1, msg);
    usart_send_string(&usart1, "\r\n");
}

void log_error(char *msg)
{
    usart_send_string(&usart1, "[ERROR] ");
    usart_send_string(&usart1, msg);
    usart_send_string(&usart1, "\r\n");
}
