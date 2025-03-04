/*****************************************************************************
 *  elboni_iic.h 
 *
 ******************************************************************************/

#ifndef __I2C_H
#define __I2C_H

/**
 * I2C config
 **/
#define I2C_MASTER_SCL_0_IO           CONFIG_I2C_0_SCL      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_0_IO           CONFIG_I2C_0_SDA      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_SCL_1_IO           CONFIG_I2C_1_SCL      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_1_IO           CONFIG_I2C_1_SDA      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_FREQ_HZ          400000                  /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                       /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                       /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000
#define WRITE_BIT I2C_MASTER_WRITE  /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ    /*!< I2C master read */
#define ACK_CHECK_EN 0x1            /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0           /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                 /*!< I2C ack value */
#define NACK_VAL 0x1                /*!< I2C nack value */

esp_err_t elboni_i2c_0_master_init(void);
esp_err_t elboni_i2c_1_master_init(void);
esp_err_t DEV_I2C_Write_Byte(int I2C_num, uint8_t addr, uint8_t reg, uint8_t Value);
esp_err_t DEV_I2C_Write_nByte(int I2C_num, uint8_t addr, uint8_t *pData, uint32_t Len);
esp_err_t DEV_I2C_Read_Byte(int I2C_num, uint8_t addr, uint8_t reg, uint8_t *data);
esp_err_t DEV_I2C_Read_nByte(int I2C_num, uint8_t addr, uint8_t reg, uint8_t *pData, uint32_t Len);
#endif