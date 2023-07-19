#include "rtthread.h"
#include "main.h"
#include "stdio.h"
#include "adc.h"
#include "usart.h"
#include "gpio.h"
#include "String.h"
#include "ds18b20.h"
#include "NMEA0183.h"
#include "cJSON.h"
#include "cJSON_test.h"
#include <stdlib.h>

uint16_t location_check_count = 1;

struct rt_thread led_thread;
rt_uint8_t rt_led_thread_stack[128];
void led_task_entry(void *parameter);

struct rt_thread ADC_thread;
rt_uint8_t rt_ADC_thread_stack[512];
void ADC_task_entry(void *parameter);

struct rt_thread Location_thread;
rt_uint8_t rt_Location_thread_stack[1024];
void StartTask_Location(void *parameter);

struct rt_thread temperature_thread;
rt_uint8_t rt_temperature_thread_stack[512];
void temperature_task_entry(void *parameter);

// 串口发送任务
struct rt_thread Usart_send_thread;
rt_uint8_t rt_Usart_send_thread_stack[1024];
void Usart_send_task_entry(void *parameter);

typedef struct _Location_struct
{
  uint8_t satellite_count;//卫星数量
  uint8_t hdop;//水平精度
  uint8_t quality_indicator;//定位状况
  float ground_speed;
  float ground_course;
  float lng;
  float lat;
  float alt;
  uint8_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t min;
  uint8_t sec;
} Location_struct;


typedef struct _GreenWaterPioneer_struct
{
  Location_struct Location;
  float Temperature;
  float PH ;
  float Turbidity;
} GreenWaterPioneer_struct;

void create_objects(void);
GreenWaterPioneer_struct GreenWaterPioneer;

uint8_t DATA_BUFF[2048];
uint16_t DATA_SIZE = 0;

uint8_t DATA3_BUFF[1024], rx3_flag;
uint16_t DATA3_SIZE = 0;
char *strx1,*strx2,*strxPowerSwitch,*strxBrightness,*strxColor;
uint8_t uart3RxState = 0;

extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;
uint16_t ADC_Value[20];
uint8_t i;
uint16_t adc1, adc2;
float ADC_Vol1, ADC_Vol2;

float Temp = 25.0;

unsigned int index = 0;

// 初始化线程函数
void MX_RT_Thread_Init(void)
{
  // 初始化LED线程
  rt_thread_init(&led_thread, "led", led_task_entry, RT_NULL,
                 &rt_led_thread_stack[0], sizeof(rt_led_thread_stack), 2, 20);
  rt_thread_init(&ADC_thread, "ADC", ADC_task_entry, RT_NULL,
                 &rt_ADC_thread_stack[0], sizeof(rt_ADC_thread_stack), 4, 20);
  rt_thread_init(&Location_thread, "Location", StartTask_Location, RT_NULL,
                 &rt_Location_thread_stack[0], sizeof(rt_Location_thread_stack), 3, 20);
  rt_thread_init(&temperature_thread, "temperature", temperature_task_entry, RT_NULL,
                 &rt_temperature_thread_stack[0], sizeof(rt_temperature_thread_stack), 5, 20);
	rt_thread_init(&Usart_send_thread, "Usart_send", Usart_send_task_entry, RT_NULL,
                 &rt_Usart_send_thread_stack[0], sizeof(rt_Usart_send_thread_stack), 6, 20);
  // 开启线程调度
  rt_thread_startup(&led_thread);
  rt_thread_startup(&ADC_thread);
  rt_thread_startup(&Location_thread);
  rt_thread_startup(&temperature_thread);
	rt_thread_startup(&Usart_send_thread);
}

// 主任务
void MX_RT_Thread_Process(void)
{
  // printf("Hello RT_Thread!!!\r\n");
  rt_thread_delay(2000);
}

// LED任务
void led_task_entry(void *parameter)
{
  while (1)
    {
      HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
      rt_thread_delay(50);
      HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
      rt_thread_delay(50);
    }
}

// ADC任务
void ADC_task_entry(void *parameter)
{
  while (1)
    {
      //HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
      //printf("adc1: ");
      adc1 = 0;
      adc2 = 0;
      for(i = 0; i < 20; i ++)
        {
          if(i % 2 == 0)
            {
              adc1 += ADC_Value[i];
              //printf("%4d ",ADC_Value[i]);
            }
          else
            adc2 += ADC_Value[i];
        }
      /*
      printf("\r\nadc2: ");
      for(i = 0; i < 20; i ++)
      {
      	if(i%2){
      		//adc2 += ADC_Value[i];
      		printf("%4d ",ADC_Value[i]);
      	}
      }

      printf("\r\n");
      printf("adc1相加: %d , adc2相加: %d\r\n",adc1,adc2);
      */
      adc1 /= 10;
      adc2 /= 10;
      //printf("adc1:%d, adc1:%d\r\n",adc1,adc2);
      ADC_Vol1 = (float)adc1 / 4096 * 3.3;//转换电压
      ADC_Vol2 = (float)adc2 / 4096 * 3.3;//转换电压
			GreenWaterPioneer.PH = 3.459*ADC_Vol1;
			GreenWaterPioneer.Turbidity = 3000-(-1120.4*(ADC_Vol2 + 1)*(ADC_Vol2 + 1)+5743.3*(ADC_Vol2 + 1)-4200);
      //printf("Voltage 1(C0): %.3f V, Voltage 2(C1): %.3f V\r\n", ADC_Vol1, ADC_Vol2);
      //HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
      rt_thread_delay(500);
    }
}

/* USER CODE BEGIN Header_StartTask_Location */
/**
 * @brief Function implementing the Task_Location thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartTask_Location */
void StartTask_Location(void *parameter)
{
  /* USER CODE BEGIN StartTask_Location */
  /* Infinite loop */
  while (1)
    {
      // myprintf(&huart1,"解析代码成功 ");
      /*
      if(GPS_SENTENCE_GGA != 0)
      	printf("GPS_SENTENCE_GGA:%d\r\n",
      					GPS_SENTENCE_GGA);
      */
      if (nmea0183.gpsParse.new_satellite_count != 0){
        //printf("卫星数量:%d,水平精度:%d,定位状况:%s\r\n", nmea0183.gpsParse.new_satellite_count, nmea0183.gpsParse.new_hdop, nmea0183.gpsParse.new_quality_indicator ? "定位成功" : "正在定位");
				GreenWaterPioneer.Location.satellite_count = nmea0183.gpsParse.new_satellite_count;
				GreenWaterPioneer.Location.hdop = nmea0183.gpsParse.new_hdop;
				GreenWaterPioneer.Location.quality_indicator = nmea0183.gpsParse.new_quality_indicator;
			}
			if (nmea0183.gpsData.ground_speed != 0){
        //printf("地面速度:%.3fkm/h,地面航向:%.3f\r\n", nmea0183.gpsData.ground_speed, nmea0183.gpsData.ground_course);
				GreenWaterPioneer.Location.ground_speed = nmea0183.gpsData.ground_speed;
				GreenWaterPioneer.Location.ground_course = nmea0183.gpsData.ground_course;
			}
      if (nmea0183.gpsData.location.lng != 0){
        //printf("东经:%f,北纬:%f,海拔:%.2f米\r\n", nmea0183.gpsData.location.lng, nmea0183.gpsData.location.lat, (float)nmea0183.gpsData.location.alt / 100);
				GreenWaterPioneer.Location.lng = nmea0183.gpsData.location.lng;
				GreenWaterPioneer.Location.lat = nmea0183.gpsData.location.lat;
				GreenWaterPioneer.Location.lng = nmea0183.gpsData.location.alt;
			}
      if (nmea0183.gpsData.date_time.year != 0){
        //printf("卫星UTC时间:20%d-%d-%d %02d:%02d:%02d\r\n", nmea0183.gpsData.date_time.year, 
				//nmea0183.gpsData.date_time.month, nmea0183.gpsData.date_time.day, nmea0183.gpsData.date_time.hour, 
				//nmea0183.gpsData.date_time.min, nmea0183.gpsData.date_time.sec);
				
				GreenWaterPioneer.Location.year = nmea0183.gpsData.date_time.year;
				GreenWaterPioneer.Location.month = nmea0183.gpsData.date_time.month;
				GreenWaterPioneer.Location.day = nmea0183.gpsData.date_time.day;
				GreenWaterPioneer.Location.hour = nmea0183.gpsData.date_time.hour;
				GreenWaterPioneer.Location.min = nmea0183.gpsData.date_time.min;
				GreenWaterPioneer.Location.sec = nmea0183.gpsData.date_time.sec;
			}
      else if (location_check_count == 0 && nmea0183.gpsData.location.lng == 0 && nmea0183.gpsParse.new_satellite_count == 0)
        {
          location_check_count = 1;
          //printf("正在搜星......\r\n");
          // HAL_UART_DeInit(&huart2);
          //USART2_Change(9600);
          //myprintf(&huart2, "$PCAS01,5*19\r\n");
          // HAL_UART_DeInit(&huart2);
          //USART2_Change(115200);
          //memset(DATA_BUFF, 0, 2048);
          //HAL_UARTEx_ReceiveToIdle_DMA(&huart2, DATA_BUFF, 2048); //开启串口DMA接收
          rt_thread_delay(500);
        }
      rt_thread_delay(1000);
      location_check_count--;
    }
  /* USER CODE END StartTask_Location */
}

// 获取温度任务
void temperature_task_entry(void *parameter)
{
  while (1)
    {
      DS18B20_GetTemperature(); //获取温度
      Temp = (float)DS18B20_TempDataStruct.TempDataAll / 100;
			GreenWaterPioneer.Temperature = Temp;
      //printf("温度:%.2f\r\n", Temp);
      //HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
      rt_thread_delay(2000);
    }
}

// 串口发送任务
void Usart_send_task_entry(void *parameter)
{
  while (1)
    {
      create_objects();
      rt_thread_delay(1000);
    }
}


/* Create a bunch of objects as demonstration. */
void create_objects(void)
{
  /* declare a few. */
  cJSON *root = NULL;
  cJSON *satellite = NULL;

  /* Our array of "records": */
  root = cJSON_CreateObject();

  //cJSON_AddItemToObject(root, "",fld = cJSON_CreateObject());
  cJSON_AddNumberToObject(root, "temperature", (uint16_t)(GreenWaterPioneer.Temperature*1000));
  cJSON_AddNumberToObject(root, "ph", (uint16_t)(GreenWaterPioneer.PH*1000));
  cJSON_AddNumberToObject(root, "turbidity", (uint16_t)(GreenWaterPioneer.Turbidity));
  cJSON_AddItemToObject(root, "satellite",satellite = cJSON_CreateObject());
  cJSON_AddNumberToObject(satellite, "Satellite_count", GreenWaterPioneer.Location.satellite_count);
  cJSON_AddNumberToObject(satellite, "hdop", GreenWaterPioneer.Location.hdop);
  cJSON_AddNumberToObject(satellite, "quality_indicator", GreenWaterPioneer.Location.quality_indicator);
  cJSON_AddNumberToObject(satellite, "ground_speed", GreenWaterPioneer.Location.ground_speed);
  cJSON_AddNumberToObject(satellite, "ground_course", GreenWaterPioneer.Location.ground_course);

  cJSON_AddNumberToObject(satellite, "lng", GreenWaterPioneer.Location.lng);
  cJSON_AddNumberToObject(satellite, "lat", GreenWaterPioneer.Location.lat);
  cJSON_AddNumberToObject(satellite, "alt", GreenWaterPioneer.Location.alt);
  cJSON_AddNumberToObject(satellite, "year", GreenWaterPioneer.Location.year);
  cJSON_AddNumberToObject(satellite, "month", GreenWaterPioneer.Location.month);
  cJSON_AddNumberToObject(satellite, "day", GreenWaterPioneer.Location.day);
  cJSON_AddNumberToObject(satellite, "hour", GreenWaterPioneer.Location.hour);
  cJSON_AddNumberToObject(satellite, "min", GreenWaterPioneer.Location.min);
  cJSON_AddNumberToObject(satellite, "sec", GreenWaterPioneer.Location.sec);
/*
	cJSON_AddNumberToObject(satellite, "lng",110.046529);
  cJSON_AddNumberToObject(satellite, "lat", 34.016344);
  cJSON_AddNumberToObject(satellite, "alt", 92556);
	cJSON_AddNumberToObject(satellite, "year", 23);
  cJSON_AddNumberToObject(satellite, "month", 7);
  cJSON_AddNumberToObject(satellite, "day", 14);
  cJSON_AddNumberToObject(satellite, "hour", 13);
  cJSON_AddNumberToObject(satellite, "min", 12);
  cJSON_AddNumberToObject(satellite, "sec", 11);
*/
	char *cPrintUnformatted = cJSON_PrintUnformatted(root);
		printf("%c%s%c",0xB0,cPrintUnformatted,0xB1);
		#ifdef L610
		memset(DATA3_BUFF,0,1024);	DATA3_SIZE=0;	uart3RxState=0;
		myprintf(&huart3,"AT+TCMQTTPUB=\"iot/GWP\",1,\"{\\\"method\\\":\\\"report\\\",\\\"clientToken\\\":\\\"123\\\",\\\"params\\\":%s\"\r\n",cPrintUnformatted);
		rt_thread_delay(1000);
		HAL_UART_Transmit(&hlpuart1,(uint8_t *)DATA3_BUFF,DATA3_SIZE,0xffff);
		
		strx1=strstr((const char*)DATA3_BUFF,(const char*)"success");
		while(strx1==NULL)
		{
			HAL_UART_Transmit(&hlpuart1,(uint8_t *)"上报失败",sizeof("上报失败"),0xffff);
			rt_thread_delay(1000);memset(DATA3_BUFF,0,1024);	DATA3_SIZE=0;	
			myprintf(&huart3,"AT+TCMQTTPUB=\"iot/GWP\",1,\"{\\\"method\\\":\\\"report\\\",\\\"clientToken\\\":\\\"123\\\",\\\"params\\\":%s\"\r\n",cPrintUnformatted);
			rt_thread_delay(2000);				
			strx1=strstr((const char*)DATA3_BUFF,(const char*)"success");
		}
		HAL_UART_Transmit(&hlpuart1,(uint8_t *)"上报成功",sizeof("上报成功"),0xffff);
		#endif
		free(cPrintUnformatted);
		cJSON_Delete(root);
}

