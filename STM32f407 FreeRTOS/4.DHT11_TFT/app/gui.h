#ifndef GUI_H
#define GUI_H

#include <stddef.h>
#include <stdint.h>
#include "FreeRTOS.h"

#define GUI_LCD_HOR_RES             240U
#define GUI_LCD_VER_RES             320U
#define GUI_LVGL_TASK_STACK_WORDS   1024U
#define GUI_LVGL_TASK_PRIORITY      1U
#define GUI_LED_COUNT               3U

void gui_init(void);
void gui_task(void *args);
void gui_set_led_state(uint8_t index, uint8_t is_on);

void gui_lcd_bus_init(void);
void gui_lcd_send_cmd(const uint8_t *cmd, size_t cmd_size,
                      const uint8_t *param, size_t param_size);
void gui_lcd_send_color(const uint8_t *cmd, size_t cmd_size,
                        const uint8_t *param, size_t param_size);

#endif
