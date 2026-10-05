#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "DMA.h"

/**
 * USART1 TX
 *
 * Memory -> USART1->DR
 *
 * DMA2 Stream7 Channel4
 */
typedef_dma dma2_usart1_tx =
{
    .DMA_Streamx = DMA2_Stream7,

    .DMA_CLK = RCC_AHB1Periph_DMA2,

    .DMA_Channel = DMA_Channel_4,

    .PeripheralBaseAddr =
        (uint32_t)&USART1->DR,

    .Direction =
        DMA_DIR_MemoryToPeripheral,

    .PeripheralInc =
        DMA_PeripheralInc_Disable,

    .MemoryInc =
        DMA_MemoryInc_Enable,

    .PeripheralDataSize =
        DMA_PeripheralDataSize_Byte,

    .MemoryDataSize =
        DMA_MemoryDataSize_Byte,

    .Mode =
        DMA_Mode_Normal,

    .Priority =
        DMA_Priority_High,

    .FIFOMode =
        DMA_FIFOMode_Disable,

    .FIFOThreshold =
        DMA_FIFOThreshold_Full,

    .MemoryBurst =
        DMA_MemoryBurst_Single,

    .PeripheralBurst =
        DMA_PeripheralBurst_Single,

    .IRQ_Channel =
        DMA2_Stream7_IRQn,

    .PreemptionPriority = 1,
    .SubPriority = 1,

    .ClearFlags =
        DMA_FLAG_FEIF7 |
        DMA_FLAG_DMEIF7 |
        DMA_FLAG_TEIF7 |
        DMA_FLAG_HTIF7 |
        DMA_FLAG_TCIF7,

    .TransferCompleteITFlag =
        DMA_IT_TCIF7,

    .TransferErrorITFlag =
        DMA_IT_TEIF7
};


/**
 * USART1 RX
 *
 * USART1->DR -> Memory
 *
 * DMA2 Stream2 Channel4
 */
typedef_dma dma2_usart1_rx =
{
    .DMA_Streamx = DMA2_Stream2,

    .DMA_CLK = RCC_AHB1Periph_DMA2,

    .DMA_Channel = DMA_Channel_4,

    .PeripheralBaseAddr =
        (uint32_t)&USART1->DR,

    .Direction =
        DMA_DIR_PeripheralToMemory,

    .PeripheralInc =
        DMA_PeripheralInc_Disable,

    .MemoryInc =
        DMA_MemoryInc_Enable,

    .PeripheralDataSize =
        DMA_PeripheralDataSize_Byte,

    .MemoryDataSize =
        DMA_MemoryDataSize_Byte,

    .Mode =
        DMA_Mode_Normal,

    .Priority =
        DMA_Priority_High,

    .FIFOMode =
        DMA_FIFOMode_Disable,

    .FIFOThreshold =
        DMA_FIFOThreshold_Full,

    .MemoryBurst =
        DMA_MemoryBurst_Single,

    .PeripheralBurst =
        DMA_PeripheralBurst_Single,

    .IRQ_Channel =
        DMA2_Stream2_IRQn,

    .PreemptionPriority = 1,
    .SubPriority = 2,

    .ClearFlags =
        DMA_FLAG_FEIF2 |
        DMA_FLAG_DMEIF2 |
        DMA_FLAG_TEIF2 |
        DMA_FLAG_HTIF2 |
        DMA_FLAG_TCIF2,

    .TransferCompleteITFlag =
        DMA_IT_TCIF2,

    .TransferErrorITFlag =
        DMA_IT_TEIF2
};