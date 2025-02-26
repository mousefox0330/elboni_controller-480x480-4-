/*****************************************************************************
 *  elboni_can.h 
 *
 ******************************************************************************/

#ifndef __RS485_H
#define __RS485_H


/**
 * This is an example which echos any data it receives on configured UART back to the sender,
 * with hardware flow control turned off. It does not use UART driver event queue.
 *
 * - Port: configured UART
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: off
 * - Pin assignment: see defines below (See Kconfig)
 */

#define RS485_TXD (CONFIG_RS485_TXD)
#define RS485_RXD (CONFIG_RS485_RXD)
#define RS485_RTS (UART_PIN_NO_CHANGE)
#define RS485_CTS (UART_PIN_NO_CHANGE)

#define RS485_PORT_NUM      (CONFIG_RS485_PORT_NUM)
#define RS485_BAUD_RATE     (CONFIG_RS485_BAUD_RATE)
#define RS485_TASK_STACK_SIZE    (CONFIG_RS485_STACK_SIZE)
// Timeout threshold for UART = number of symbols (~10 tics) with unchanged state on receive pin
#define RS485_READ_TOUT          (3) // 3.5T * 8 = 28 ticks, TOUT=3 -> ~24..33 ticks
#define BUF_SIZE (1024)


void elboni_RS485_send_data(uint8_t * data, int len);
void elboni_RS485_init(void);
#endif