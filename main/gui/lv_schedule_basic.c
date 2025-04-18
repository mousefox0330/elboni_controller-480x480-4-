/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "settings.h"

#include "lv_schedule_basic.h"

#define TAG "lvgl_basic"
#define LVGL_BASIC_DEBUG 1
#if LVGL_BASIC_DEBUG
	#define ELBONI_DBG_PRINT_LVBASIC(...) ESP_LOGI(__VA_ARGS__)
#else
	#define ELBONI_DBG_PRINT_LVBASIC(...)
#endif

#define TIME_ON_TRIGGER  10

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_layer_t *screen_off_layer;
static lv_timer_t *timer_system;
static lv_layer_t *current_layer = NULL;

static time_out_count time_enter_clock = {
    .timeOut = 0,
    .time_base = 0,
};

extern void memory_monitor();

bool is_time_out(time_out_count *tm)
{
    int32_t isTmOut;
    isTmOut = (lv_tick_get() - (tm->time_base + tm->timeOut));
    if ((isTmOut > 0) || (0 == tm->time_base)) {
        tm->time_base = lv_tick_get();
        return true;
    } else {
        return false;
    }
}

bool set_time_out(time_out_count *tm, uint32_t ms)
{
    tm->time_base = lv_tick_get();
    tm->timeOut = ms;
    return true;
}

bool reload_time_out(time_out_count *tm)
{
    tm->time_base = lv_tick_get();
    return true;
}

void lv_func_create_layer(lv_layer_t *create_layer)
{
    bool result = false;
    result = create_layer->enter_cb(create_layer);
    if (true == result) {
        ELBONI_DBG_PRINT_LVBASIC(TAG, "[+] Create lv_layer:%s", create_layer->lv_obj_name);
    }

    if ((true == result) && (NULL == create_layer->timer_handle)) {
        create_layer->timer_handle = lv_timer_create(create_layer->timer_cb, TIME_ON_TRIGGER, NULL);
        //lv_timer_set_repeat_count(create_layer->timer_handle, 10);
        ELBONI_DBG_PRINT_LVBASIC(TAG, "[+] Create lv_timer:%s", create_layer->lv_obj_name);
    }

    if (create_layer->lv_show_layer) {
        create_layer->lv_show_layer->lv_obj_parent = create_layer->lv_obj_layer;
        result = create_layer->lv_show_layer->enter_cb(create_layer->lv_show_layer);
        if (true == result) {
            ELBONI_DBG_PRINT_LVBASIC(TAG, "[+] Create show lv_layer:%s", create_layer->lv_show_layer->lv_obj_name);
        }

        if ((true == result) && (NULL == create_layer->lv_show_layer->timer_handle)) {
            create_layer->lv_show_layer->timer_handle = lv_timer_create(create_layer->lv_show_layer->timer_cb, TIME_ON_TRIGGER, NULL);
            ELBONI_DBG_PRINT_LVBASIC(TAG, "[+] Create show lv_timer:%s", create_layer->lv_show_layer->lv_obj_name);
        }
    }
}

void lv_func_goto_layer(lv_layer_t *dst_layer)
{
    lv_timer_enable(false);
    lv_layer_t *src_layer = current_layer;

    if (src_layer) {

        if (src_layer->lv_obj_layer) {

            if (src_layer->lv_show_layer) {
                src_layer->lv_show_layer->exit_cb(src_layer->lv_show_layer);
                ELBONI_DBG_PRINT_LVBASIC(TAG, "[-] Delete show lv_layer:%s", src_layer->lv_show_layer->lv_obj_name);
                if (src_layer->lv_show_layer->lv_obj_layer) {
                    //lv_obj_del_async(src_layer->lv_show_layer->lv_obj_layer);
                    lv_obj_del(src_layer->lv_show_layer->lv_obj_layer);
                    src_layer->lv_show_layer->lv_obj_layer = NULL;
                }

                if (src_layer->lv_show_layer->timer_handle) {
                    ELBONI_DBG_PRINT_LVBASIC(TAG, "[-] Delete show lv_timer:%s,%p", src_layer->lv_show_layer->lv_obj_name, src_layer->lv_show_layer->timer_handle);
                    lv_timer_del(src_layer->lv_show_layer->timer_handle);
                    src_layer->lv_show_layer->timer_handle = NULL;
                }
            }

            src_layer->exit_cb(src_layer);
            ELBONI_DBG_PRINT_LVBASIC(TAG, "[-] Delete lv_layer :%s", src_layer->lv_obj_name);
            //lv_obj_del_async(src_layer->lv_obj_layer);
            lv_obj_del(src_layer->lv_obj_layer);
            src_layer->lv_obj_layer = NULL;
        }

        if (src_layer->timer_handle) {
            ELBONI_DBG_PRINT_LVBASIC(TAG, "[-] Delete lv_timer :%s,%p", src_layer->lv_obj_name, src_layer->timer_handle);
            lv_timer_del(src_layer->timer_handle);
            src_layer->timer_handle = NULL;
        }

        lv_timer_t *list;
        while ((list = lv_timer_get_next(NULL)) != timer_system) {
            ELBONI_DBG_PRINT_LVBASIC(TAG, "lv_time_del, %p,%p", list, timer_system);
            lv_timer_del(list);
        }

        lv_anim_del_all();
    }

    if (dst_layer) {
        if (NULL == dst_layer->lv_obj_layer) {
            lv_func_create_layer(dst_layer);
        } else {
            ELBONI_DBG_PRINT_LVBASIC(TAG, "%s != NULL", dst_layer->lv_obj_name);
        }
        current_layer = dst_layer;
    }

    lv_timer_enable(true);
}

/*
 * once only
 */
void lv_create_home(lv_layer_t *home_layer)
{
    ELBONI_DBG_PRINT_LVBASIC(TAG, "Enter home page");
    timer_system = lv_timer_get_next(NULL);
    lv_func_goto_layer(home_layer);
}

void feed_clock_time()
{
    reload_time_out(&time_enter_clock);
}

void enter_clock_time()
{
    ELBONI_DBG_PRINT_LVBASIC(TAG, "screen off");
    lv_func_goto_layer(screen_off_layer);
    time_enter_clock.time_base = 0;
}

void reset_clock_time(uint8_t index)
{
    uint32_t clock_time[] = {15 * 1000, 5 * 60 * 1000, 1 * 60 * 60 * 1000, 0xFFFFFFFF};
    assert(index < 4);

    uint32_t tmOut = clock_time[index];
    ELBONI_DBG_PRINT_LVBASIC(TAG, "reset clock time:%d sec", tmOut / 1000);
    set_time_out(&time_enter_clock, tmOut);
}

static void time_clock_update_cb(lv_timer_t *timer)
{
    lv_obj_t *obj = (lv_obj_t *)timer->user_data;
    screen_off_layer = (lv_layer_t *)obj;

    if (is_time_out(&time_enter_clock)) {
        lv_func_goto_layer(screen_off_layer);
        feed_clock_time();
    }
}

void lv_create_clock(lv_layer_t *clock_layer)
{
    sys_param_t *sys_set = settings_get_parameter();
    reset_clock_time(sys_set->standby_time);

    lv_timer_t *timer_clock = lv_timer_create(time_clock_update_cb, 1 * 1000, clock_layer);
    if ( timer_clock ) {
        timer_system = timer_clock;
        ELBONI_DBG_PRINT_LVBASIC(TAG, "Init clock time ok, %p", timer_clock);
    }
}
