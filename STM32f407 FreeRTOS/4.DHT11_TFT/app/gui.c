#include <stddef.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx.h"
#include "lvgl.h"
#include "src/drivers/display/ili9341/lv_ili9341.h"
#include "gui.h"

#define GUI_DRAW_BUF_LINES          20U
#define GUI_DRAW_BUF_SIZE_BYTES     (GUI_LCD_HOR_RES * GUI_DRAW_BUF_LINES * 2U)

#define GUI_LCD_SPI                 SPI2
#define GUI_LCD_SPI_CLK             RCC_APB1Periph_SPI2
#define GUI_LCD_SCK_PORT            GPIOB
#define GUI_LCD_SCK_PIN             GPIO_Pin_13
#define GUI_LCD_SCK_PIN_SOURCE      GPIO_PinSource13
#define GUI_LCD_MISO_PORT           GPIOC
#define GUI_LCD_MISO_PIN            GPIO_Pin_2
#define GUI_LCD_MISO_PIN_SOURCE     GPIO_PinSource2
#define GUI_LCD_MOSI_PORT           GPIOC
#define GUI_LCD_MOSI_PIN            GPIO_Pin_3
#define GUI_LCD_MOSI_PIN_SOURCE     GPIO_PinSource3
#define GUI_LCD_CTRL_PORT           GPIOE
#define GUI_LCD_CS_PIN              GPIO_Pin_2
#define GUI_LCD_RST_PIN             GPIO_Pin_3
#define GUI_LCD_DC_PIN              GPIO_Pin_4
#define GUI_LCD_BL_PIN              GPIO_Pin_5

static lv_display_t *ili9341_disp;
static volatile uint8_t gui_lvgl_ready;
static volatile uint8_t gui_led_state[GUI_LED_COUNT];
static volatile uint32_t gui_key_count[GUI_LED_COUNT];
static lv_obj_t *gui_led_label[GUI_LED_COUNT];
static lv_obj_t *gui_count_label;
static LV_ATTRIBUTE_MEM_ALIGN uint8_t gui_draw_buf[GUI_DRAW_BUF_SIZE_BYTES];

static void gui_lcd_cs_low(void)
{
    GPIO_ResetBits(GUI_LCD_CTRL_PORT, GUI_LCD_CS_PIN);
}

static void gui_lcd_cs_high(void)
{
    GPIO_SetBits(GUI_LCD_CTRL_PORT, GUI_LCD_CS_PIN);
}

static void gui_lcd_dc_cmd(void)
{
    GPIO_ResetBits(GUI_LCD_CTRL_PORT, GUI_LCD_DC_PIN);
}

static void gui_lcd_dc_data(void)
{
    GPIO_SetBits(GUI_LCD_CTRL_PORT, GUI_LCD_DC_PIN);
}

void gui_set_led_state(uint8_t index, uint8_t is_on)
{
    if(index >= GUI_LED_COUNT) {
        return;
    }

    gui_led_state[index] = is_on ? 1U : 0U;
    gui_key_count[index]++;
}

static void gui_lcd_spi_write_byte(uint8_t data)
{
    while(SPI_I2S_GetFlagStatus(GUI_LCD_SPI, SPI_I2S_FLAG_TXE) == RESET) {
    }

    SPI_I2S_SendData(GUI_LCD_SPI, data);

    while(SPI_I2S_GetFlagStatus(GUI_LCD_SPI, SPI_I2S_FLAG_RXNE) == RESET) {
    }

    (void)SPI_I2S_ReceiveData(GUI_LCD_SPI);
}

static void gui_lcd_spi_wait_idle(void)
{
    while(SPI_I2S_GetFlagStatus(GUI_LCD_SPI, SPI_I2S_FLAG_TXE) == RESET) {
    }

    while(SPI_I2S_GetFlagStatus(GUI_LCD_SPI, SPI_I2S_FLAG_BSY) == SET) {
    }
}

static void gui_lcd_write_bytes(const uint8_t *data, size_t size)
{
    while(size != 0U) {
        gui_lcd_spi_write_byte(*data++);
        size--;
    }
}

static void gui_lcd_write_rgb565(const uint8_t *data, size_t size)
{
    while(size >= 2U) {
        gui_lcd_spi_write_byte(data[1]);
        gui_lcd_spi_write_byte(data[0]);
        data += 2;
        size -= 2;
    }
}

void gui_lcd_bus_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    SPI_InitTypeDef SPI_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB |
                           RCC_AHB1Periph_GPIOC |
                           RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_APB1PeriphClockCmd(GUI_LCD_SPI_CLK, ENABLE);

    GPIO_PinAFConfig(GUI_LCD_SCK_PORT, GUI_LCD_SCK_PIN_SOURCE, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GUI_LCD_MISO_PORT, GUI_LCD_MISO_PIN_SOURCE, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GUI_LCD_MOSI_PORT, GUI_LCD_MOSI_PIN_SOURCE, GPIO_AF_SPI2);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
    GPIO_InitStruct.GPIO_Pin = GUI_LCD_SCK_PIN;
    GPIO_Init(GUI_LCD_SCK_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GUI_LCD_MISO_PIN | GUI_LCD_MOSI_PIN;
    GPIO_Init(GUI_LCD_MOSI_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
    GPIO_InitStruct.GPIO_Pin = GUI_LCD_CS_PIN | GUI_LCD_RST_PIN |
                               GUI_LCD_DC_PIN | GUI_LCD_BL_PIN;
    GPIO_Init(GUI_LCD_CTRL_PORT, &GPIO_InitStruct);

    gui_lcd_cs_high();
    gui_lcd_dc_data();
    GPIO_SetBits(GUI_LCD_CTRL_PORT, GUI_LCD_BL_PIN);

    SPI_I2S_DeInit(GUI_LCD_SPI);
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    SPI_Init(GUI_LCD_SPI, &SPI_InitStruct);
    SPI_NSSInternalSoftwareConfig(GUI_LCD_SPI, SPI_NSSInternalSoft_Set);
    SPI_Cmd(GUI_LCD_SPI, ENABLE);

    GPIO_ResetBits(GUI_LCD_CTRL_PORT, GUI_LCD_RST_PIN);
    vTaskDelay(pdMS_TO_TICKS(20));
    GPIO_SetBits(GUI_LCD_CTRL_PORT, GUI_LCD_RST_PIN);
    vTaskDelay(pdMS_TO_TICKS(120));
}

void gui_lcd_send_cmd(const uint8_t *cmd, size_t cmd_size,
                      const uint8_t *param, size_t param_size)
{
    gui_lcd_cs_low();

    gui_lcd_dc_cmd();
    gui_lcd_write_bytes(cmd, cmd_size);

    if((param != NULL) && (param_size != 0U)) {
        gui_lcd_dc_data();
        gui_lcd_write_bytes(param, param_size);
    }

    gui_lcd_spi_wait_idle();
    gui_lcd_cs_high();
}

void gui_lcd_send_color(const uint8_t *cmd, size_t cmd_size,
                        const uint8_t *param, size_t param_size)
{
    gui_lcd_cs_low();

    gui_lcd_dc_cmd();
    gui_lcd_write_bytes(cmd, cmd_size);

    if((param != NULL) && (param_size != 0U)) {
        gui_lcd_dc_data();
        gui_lcd_write_rgb565(param, param_size);
    }

    gui_lcd_spi_wait_idle();
    gui_lcd_cs_high();
}

static void gui_ili9341_send_cmd(lv_display_t *disp,
                                  const uint8_t *cmd, size_t cmd_size,
                                  const uint8_t *param, size_t param_size)
{
    LV_UNUSED(disp);
    gui_lcd_send_cmd(cmd, cmd_size, param, param_size);
}

static void gui_ili9341_send_color(lv_display_t *disp,
                                    const uint8_t *cmd, size_t cmd_size,
                                    uint8_t *param, size_t param_size)
{
    gui_lcd_send_color(cmd, cmd_size, param, param_size);
    lv_display_flush_ready(disp);
}

static void gui_lv_delay_ms(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

static void gui_update_led_screen(void)
{
    static const char * const led_name[GUI_LED_COUNT] = {
        "KEY0 -> LED0",
        "KEY1 -> LED1",
        "KEY2 -> LED2"
    };
    uint8_t i;

    for(i = 0U; i < GUI_LED_COUNT; i++) {
        uint8_t is_on = gui_led_state[i];
        lv_label_set_text_fmt(gui_led_label[i], "%s: %s", led_name[i], is_on ? "ON" : "OFF");
        lv_obj_set_style_text_color(gui_led_label[i],
                                    is_on ? lv_color_hex(0x1FD65A) : lv_color_hex(0x8D99A6),
                                    LV_PART_MAIN);
    }

    lv_label_set_text_fmt(gui_count_label, "Count  K0:%lu  K1:%lu  K2:%lu",
                          (unsigned long)gui_key_count[0],
                          (unsigned long)gui_key_count[1],
                          (unsigned long)gui_key_count[2]);
}

static void gui_create_screen(void)
{
    uint8_t i;
    lv_obj_t *scr = lv_screen_active();

    lv_obj_set_style_bg_color(scr, lv_color_hex(0x101820), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);

    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "KEY LED Monitor");
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 18);

    for(i = 0U; i < GUI_LED_COUNT; i++) {
        gui_led_label[i] = lv_label_create(scr);
        lv_obj_set_style_text_font(gui_led_label[i], &lv_font_montserrat_14, LV_PART_MAIN);
        lv_obj_align(gui_led_label[i], LV_ALIGN_TOP_LEFT, 24, 78 + (int32_t)i * 42);
    }

    gui_count_label = lv_label_create(scr);
    lv_obj_set_style_text_color(gui_count_label, lv_color_hex(0xFEEA00), LV_PART_MAIN);
    lv_obj_set_style_text_font(gui_count_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_align(gui_count_label, LV_ALIGN_BOTTOM_MID, 0, -24);

    gui_update_led_screen();
}

void gui_init(void)
{
    lv_init();
    lv_delay_set_cb(gui_lv_delay_ms);
    gui_lvgl_ready = 1U;
    gui_lcd_bus_init();

    ili9341_disp = lv_ili9341_create(GUI_LCD_HOR_RES, GUI_LCD_VER_RES,
                                     LV_LCD_FLAG_NONE,
                                     gui_ili9341_send_cmd,
                                     gui_ili9341_send_color);

    if(ili9341_disp == NULL) {
        return;
    }

    lv_display_set_color_format(ili9341_disp, LV_COLOR_FORMAT_RGB565);
    lv_lcd_generic_mipi_set_address_mode(ili9341_disp, true, true, true, false);
    lv_display_set_buffers(ili9341_disp, gui_draw_buf, NULL,
                           sizeof(gui_draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_default(ili9341_disp);

    gui_create_screen();
}

void gui_task(void *args)
{
    LV_UNUSED(args);

    gui_init();

    while(1) {
        gui_update_led_screen();
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void vApplicationTickHook(void)
{
    if(gui_lvgl_ready != 0U) {
        lv_tick_inc(1);
    }
}
