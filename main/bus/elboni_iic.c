/* elboni RS485

   elboni 4' 480x480 controller IIC port control code.
   
*/

#include <stdio.h>
#include "esp_rom_sys.h"
#include "esp_log.h"
#include "argtable3/argtable3.h"
#include "driver/i2c.h"
#include "esp_console.h"
#include "driver/gpio.h"
#include "elboni_iic.h"

#define TAG "IIC"
#define IIC_DEBUG 1

#if RS485_DEBUG
	#define ELBONI_DBG_PRINT_IIC(...) ESP_LOGI(__VA_ARGS__)
#else
	#define ELBONI_DBG_PRINT_IIC(...)
#endif
/**
 * I2C
 **/
esp_err_t elboni_i2c_0_master_init(void)
{
	esp_err_t ret = ESP_OK;
    int i2c_master_port = I2C_RTC_NUM_0;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_0_IO,
        .scl_io_num = I2C_MASTER_SCL_0_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    ret = i2c_param_config(i2c_master_port, &conf);
	if(ret != ESP_OK) {
		ELBONI_DBG_PRINT_IIC(TAG, "Config i2c 0 param error %d", ret);
		return ret;
	}
    ret = i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
	if(ret != ESP_OK) {
		ELBONI_DBG_PRINT_IIC(TAG, "Install i2c 0 param error %d", ret);
		return ret;
	}
	
	return ESP_OK;
}

esp_err_t elboni_i2c_1_master_init(void)
{
	esp_err_t ret = ESP_OK;
    int i2c_master_port = I2C_RTC_NUM_1;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_1_IO,
        .scl_io_num = I2C_MASTER_SCL_1_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    ret = i2c_param_config(i2c_master_port, &conf);
	if(ret != ESP_OK) {
		ELBONI_DBG_PRINT_IIC(TAG, "Config i2c 1 param error %d", ret);
		return ret;
	}
	
    ret = i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
	if(ret != ESP_OK) {
		ELBONI_DBG_PRINT_IIC(TAG, "Install i2c 1 param error %d", ret);
		return ret;
	}
		
	return ESP_OK;
}

esp_err_t  DEV_I2C_Write_Byte(int I2C_num, uint8_t addr, uint8_t reg, uint8_t Value)
{
	esp_err_t ret = ESP_OK;
    uint8_t write_buf[2] = {reg, Value};
    ret = i2c_master_write_to_device(I2C_num, addr, write_buf, 2, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
	if(ret != ESP_OK) {
		ELBONI_DBG_PRINT_IIC(TAG, "I2c port %d addr:%x write byte error %d", I2C_num, addr, ret);
		return ret;
	}

	return ESP_OK;
}

esp_err_t  DEV_I2C_Write_nByte(int I2C_num, uint8_t addr, uint8_t *pData, uint32_t Len)
{
	esp_err_t ret = ESP_OK;
    ret = i2c_master_write_to_device(I2C_num, addr, pData, Len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
	if(ret != ESP_OK) {
		ELBONI_DBG_PRINT_IIC(TAG, "I2c port %d addr:%x write nbyte error %d", I2C_num, addr, ret);
		return ret;
	}

	return ESP_OK;
}

esp_err_t DEV_I2C_Read_Byte(int I2C_num, uint8_t addr, uint8_t reg, uint8_t *data)
{
	esp_err_t ret = ESP_OK;
    ret = i2c_master_write_read_device(I2C_num, addr, &reg, 1, data, 1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
	if(ret != ESP_OK) {
		ELBONI_DBG_PRINT_IIC(TAG, "I2c port %d addr:%x read byte error %d", I2C_num, addr, ret);
		return ret;
	}

	return ESP_OK;
}

esp_err_t DEV_I2C_Read_nByte(int I2C_num, uint8_t addr, uint8_t reg, uint8_t *pData, uint32_t Len)
{
	esp_err_t ret = ESP_OK;
	ret = i2c_master_write_read_device(I2C_num, addr, &reg, 1, pData, Len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
	if(ret != ESP_OK) {
		ELBONI_DBG_PRINT_IIC(TAG, "I2c port %d addr:%x read nbyte error %d", I2C_num, addr, ret);
		return ret;
	}

	return ESP_OK;
}
