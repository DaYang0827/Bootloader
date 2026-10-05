#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "LED.h"

#define APP_BASE_ADDRESS     0x08100000



static void led_blink (void * args)
{
	LED_TypeDef * led = args;

	led_init(led);
	while (1)
	{
		led_on(led);
		vTaskDelay(pdMS_TO_TICKS(200));
		led_off(led);
		vTaskDelay(pdMS_TO_TICKS(200));
	}
}

int main(void)
{
	extern void JumpAPP(uint32_t base);
	JumpAPP(APP_BASE_ADDRESS);
	
	SystemCoreClockUpdate();
	xTaskCreate(led_blink, "led_blink", 256, (void*)&LED0, 1, NULL);
	xTaskCreate(led_blink, "led_blink", 256, (void*)&LED1, 1, NULL);
	xTaskCreate(led_blink, "led_blink", 256, (void*)&LED2, 1, NULL);

	vTaskStartScheduler();

	return 0;
}
