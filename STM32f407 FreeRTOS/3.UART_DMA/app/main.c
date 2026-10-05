#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "key.h"
#include "USART.h"
#include "DMA.h"

typedef struct
{
    typedef_key *key;
    typedef_led *led;
} KeyLedPair_TypeDef;

KeyLedPair_TypeDef pair0 = {&KEY0, &LED1};
KeyLedPair_TypeDef pair1 = {&KEY1, &LED2};
KeyLedPair_TypeDef pair2 = {&KEY2, &LED0};

static void key_led_task(void *args)
{
    KeyLedPair_TypeDef *pair = (KeyLedPair_TypeDef *)args;

    key_init(pair->key);
    led_init(pair->led);

    while (1)
    {
        if (key_is_pressed(pair->key))
        {
            led_toggle(pair->led);

            while (key_is_pressed(pair->key))
            {
                vTaskDelay(pdMS_TO_TICKS(10));
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

int main(void)
{
	xTaskCreate(key_led_task, "KEY0_LED0", 256, (void *)&pair0, 1, NULL);
	xTaskCreate(key_led_task, "KEY1_LED1", 256, (void *)&pair1, 1, NULL);
	xTaskCreate(key_led_task, "KEY2_LED2", 256, (void *)&pair2, 1, NULL);

	vTaskStartScheduler();
}
