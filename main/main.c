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

#define MAIN_RS485_DEBUG 0
#define MAIN_CAN_DEBUG   0
#define MAIN_SD_DEBUG    1

#define EX_SDCARD_POWER     CONFIG_SDMMC_EXTERN_POWER_PIN

#define TAG "MAIN"

#define BSP_IO_EXPANDER_I2C_ADDRESS (ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000)
static esp_io_expander_handle_t io_expander = NULL; // IO expander tca9554 handle

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
	esp_io_expander_set_dir(io_expander, EX_SDCARD_POWER, IO_EXPANDER_OUTPUT);
    esp_io_expander_set_level(io_expander, EX_SDCARD_POWER, true);
	ESP_LOGI(TAG, "Enable extern power contorl: set tca9554 pin 7 Output&High");
    vTaskDelay(pdMS_TO_TICKS(200));
	
	elboni_sdcard_vfat_init();
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