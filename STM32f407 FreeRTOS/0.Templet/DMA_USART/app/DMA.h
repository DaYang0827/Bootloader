#ifndef __DMA_H__
#define __DMA_H__

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "misc.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define DMA_TIMEOUT    1000000U

typedef struct
{
    DMA_Stream_TypeDef *DMA_Streamx;

    uint32_t DMA_CLK;

    uint32_t DMA_Channel;

    uint32_t PeripheralBaseAddr;

    uint32_t Direction;

    uint32_t PeripheralInc;

    uint32_t MemoryInc;

    uint32_t PeripheralDataSize;

    uint32_t MemoryDataSize;

    uint32_t Mode;

    uint32_t Priority;

    uint32_t FIFOMode;
    uint32_t FIFOThreshold;

    uint32_t MemoryBurst;
    uint32_t PeripheralBurst;

    IRQn_Type IRQ_Channel;

    uint8_t PreemptionPriority;
    uint8_t SubPriority;

    uint32_t ClearFlags;

    uint32_t TransferCompleteITFlag;
    uint32_t TransferErrorITFlag;

} typedef_dma;

void dma_init(typedef_dma * dma);
void dma_send(typedef_dma * dma, uint32_t data, uint8_t len);
uint32_t dma_recevie(typedef_dma * dma);

void dma_init(typedef_dma *dma);

uint8_t dma_start(typedef_dma *dma, uint32_t memory_address, uint16_t length);

void dma_stop(typedef_dma *dma);

uint16_t dma_get_remaining(typedef_dma *dma);

/* USART1 DMA封装函数 */
uint8_t usart1_dma_send(const uint8_t *data, uint16_t length);

uint8_t usart1_dma_receive(uint8_t *data, uint16_t length);

#endif
