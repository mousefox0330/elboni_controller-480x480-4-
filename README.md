# elboni_controller-480x480-4-
Board: WaveShare ESP32-S3-Touch-LCD-4
1 RS485_RXD PIN 43
  RS485_TXD PIN 44
  CAN_TX        06
  CAN_RX        00
2 CAN interface need Ground connection with connect devices, when message Transmission error. 
3 MMC（TF） interface power is control by TCA9554(A) pin 7, IIC_SDA 8, IIC_SCL 9
  Use “idf.py add-dependency esp_io_expander_tca9554==1.0.1” to add this power control chipset. IIC address is ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000(0x20) that is defined by PIN A0~A2 of TCA9554.
  TF SDMMC_CLK pin 2, SDMMC_CMD(MOSI) pin 1, SDMMC_DATA(MISO) pin 4, SDMMC_CS NO.
  This have PIN conflict!!!! With LCD.
4 RTC, this target not connect RTC ISR to main ESP32S3, so there no Alarm function. 
NOTE!!!!! Do not define PIN 18 19 for IIC or other bus, that is USB N&P.
5 idf.py add-dependency "lvgl/lvgl^8"


settings.h:19
system setting nvrom