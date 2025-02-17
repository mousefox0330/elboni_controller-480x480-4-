#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "elboni_rs485.h"

void app_main(void)
{
	elboni_RS485_init();
}