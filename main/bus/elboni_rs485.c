/* elboni RS485

   elboni 4' 480x480 controller RS485 port control code.
   
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "elboni_rs485.h"

#define TAG "RS485"
#define RS485_DEBUG 1

#if RS485_DEBUG
	#define ELBONI_DBG_PRINT_RS485(...) ESP_LOGI(__VA_ARGS__)
#else
	#define ELBONI_DBG_PRINT_RS485(...)
#endif


void elboni_RS485_Recv_task(void *arg)
{
    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(RS485_PORT_NUM, data, (BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);
        // Write data back to the UART
#if RS485_DEBUG
        uart_write_bytes(RS485_PORT_NUM, (const char *) data, len);

        if (len) {
            data[len] = '\0';
            ELBONI_DBG_PRINT_RS485(TAG, "Recv str: %s %d", (char *) data, len);
        }
#endif
		vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void elboni_RS485_send_data(uint8_t * data, int len)
{
	uart_write_bytes(RS485_PORT_NUM, (const char *) data, len);
}

void elboni_RS485_UART_init()
{
	/* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = RS485_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.rx_flow_ctrl_thresh = 122,
        .source_clk = UART_SCLK_DEFAULT,
    };

#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    ESP_ERROR_CHECK(uart_driver_install(RS485_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(RS485_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(RS485_PORT_NUM, RS485_TXD, RS485_RXD, RS485_RTS, RS485_CTS));
	// Set RS485 half duplex mode
    ESP_ERROR_CHECK(uart_set_mode(RS485_PORT_NUM, UART_MODE_RS485_HALF_DUPLEX));
	    // Set read timeout of UART TOUT feature
    ESP_ERROR_CHECK(uart_set_rx_timeout(RS485_PORT_NUM, RS485_READ_TOUT));
	ELBONI_DBG_PRINT_RS485(TAG, "RS485 Driver initialize");
}

void elboni_RS485_init(void)
{
	elboni_RS485_UART_init();
#if RS485_DEBUG
	xTaskCreate(elboni_RS485_Recv_task, "uart_echo_task", RS485_TASK_STACK_SIZE, NULL, 10, NULL);
#endif
}

