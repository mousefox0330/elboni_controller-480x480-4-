/*****************************************************************************
 *  elboni_can.c 
 *
 ******************************************************************************/

#ifndef __CAN_H
#define __CAN_H

#include "driver/twai.h"   // TWAI (CAN) driver

// Define the GPIO pins used for CAN communication
//#define TX_GPIO_NUM GPIO_NUM_6 // Transmit GPIO number for CAN
//#define RX_GPIO_NUM GPIO_NUM_0 // Receive GPIO number for CAN
#define TX_GPIO_NUM CONFIG_CAN_TX // Transmit GPIO number for CAN
#define RX_GPIO_NUM CONFIG_CAN_RX // Receive GPIO number for CAN

// Define transmission and polling intervals in milliseconds
#define TRANSMIT_RATE_MS 1000 // Transmission interval for CAN messages
#define POLLING_RATE_MS 1000  // Polling interval for checking CAN status

#define CAN_TASK_STACK_SIZE (CONFIG_CAN_STACK_SIZE)
// Function declarations:

/**
 * @brief Initializes the CAN (TWAI) interface with the provided configurations.
 *
 * @param t_config Timing configuration for CAN communication.
 * @param f_config Filter configuration for CAN.
 * @param g_config General configuration for the CAN interface.
 *
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t elboni_can_init(void);

/**
 * @brief Reads and returns any alerts from the CAN interface.
 *
 * @return A 32-bit value representing the alerts.
 */
uint32_t elboni_can_read_alerts();

/**
 * @brief Sends a single byte of data over the CAN interface.
 *
 * @param message The CAN message to be transmitted.
 */
void elboni_can_write_Byte(twai_message_t message);

/**
 * @brief Reads a single byte of data from the CAN interface.
 *
 * @return A CAN message containing the received data.
 */
twai_message_t elboni_can_read_Byte();

void elboni_can_receive_task(void* arg);

#endif  // __CAN_H
