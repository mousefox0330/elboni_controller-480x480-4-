#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "elboni_rs485.h"
#include "driver/twai.h"   // TWAI (CAN) driver
#include "elboni_can.h"

#define MAIN_RS485_DEBUG 1
#define MAIN_CAN_DEBUG   1

static const char *TAG = "MAIN";

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
	elboni_can_init();
	
	while (1) {
#if MAIN_RS485_DEBUG
        elboni_RS485_send_data((uint8_t *)test, strlen(test));
		ESP_LOGI(TAG, "Send uart: %s %d", (char *) test, strlen(test));
#endif
#if MAIN_RS485_DEBUG
		elboni_can_write_Byte(message);
		ESP_LOGI(TAG, "Send can: id:%x %x %x", message.identifier, *(int *)message.data, *(((int *)message.data)+1));
#endif
		vTaskDelay(pdMS_TO_TICKS(10000));
    }
}