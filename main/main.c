#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "sdkconfig.h"
#include "elboni_rs485.h"
#include "driver/twai.h"   // TWAI (CAN) driver
#include "elboni_can.h"
#include "elboni_iic.h"
#include "elboni_rtc.h"
#include "esp_io_expander_tca9554.h"
#include "elboni_sd.h"
#include "modules.h"
#include "app_weather.h"
#include "wifi.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_io_additions.h"
#include "esp_lcd_touch_gt911.h"
#include "esp_lcd_st7701.h"

#define MAIN_RS485_DEBUG 0
#define MAIN_CAN_DEBUG   0
#define MAIN_SD_DEBUG    0

#define EX_SDCARD_POWER     CONFIG_SDMMC_EXTERN_POWER_PIN
#define EX_PANDER_PIN_NUM_5 5 
#define EX_PANDER_PIN_NUM_0 0
#define EX_PANDER_PIN_NUM_2 2
#define EX_PIN_NUM_TOUCH_INT (GPIO_NUM_16) // -1 if not used

#define TAG "MAIN"

#define BSP_IO_EXPANDER_I2C_ADDRESS (ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000)
static esp_io_expander_handle_t io_expander = NULL; // IO expander tca9554 handle
static esp_lcd_touch_handle_t tp_handle = NULL;
static esp_lcd_panel_handle_t lcd_handle = NULL;



void app_main(void)
{
#if MAIN_RS485_DEBUG
	static char test[8]="012345";
#endif

#if MAIN_RS485_DEBUG
	twai_message_t message;
    message.identifier = 0x123;     // CAN ID
    message.extd = 0;               // Standard
    message.rtr = 0;                // type
    message.data_length_code = 8;   // data[8]
    message.data[0] = 0x11;
    message.data[1] = 0x22;
    message.data[2] = 0x33;
    message.data[3] = 0x44;
    message.data[4] = 0x55;
    message.data[5] = 0x66;
    message.data[6] = 0x77;
    message.data[7] = 0x88;
#endif
	
	elboni_RS485_init();
	ESP_LOGI(TAG, "RS485 initialize");
	elboni_can_init();
	ESP_LOGI(TAG, "CAN initialize");
	elboni_i2c_0_master_init();
	ESP_LOGI(TAG, "I2C 0 initialize");
	elboni_rtc_init(I2C_NUM_0);
	ESP_LOGI(TAG, "RTC initialize");

	esp_io_expander_new_i2c_tca9554(I2C_NUM_0, BSP_IO_EXPANDER_I2C_ADDRESS, &io_expander);
	//Enable SD interface power
	esp_io_expander_set_dir(io_expander, IO_EXPANDER_PIN_NUM_7, IO_EXPANDER_OUTPUT);
    esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_7, true);
	ESP_LOGI(TAG, "Enable extern power contorl: set tca9554 pin 7 Output&High");
    vTaskDelay(pdMS_TO_TICKS(200));
	
	elboni_sdcard_vfat_init();
	elboni_nvram_init();
	elboni_spiffs_mount();
	app_weather_start();
    app_network_start();
	//elboni_ble_gatt_client();
	
	/* set LCD power reset */
	esp_io_expander_set_dir(io_expander, IO_EXPANDER_PIN_NUM_5 | IO_EXPANDER_PIN_NUM_0 | IO_EXPANDER_PIN_NUM_2, IO_EXPANDER_OUTPUT);
    esp_rom_gpio_pad_select_gpio(EX_PIN_NUM_TOUCH_INT);
    gpio_set_direction(EX_PIN_NUM_TOUCH_INT, GPIO_MODE_OUTPUT);
    gpio_set_level(EX_PIN_NUM_TOUCH_INT, 0);
    esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_5, 1);
    esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_0, 0);
    esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_2, 0);
    vTaskDelay(pdMS_TO_TICKS(200));
    esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_5, 0);
    esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_0, 1);
    esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_2, 1);
	
	
	lcd_handle = elboni_st7701_lcd_init();
	tp_handle = elboni_gt911_touch_init(I2C_NUM_0);
	
#if MAIN_SD_DEBUG
	elboni_vfat_test();
#endif

	while (1) {
#if MAIN_RS485_DEBUG
        elboni_RS485_send_data((uint8_t *)test, strlen(test));
		ESP_LOGI(TAG, "Send uart: %s %d", (char *) test, strlen(test));
#endif
#if MAIN_CAN_DEBUG
		elboni_can_write_Byte(message);
		ESP_LOGI(TAG, "Send can: id:%x %x %x", message.identifier, *(int *)message.data, *(((int *)message.data)+1));
#endif
		vTaskDelay(pdMS_TO_TICKS(10000));
    }
}