#include "DS18B20.h"
#include "main.h"
#include "rtthread.h"

static void DS18B20_IO_Mode(uint8_t in_or_out);
static void DS18B20_Write_Byte(uint8_t Wdata);
static uint8_t DS18B20_Read_Byte(void);

uint8_t Sign = 1; //温度的正1负0标志
DS18B20_TempDataTypeDef DS18B20_TempDataStruct = {0};

//DS18B20_IO模式配置
static void DS18B20_IO_Mode(uint8_t in_or_out)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DS18_Pin;
    if (in_or_out == 1)
    {
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    }
    else if (in_or_out == 2)
    {
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    }
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DS18_GPIO_Port, &GPIO_InitStruct);
}

//DS18B20初始化
uint16_t DS18B20_Init(void)
{
    uint16_t DS_flag;
    DS18B20_IO_Mode(out);
    DS18B20_IO_Write_0();
    HAL_Delay_us(600);
    DS18B20_IO_Write_1();
    HAL_Delay_us(30);
    DS18B20_IO_Mode(in);
    DS_flag = DS18B20_IO_Read();
    HAL_Delay_us(500);
    return DS_flag;
}

//向DS18B20写入一个字节数据
static void DS18B20_Write_Byte(uint8_t Wdata)
{
    uint8_t i;
    for (i = 8; i > 0; i--)
    {
        DS18B20_IO_Mode(out);
        DS18B20_IO_Write_0();
        HAL_Delay_us(2);
        if ((Wdata & 0x01) == 1) //从最低位开始写
            DS18B20_IO_Write_1();
        else
            DS18B20_IO_Write_0();
        HAL_Delay_us(70);
        DS18B20_IO_Write_1();
        Wdata >>= 1;
    }
}

//向DS18B20读出一个字节数据
static uint8_t DS18B20_Read_Byte(void)
{
    uint8_t i;
    uint8_t Rdata = 0;
    for (i = 8; i > 0; i--)
    {
        DS18B20_IO_Mode(out);
        DS18B20_IO_Write_0();
        HAL_Delay_us(2);
        DS18B20_IO_Write_1();
        HAL_Delay_us(12);
        Rdata >>= 1;
        DS18B20_IO_Mode(in);
        if (DS18B20_IO_Read() == 1)
            Rdata |= 0x80;
        HAL_Delay_us(50);
    }
    return Rdata;
}

//DS18B20获取温度
void DS18B20_GetTemperature(void)
{
    /*
    读到的温度数据 Byte0,Byte1 (12位精度)
    TempL  2^3  2^2  2^1  2^0  2^-1  2^-2  2^-3  2^-4
    TempH   S    S    S    S    S    2^6   2^5   2^4
    S=0(温度为正)  S=1(温度为负)
    */
    uint8_t TempL;
    uint8_t TempH;

    DS18B20_Init();
    DS18B20_Write_Byte(0xcc); //跳过读取序列号(ROM)
    DS18B20_Write_Byte(0x44); //启动温度检测

    rt_thread_delay(750); //延时800ms(温度检测时间  12位精度检测时间>750ms(上电默认12位分辨率))
		//while (DS18B20_Read_Byte() != 0xFF);

    DS18B20_Init();
    DS18B20_Write_Byte(0xcc);
    DS18B20_Write_Byte(0xbe); //读取温度寄存器内容(读取头两个值为温度)

    TempL = DS18B20_Read_Byte(); //读取Byte0温度数据
    TempH = DS18B20_Read_Byte(); //读取Byte1温度数据
    DS18B20_TempDataStruct.TempDataAll = (TempH << 8) | TempL;
    if (TempH > 7)
    {
        DS18B20_TempDataStruct.TempDataAll = ~DS18B20_TempDataStruct.TempDataAll + 1;
        Sign = 0;
    }
		else
			Sign = 1;
    DS18B20_TempDataStruct.TempDataAll = DS18B20_TempDataStruct.TempDataAll * 0.0625 * 100 + 0.5;
    /*留两位小数就*100，+0.5是四舍五入，因为C语言浮点数转换为整型的时候把小数点
    后面的数自动去掉，不管是否大于0.5，而+0.5之后大于0.5的就是进1了，小于0.5的就
    算加上0.5，还是在小数点后面*/

    //拆解各个部分的数据
    DS18B20_TempDataStruct.Hun = DS18B20_TempDataStruct.TempDataAll / 10000;
    DS18B20_TempDataStruct.Tens = (DS18B20_TempDataStruct.TempDataAll / 1000) % 10;
    DS18B20_TempDataStruct.Unit = (DS18B20_TempDataStruct.TempDataAll / 100) % 10;
    DS18B20_TempDataStruct.Decimals1 = (DS18B20_TempDataStruct.TempDataAll / 10) % 10;
    DS18B20_TempDataStruct.Decimals2 = DS18B20_TempDataStruct.TempDataAll % 10;
}