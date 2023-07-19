#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "oledfont.h"
#include <Ticker.h>                       // 使用Ticker库，需要包含头文件
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#ifndef APSSID
#define APSSID "GWP"
#define APPSK "12345678"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;
uint8_t key = 'q', key_temp = 0, controller = 0;
uint8_t controller_key = 'q', controller_key_temp = 0;
String json_buf;

float temperature,ph,turbidity,lng,lat,alt;

Ticker timer1;
Ticker timer2;

ESP8266WebServer server(80);

const size_t capacity = JSON_OBJECT_SIZE(2) + 512;
DynamicJsonDocument doc(capacity);

void handleRoot() {
  server.send(200, "text/html", 
  "<h1>You are connected</h1><br><h1>\u8fd9\u662f\u4e00\u4e2awebsocket\u89c6\u9891\u6d41</h1>");
}


void handleJson() {
  json_buf = server.arg("json");
  //Serial.println(json_buf);
  server.send(200, "text/html", "1");
  deserializeJson(doc, json_buf);
  temperature = float(doc["temperature"].as<int>())/1000;
  ph = float(doc["ph"].as<int>())/1000;
  turbidity = float(doc["turbidity"].as<int>());
  lng = doc["satellite"]["lng"].as<float>();
  lat = doc["satellite"]["lat"].as<float>();
  alt = float(doc["satellite"]["alt"].as<int>())/100;
  Serial.printf("温度:%.3f, PH:%.3f 浑浊度:%.3f\r\n",temperature,ph,turbidity);
  Serial.printf("经度:%.6f, 纬度:%.6f 海拔:%.2f\r\n",lng,lat,alt);
  OLED_Showdecimal(56,0,lng,3,6,8);
  OLED_Showdecimal(62,1,lat,2,6,8);
  OLED_Showdecimal(40,2,temperature,2,2,16);
  OLED_ShowNum(40,4,(uint16_t)(turbidity),4,16);
  OLED_Showdecimal(72,6,ph,2,3,16);
}

void stop_boat() {
  key = 'q';
  tingzhi1();
  tingzhi2();
}

void check_action() {
  if (key != key_temp) {
    key_temp = key;
    switch (key) {
      case 'w': {
          controller = 0;
          OLED_ShowChinese(0, 6, 11, 16);
          OLED_ShowChinese(16, 6, 12, 16); //前进
          break;
        }
      case 'a': {
          controller = 0;
          OLED_ShowChinese(0, 6, 15, 16);
          OLED_ShowChinese(16, 6, 17, 16); //左转
          break;
        }
      case 's': {
          controller = 0;
          OLED_ShowChinese(0, 6, 13, 16);
          OLED_ShowChinese(16, 6, 14, 16); //后退
          break;
        }
      case 'd': {
          controller = 0;
          OLED_ShowChinese(0, 6, 16, 16);
          OLED_ShowChinese(16, 6, 17, 16); //右转
          break;
        }
      case 'r': {
          controller = 0;
          OLED_ShowChinese(0, 6, 13, 16);
          OLED_ShowChinese(16, 6, 15, 16); //后退左转
          break;
        }
      case 'f': {
          controller = 0;
          OLED_ShowChinese(0, 6, 13, 16);
          OLED_ShowChinese(16, 6, 16, 16); //后退右转
          break;
        }
      case 't': {
          controller = 0;
          OLED_ShowString(0, 6, "    ", 16);
          break;
        }
      case 'y': {
          controller = 0;
          OLED_ShowString(0, 6, "    ", 16);
          break;
        }
      case 'q': {
          controller = 1;
          OLED_ShowString(0, 6, "    ", 16);
          break;
        }
        //default : {server.send(404, "text/html", "<h1>You are lost</h1>");}
    }
  }
  /*
    if (key == 'q') {
      Serial.print(digitalRead(D5));
      Serial.print(" ");
      Serial.println(digitalRead(D7));
      if (controller_key != controller_key_temp) {
        controller_key_temp = controller_key;

      }
    }
  */
}

void action() {
  if (timer2.active()) {
    timer2.detach();
    timer2.once(2, stop_boat);
  }
  key = server.arg("action")[0];
  switch (key) {
    case 'w': {
        qianjin();
        break;
      }
    case 'a': {
        zuozhuan();
        break;
      }
    case 's': {
        houtui();
        break;
      }
    case 'd': {
        youzhuan();
        break;
      }
    case 'r': {
        htzuozhuan();
        break;
      }
    case 'f': {
        htyouzhuan();
        break;
      }
    case 't': {
        tingzhi1();
        break;
      }
    case 'y': {
        tingzhi2();
        break;
      }
    case 'q': {
        tingzhi1();
        tingzhi2();
        break;
      }
      //default : {server.send(404, "text/html", "<h1>You are lost</h1>");}
  }
  server.send(200, "text/html", String(char(key)));
}

void setup() {
  pinMode(D5, INPUT);
  pinMode(D7, INPUT);
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/sub_json", handleJson);
  server.on("/action", action);
  server.begin();
  Serial.println("HTTP server started");
  OLED_Init();
  OLED_ColorTurn(0);//0正常显示 1反色显示
  OLED_DisplayTurn(0);//0正常显示 1翻转180度显示

  OLED_ShowChinese(0, 0, 0, 16); //经
  OLED_ShowChinese(16, 0, 1, 16); //纬
  OLED_ShowChinese(32, 0, 2, 16); //度
  OLED_ShowChar(48, 0, ':', 16);
  //OLED_ShowString(0,0,"lng:",8);
  
//  OLED_ShowChinese(0, 2, 1, 16); //纬
//  OLED_ShowChinese(16, 2, 2, 16); //度
//  OLED_ShowChar(32, 2, ':', 16);

  OLED_ShowChinese(0, 2, 20, 16); 
  OLED_ShowChinese(16, 2, 21, 16); //温度
  OLED_ShowChar(32, 2, ':', 16);

  OLED_ShowChinese(0, 4, 22, 16); 
  OLED_ShowChinese(16, 4, 21, 16); //浊度
  OLED_ShowString(32, 4,":     TU", 16);

  OLED_ShowString(48,6,"PH:",16);
  //timer1.attach(2000, esp8266Http);
}

void loop() {
  server.handleClient();
  check_action();
  
}
