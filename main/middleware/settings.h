/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once
#include <stdbool.h>

#define DEFAULT_ESP_WIFI_SSID      "wifi_ssid"
#define DEFAULT_ESP_WIFI_PASS      "wifi_passwd"

typedef enum {
    LANG_EN,
    LANG_CN,
    LANG_MAX,
} language_t;

typedef struct {
    bool need_hint;
    bool sr_enable;
    bool demo_gui;
    language_t lang;
    uint8_t standby_time;   /**< 0-3.*/
    uint8_t brightness;     /**< 0-100.*/

    char ssid[32];      /**< SSID of target AP. */
    char password[64];  /**< Password of target AP. */
    uint8_t ssid_len;      /**< SSID of target AP. */
    uint8_t password_len;  /**< Password of target AP. */
} sys_param_t;

esp_err_t settings_factory_reset(void);
esp_err_t settings_read_parameter_from_nvs(void);
esp_err_t settings_write_parameter_to_nvs(void);
sys_param_t *settings_get_parameter(void);
