/*  elboni_rtc.c
    RTC simple example, how to initialize I2C, configure PCF85063A
    As well as reading and writing to the register of the sensor connected through I2C and alarm interrupt.
*/
#include "elboni_rtc.h"
#include <time.h>
#include <sys/time.h>

#define RTC_DEBUG 0
#define TAG "RTC"

#if RS485_DEBUG
	#define ELBONI_DBG_PRINT_RTC(...) ESP_LOGI(__VA_ARGS__)
#else
	#define ELBONI_DBG_PRINT_RTC(...)
#endif


static datetime_t Set_Time = {
    .year = 2025,
    .month = 03,
    .day = 30,
    .dotw = 7,
    .hour = 9,
    .min = 0,
    .sec = 0
};

static datetime_t Set_Alarm_Time = {
    .year = 2025,
    .month = 03,
    .day = 30,
    .dotw = 7,
    .hour = 9,
    .min = 0,
    .sec = 10
};

#if RTC_DEBUG
static char datetime_str[256];
#endif
// External interrupt handler function
static int Alarm_flag = 0;

static void IRAM_ATTR gpio_isr_handler(void* arg) {
    Alarm_flag = 1;
}

int elboni_rtc_init(int iic_num)
{
#if RTC_DEBUG
    datetime_t Now_time;
#endif

    //Initialize PCF85063A
    PCF85063A_Init(0);
    //set time
    PCF85063A_Set_All(Set_Time);
    //set alarm
	if(CONFIG_RTC_ALARM_PIN >= 0)
	{
		PCF85063A_Set_Alarm(Set_Alarm_Time);
		//Start alarm interrupt
		PCF85063A_Enable_Alarm();
		//Initialize the ESP32 interrupt input heel callback function
		DEV_GPIO_INT(CONFIG_RTC_ALARM_PIN, gpio_isr_handler);
	}

	//set system time
	//elboni_setting_system_time(Set_Time);
#if RTC_DEBUG
    while (1)
    {
        //reading access time
        PCF85063A_Read_now(&Now_time);
        //Convert to string
        datetime_to_str(datetime_str,Now_time);
        ESP_LOGI(TAG,"Now_time is %s ",datetime_str); 
        
        if ((Alarm_flag == 1) && (CONFIG_RTC_ALARM_PIN >= 0))
        {
            Alarm_flag = 0;
            //Start the alarm again.Comment out the function if it only needs to run once 再次启动闹钟，如果只需要运行一次，需要注释这个函数
            PCF85063A_Enable_Alarm();
            ESP_LOGI(TAG,"The alarm clock goes off.");
        }
        //Delay 1 second 延时1秒
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
#endif
	return 0;
}

void elboni_setting_system_time(void)
{
	datetime_t Now_time;

	PCF85063A_Read_now(&Now_time);
	
	struct tm tm_time = {
        .tm_year = Now_time.year - 1900, // tm_year from 1900
        .tm_mon = Now_time.month - 1,   // tm_mon start form 0
        .tm_mday = Now_time.day,
        .tm_hour = Now_time.hour,
        .tm_min = Now_time.min,
        .tm_sec = Now_time.sec
    };

    time_t time_sec = mktime(&tm_time);

    if (time_sec == -1) {
        printf("Error: Failed to convert datetime_t to time_t\n");
        return;
    }

    struct timeval tv = {
        .tv_sec = time_sec,
        .tv_usec = 0
    };

	settimeofday(&tv, NULL);
}