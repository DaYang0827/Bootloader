#include "stm32f4xx.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "USART.h"
#include "RingBuffer.h"
#include "FreeRTOS.h"
#include "task.h"

extern USART_t usart1;
extern RingBuffer_t rb;

 
TaskHandle_t task1_handle;

void send_task1(void *pvParameters)
{
    while(1)
    {
        usart_send_string(&usart1,"Task1\r\n");
        //vTaskDelay(pdMS_TO_TICKS(1000));

        taskYIELD();
    }
}

void send_task2(void *pvParameters)
{
    while(1)
    {
        usart_send_string(&usart1,"Task2\r\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vApplicationStackOverflowHook(TaskHandle_t xTask,
                                   char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;

    taskDISABLE_INTERRUPTS();

    while(1)
    {
    }
}

void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();

    while(1)
    {
    }
}

void vAssertCalled(const char *file, int line)
{
    (void)file;
    (void)line;

    taskDISABLE_INTERRUPTS();

    while(1)
    {
    }
}

int main(void)
{
    rb_init(&rb);

	usart_init(&usart1);

    xTaskCreate(send_task1,"TASK1", 128, NULL, 1, NULL);
    xTaskCreate(send_task2,"TASK2", 128, NULL, 1, NULL);

    vTaskStartScheduler(); 

    while(1)
    {
    }
}

