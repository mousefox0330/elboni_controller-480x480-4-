#include "esp_lcd_touch_gt911.h"
#include "esp_lcd_st7701.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"


#define VERSION_MAJOR 0
#define VERSION_MINOR 3
#define VERSION_PATCH 2
#define VERSION_INFO ""

struct remote_device{
	char name[32];
	int name_len;
	esp_ble_gap_cb_param_t scan_result;
};

typedef enum {
    BLE_SCAN_IDLE,
    BLE_SCAN_BUSY,
    BLE_SCAN_RENEW,
    BLE_SCAN_UPDATE,
} ble_scan_status_t;

typedef enum {
    BLE_STATUS_CONNECTING,
    BLE_STATUS_CONNECTED_OK,
    BLE_STATUS_CONNECTED_FAILED,
} ble_Connect_Status;

void elboni_nvram_init(void);
esp_err_t bsp_spiffs_unmount(void);
esp_err_t elboni_spiffs_mount(void);
esp_err_t elboni_spiffs_unmount(void);
void elboni_ble_gatt_client(void);
void elboni_ble_gatt_scan(int timeout);
void elboni_ble_gatt_connect(struct remote_device *device);
esp_err_t elboni_ble_disable(void);
ble_scan_status_t elboni_ble_scan_state(void);
ble_Connect_Status elboni_ble_connect_state(void);
int elboni_ble_scan_count(void);
struct remote_device *elboni_ble_get_scan_result(int i);
bool app_ble_lock(uint32_t timeout_ms);
void app_ble_unlock(void);
esp_lcd_panel_handle_t elboni_st7701_lcd_init(void);
esp_lcd_touch_handle_t elboni_gt911_touch_init(int i2c_port);
esp_err_t lvgl_port_init(esp_lcd_panel_handle_t lcd, esp_lcd_touch_handle_t tp);
void elboni_panel_event_callbacks(esp_lcd_panel_handle_t lcd_handle);
void app_sntp_init(void);