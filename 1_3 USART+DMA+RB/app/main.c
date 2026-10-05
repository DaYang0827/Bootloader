#include "stm32f4xx.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "USART.h"
#include "RingBuffer.h"
#include "DMA.h"
#include "RingBuffer.h"

extern USART_t usart1;
extern DMA_t dma1;
extern RingBuffer_t rb;

int main(void)
{
	usart_init(&usart1);
	dma_init(&dma1);
	rb_init(&rb);
    
    uint8_t data = 0;

    while(1)
    {
        if(rb_read(&rb, &data))
        {
          usart_sendbyte(&usart1, data);
        }
    }
}

