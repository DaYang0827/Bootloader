#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "key.h"
#include "USART.h"
#include "gui.h"

typedef struct
{
    typedef_key *key;
    typedef_led *led;
    uint8_t index;
    uint8_t led_is_on;
} KeyLedPair_TypeDef;

KeyLedPair_TypeDef pair0 = {&KEY0, &LED0, 0, 0};
KeyLedPair_TypeDef pair1 = {&KEY1, &LED1, 1, 0};
KeyLedPair_TypeDef pair2 = {&KEY2, &LED2, 2, 0};

static void key_led_task(void *args)
{
    KeyLedPair_TypeDef *pair = (KeyLedPair_TypeDef *)args;

    key_init(pair->key);
    led_init(pair->led);
    led_off(pair->led);
    pair->led_is_on = 0U;

    while (1)
    {
        if (key_is_pressed(pair->key))
        {
            led_toggle(pair->led);
            pair->led_is_on = pair->led_is_on ? 0U : 1U;
            gui_set_led_state(pair->index, pair->led_is_on);

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
    xTaskCreate(gui_task, "LVGL", GUI_LVGL_TASK_STACK_WORDS, NULL,
                GUI_LVGL_TASK_PRIORITY, NULL);

    vTaskStartScheduler();

    while (1)
    {
    }
}
