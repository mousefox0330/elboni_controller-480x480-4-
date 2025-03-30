
#include "esp_err.h"
#include "esp_log.h"
#include "esp_log.h"
#include "esp_check.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_spiffs.h"
#include "settings.h"

#define TAG "NVRAM"
#define NVRAM_DEBUG 1
#if NVRAM_DEBUG
	#define ELBONI_DBG_PRINT_NVRAM(...) ESP_LOGI(__VA_ARGS__)
#else
	#define ELBONI_DBG_PRINT_NVRAM(...)
#endif


#define SPIFFS_MOUNT_POINT "/spiffs"
#define SPIFFS_PARTITION_LABEL "storage"
#define SPIFFS_MAX_FILES 5

void elboni_nvram_init(void)
{
	esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    ESP_ERROR_CHECK(settings_read_parameter_from_nvs());
}


/**************************************************************************************************
 *
 * SPIFFS Function
 *
 **************************************************************************************************/
esp_err_t elboni_spiffs_mount(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = SPIFFS_MOUNT_POINT,
        .partition_label = SPIFFS_PARTITION_LABEL,
        .max_files = SPIFFS_MAX_FILES,
#ifdef CONFIG_BSP_SPIFFS_FORMAT_ON_MOUNT_FAIL
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif
    };

    esp_err_t ret_val = esp_vfs_spiffs_register(&conf);

    ESP_ERROR_CHECK(ret_val);

    size_t total = 0, used = 0;
    ret_val = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret_val != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret_val));
    } else {
        ELBONI_DBG_PRINT_NVRAM(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    return ret_val;
}

esp_err_t bsp_spiffs_unmount(void)
{
    return esp_vfs_spiffs_unregister(SPIFFS_PARTITION_LABEL);
}