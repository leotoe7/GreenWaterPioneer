#ifndef __DS18B20_H__
#define __DS18B20_H__

#include "gpio.h"
#include "rtthread.h"
#include "osdelay.h"

#define HAL_Delay_us(us) rt_hw_us_delay(us)

//各部分温度数据
typedef struct DS18B20_Temperature
{
    uint16_t Hun;
    uint16_t Tens;
    uint16_t Unit;
    uint16_t Decimals1;
    uint16_t Decimals2;
    uint16_t TempDataAll;
} DS18B20_TempDataTypeDef;
extern DS18B20_TempDataTypeDef DS18B20_TempDataStruct;

#define in 1
#define out 2
#define DS18B20_IO_Read() HAL_GPIO_ReadPin(DS18_GPIO_Port, DS18_Pin)
#define DS18B20_IO_Write_1() HAL_GPIO_WritePin(DS18_GPIO_Port, DS18_Pin, GPIO_PIN_SET)
#define DS18B20_IO_Write_0() HAL_GPIO_WritePin(DS18_GPIO_Port, DS18_Pin, GPIO_PIN_RESET)

uint16_t DS18B20_Init(void);
void DS18B20_GetTemperature(void);

#endif //__DS18B20_H__