/*****************************************************************************
 *  elboni_can.h 
 *
 ******************************************************************************/

#ifndef __RTC_H
#define __RTC_H
#include "PCF85063A.h"

int elboni_rtc_init(int iic_num);
void elboni_setting_system_time(void);
#endif