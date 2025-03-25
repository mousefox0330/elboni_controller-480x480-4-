#include "esp_lcd_touch_gt911.h"
#include "esp_lcd_st7701.h"

void elboni_nvram_init(void);
esp_err_t bsp_spiffs_unmount(void);
esp_err_t elboni_spiffs_mount(void);
esp_err_t elboni_spiffs_unmount(void);
void elboni_ble_gatt_client(void);
esp_lcd_panel_handle_t elboni_st7701_lcd_init(void);
esp_lcd_touch_handle_t elboni_gt911_touch_init(int i2c_port);