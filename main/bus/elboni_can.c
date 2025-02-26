/*****************************************************************************
 * elboni_can.c
 *
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
// Include necessary driver headers
#include "driver/twai.h"   // TWAI (CAN) driver
#include "elboni_can.h"  // Include header file for CAN driver functions
#include "esp_err.h"
#include "esp_log.h"

#define TAG "TWAI" // Log tag for the CAN example (for logging)
#define CAN_DEBUG 1
#if CAN_DEBUG
	#define ELBONI_DBG_PRINT_CAN(...) ESP_LOGI(__VA_ARGS__)
#else
	#define ELBONI_DBG_PRINT_CAN(...)
#endif

//static twai_message_t message_save[8];
/**
 * @brief Initializes the CAN (TWAI) interface.
 *
 * This function installs and starts the TWAI driver with the specified
 * configurations. It also sets up alert conditions to monitor the CAN bus.
 *
 * @param t_config Timing configuration for CAN communication.
 * @param f_config Filter configuration for CAN messages.
 * @param g_config General configuration for the CAN interface.
 *
 * @return ESP_OK on success, ESP_FAIL otherwise.
 */
esp_err_t elboni_can_init(void)
{
	static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();                                                 // Set CAN bus speed to 500 kbps
    static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();                                              // Accept all incoming CAN messages
    static const twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NORMAL);

    // Install TWAI driver
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK)
    {
        ELBONI_DBG_PRINT_CAN(TAG, "Driver installed"); // Log driver installation success
    }
    else
    {
        ELBONI_DBG_PRINT_CAN(TAG, "Failed to install driver"); // Log driver installation failure
        return ESP_FAIL;
    }

    // Start TWAI driver
    if (twai_start() == ESP_OK)
    {
        ELBONI_DBG_PRINT_CAN(TAG, "Driver started"); // Log driver start success
    }
    else
    {
        ELBONI_DBG_PRINT_CAN(TAG, "Failed to start driver"); // Log driver start failure
        return ESP_FAIL;
    }

    // Configure alerts for specific CAN events
    uint32_t alerts_to_enable = TWAI_ALERT_TX_SUCCESS | TWAI_ALERT_TX_FAILED |
                                TWAI_ALERT_RX_DATA | TWAI_ALERT_RX_QUEUE_FULL |
                                TWAI_ALERT_ERR_PASS | TWAI_ALERT_BUS_ERROR;
    if (twai_reconfigure_alerts(alerts_to_enable, NULL) == ESP_OK)
    {
        ELBONI_DBG_PRINT_CAN(TAG, "CAN Alerts reconfigured"); // Log alert reconfiguration success
    }
    else
    {
        ELBONI_DBG_PRINT_CAN(TAG, "Failed to reconfigure alerts"); // Log alert reconfiguration failure
        return ESP_FAIL;
    }

#if CAN_DEBUG
	ELBONI_DBG_PRINT_CAN(TAG, "CAN stack size %d", CAN_TASK_STACK_SIZE);
	xTaskCreate(elboni_can_receive_task, "twai_echo_task", CAN_TASK_STACK_SIZE, NULL, 10, NULL);
#endif

    return ESP_OK;
}

/**
 * @brief Reads and handles alerts from the CAN interface.
 *
 * This function reads triggered alerts and provides detailed logs for each alert type.
 *
 * @return A 32-bit value representing the triggered alert type.
 */
uint32_t elboni_can_read_alerts()
{
    uint32_t alerts_triggered;                                           // Variable to store triggered alerts
    twai_read_alerts(&alerts_triggered, pdMS_TO_TICKS(POLLING_RATE_MS)); // Read alerts
    twai_status_info_t twaistatus;                                       // Variable to store TWAI status information
    twai_get_status_info(&twaistatus);                                   // Get TWAI status information

    if (alerts_triggered & TWAI_ALERT_ERR_PASS)
    {
        ELBONI_DBG_PRINT_CAN(TAG, "Alert: TWAI controller is in error passive state.");
        return TWAI_ALERT_ERR_PASS;
    }

    if (alerts_triggered & TWAI_ALERT_BUS_ERROR)
    {
        ELBONI_DBG_PRINT_CAN(TAG, "Alert: Bus error occurred.");
        ELBONI_DBG_PRINT_CAN(TAG, "Bus error count: %" PRIu32, twaistatus.bus_error_count);
        return TWAI_ALERT_BUS_ERROR;
    }

    if (alerts_triggered & TWAI_ALERT_TX_FAILED)
    {
        ELBONI_DBG_PRINT_CAN(TAG, "Alert: Transmission failed.");
        ELBONI_DBG_PRINT_CAN(TAG, "TX buffered: %" PRIu32, twaistatus.msgs_to_tx);
        return TWAI_ALERT_TX_FAILED;
    }

    if (alerts_triggered & TWAI_ALERT_TX_SUCCESS)
    {
        //ELBONI_DBG_PRINT_CAN(TAG, "Alert: Transmission successful.");
        return TWAI_ALERT_TX_SUCCESS;
    }

    if (alerts_triggered & TWAI_ALERT_RX_QUEUE_FULL)
    {
        ELBONI_DBG_PRINT_CAN(TAG, "Alert: RX queue full, frame lost.");
        return TWAI_ALERT_RX_QUEUE_FULL;
    }

    if (alerts_triggered & TWAI_ALERT_RX_DATA)
    {
        return TWAI_ALERT_RX_DATA;
    }

    return 0;
}

/**
 * @brief Transmits a CAN message.
 *
 * This function queues a CAN message for transmission and logs the result.
 *
 * @param message The CAN message to be transmitted.
 */
void elboni_can_write_Byte(twai_message_t message)
{
    if (twai_transmit(&message, portMAX_DELAY) == ESP_OK)
    {
        //ELBONI_DBG_PRINT_CAN(TAG,"Message queued for transmission"); // Log success
    }
    else
    {
        ELBONI_DBG_PRINT_CAN(TAG,"Failed to queue message for transmission"); // Log failure
    }
}

/**
 * @brief Receives a CAN message.
 *
 * This function reads and logs received messages from the CAN interface.
 *
 * @return The received CAN message.
 */
twai_message_t elboni_can_read_Byte(void)
{
    twai_message_t message; // Variable to hold received message
	
	memset(&message, 0 ,sizeof(twai_message_t));
    while (twai_receive(&message, 0) == ESP_OK)
    {
        if (message.extd)
        {
            ELBONI_DBG_PRINT_CAN(TAG, "Message is in Extended Format");
        }
        else
        {
            ELBONI_DBG_PRINT_CAN(TAG, "Message is in Standard Format");
        }
	#if CAN_DEBUG
        ELBONI_DBG_PRINT_CAN(TAG,"ID: %lx", message.identifier);
		ELBONI_DBG_PRINT_CAN(TAG,"Byte:");
        if (!message.rtr)
        {
            for (int i = 0; i < message.data_length_code; i++)
            {
                ELBONI_DBG_PRINT_CAN(TAG," %d = %02x,", i, message.data[i]);
            }
            printf("\n");
        }
	#endif
    }
    return message;
}

void elboni_can_receive_task(void* arg)
{
	twai_message_t message;
	uint32_t alerts_triggered;

    while (1) {
		alerts_triggered = elboni_can_read_alerts();  // Check for any triggered CAN bus alerts
        
        // If new CAN data is received, read and transmit it
        if (alerts_triggered & TWAI_ALERT_RX_DATA)
        {
            message = elboni_can_read_Byte();      // Read the received CAN message
#if CAN_DEBUG
            elboni_can_write_Byte(message);        // Re-transmit the received message
#endif
        }
		vTaskDelay(pdMS_TO_TICKS(200));
    }
	
}