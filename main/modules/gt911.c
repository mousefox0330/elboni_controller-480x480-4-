/*
 * elboni_gt911 touch driver
 */

#include "esp_log.h"
#include "esp_heap_caps.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_io_additions.h"
#include "esp_lcd_touch_gt911.h"
#include "driver/gpio.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your touch spec ////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXAMPLE_LCD_H_RES 480
#define EXAMPLE_LCD_V_RES 480
#define EXAMPLE_PIN_NUM_TOUCH_RST -1          // -1 if not used
#define EXAMPLE_PIN_NUM_TOUCH_INT GPIO_NUM_16 // -1 if not used

#define TAG "GT911"

esp_lcd_touch_handle_t elboni_gt911_touch_init(esp_lcd_i2c_bus_handle_t i2c_port)
{
	esp_lcd_touch_handle_t tp_handle = NULL;

    esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    const esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_GT911_CONFIG();

    ESP_LOGI(TAG, "Initialize I2C panel IO");
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)i2c_port, &tp_io_config, &tp_io_handle));

    ESP_LOGI(TAG, "Initialize touch controller GT911 %d %d", EXAMPLE_PIN_NUM_TOUCH_INT, EXAMPLE_PIN_NUM_TOUCH_RST);
    const esp_lcd_touch_config_t tp_cfg = {
        .x_max = EXAMPLE_LCD_H_RES,
        .y_max = EXAMPLE_LCD_V_RES,
        .rst_gpio_num = EXAMPLE_PIN_NUM_TOUCH_RST,
        .int_gpio_num = EXAMPLE_PIN_NUM_TOUCH_INT,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = 0,
            .mirror_x = 0,
            .mirror_y = 0,
        },
    };
    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_gt911(tp_io_handle, &tp_cfg, &tp_handle));
	
	return tp_handle;
}