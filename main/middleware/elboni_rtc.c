/*  elboni_rtc.c
    RTC simple example, how to initialize I2C, configure PCF85063A
    As well as reading and writing to the register of the sensor connected through I2C and alarm interrupt.
*/
#include "PCF85063A.h"

#define RTC_DEBUG 1
static const char *TAG = "RTC";

static datetime_t Set_Time = {
    .year = 2024,
    .month = 02,
    .day = 02,
    .dotw = 5,
    .hour = 9,
    .min = 0,
    .sec = 0};

static datetime_t Set_Alarm_Time = {
    .year = 2024,
    .month = 02,
    .day = 02,
    .dotw = 5,
    .hour = 9,
    .min = 0,
    .sec = 2};

char datetime_str[256];
char ip[20];

// External interrupt handler function
int Alarm_flag = 0;
static void IRAM_ATTR gpio_isr_handler(void* arg) {
    Alarm_flag = 1;
}


int elboni_rtc_init(void)
{
    datetime_t Now_time;
    //Initialize PCF85063A
    PCF85063A_Init();
    //set time
    PCF85063A_Set_All(Set_Time);
    //set alarm
	if(CONFIG_RTC_ALARM_PIN >= 0)
	{
		PCF85063A_Set_Alarm(Set_Alarm_Time);
		//Start alarm interrupt
		PCF85063A_Enable_Alarm();
		//Initialize the ESP32 interrupt input heel callback function
		DEV_GPIO_INT(6, gpio_isr_handler);
	}

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
}
