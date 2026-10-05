#include "stm32f4xx.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "USART.h"
#include "DMA.h"
#include "RingBuffer.h"
#include "Parser.h"

extern USART_t usart1;
extern RingBuffer_t rb;
extern DMA_t dma1;

int main(void)
{
	rb_init(&rb);
	dma_init(&dma1);
	usart_init(&usart1);

	usart_send_string(&usart1, "Prepare for packet!\r\n");

	while(1)
	{
		protocol_process();
	}
}
