#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "elboni_rs485.h"

#define MAIN_RS485_DEBUG 1

static const char *TAG = "MAIN";

void app_main(void)
{
	static char test[8]="012345";
	
	elboni_RS485_init();
	while (1) {
#if MAIN_RS485_DEBUG
        elboni_RS485_send_data((uint8_t *)test, strlen(test));
		ESP_LOGI(TAG, "Send str: %s %d", (char *) test, strlen(test));
#endif
		vTaskDelay(pdMS_TO_TICKS(10000));
    }
}