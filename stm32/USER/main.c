#include "sys.h"	
#include "delay.h"	
#include "led.h" 
#include "beep.h" 
#include "oled.h"
#include "usart.h"
#include "dht11.h"
#include "bh1750.h"
#include "timer.h"
#include "stdio.h"

//网络协议层
#include "onenet.h"

//网络设备
#include "esp8266.h"

u8 temperature_H;  //温度 整数
u8 temperature_L;  //温度 小数
u8 humidity_H;  //湿度 整数
u8 humidity_L;  //湿度 小数
float light = 0;//光照度lx

u8 isBeep; //蜂鸣器标志位
u8 isOnWLAN; //网络是否连接标志位
u8 isOnMQTT; //判断是否正确连接MQTT服务器
char MQTTbuf[64]; //MQTT上传数据

const char *subtopics[] = {"/qcmqtt/sub"}; //emqxt 订阅地址 上传
const char *pubtopics = {"/qcmqtt/pub"}; //emqxt 订阅地址 下放
unsigned short timeCount = 0;	//发送间隔变量
unsigned char *dataPtr = NULL;

char OledBuf[64]; //OLED显示数组
int main(void)
{
	delay_init();	    	 //延时函数初始化	  
	OLED_Init();

	TIM2_Int_Init(4999,7199); //10Khz的计数频率，计数到5000为500ms  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	LED_Init();		  	 	//初始化与LED连接的硬件接口
	BEEP_Init();         	//初始化蜂鸣器端口
	
	BH1750_Init();
	//while(DHT11_Init());
	Usart1_Init(115200);
	Usart2_Init(115200);
	 
	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
	OLED_StringPlus(2,1,"硬件初始化成功");
	delay_ms(1000);
	OLED_Clear();
	
	OLED_StringPlus(2,3,"WIFI连接中");
	delay_ms(500);
	ESP8266_Init();					//初始化ESP8266
	OLED_Clear();
	
	OLED_StringPlus(1,2,"SmallProject");
	
	if(isOnWLAN) {
		while(OneNet_DevLink(2));			//接入OneNET
	}
	
	if(isOnWLAN) {
		BEEP = 1;isBeep = 1;
		delay_ms(300); 
		BEEP = 0;isBeep = 0;
		
		OneNet_Subscribe(subtopics, 1); //订阅mqtt
	}

	
	while(1)
	{	

		if(timeCount % 40 == 0) { 
		/* 温湿度获取 */
		  DHT11_Read_Data(&temperature_H,&temperature_L,&humidity_H,&humidity_L);	//读取温湿度值
		 
				/* 光照度获取 */
			if(!i2c_CheckDevice(BH1750_Addr)) { //获取光照值
				light = LIght_Intensity();
			}
			/* OLED显示温湿亮度并上传串口 */
			sprintf(OledBuf,":%d.%d",temperature_H,temperature_L);
			OLED_StringPlus(2,1,"温度"); OLED_ShowString(2,5,OledBuf);
			sprintf(OledBuf,":%d.%d",humidity_H,humidity_L);
			OLED_StringPlus(3,1,"湿度"); OLED_ShowString(3,5,OledBuf);
			sprintf(OledBuf,":%.2f",light);
			OLED_StringPlus(4,1,"亮度"); OLED_ShowString(4,5,OledBuf);
		}

		if(++timeCount >= 80 && isOnMQTT)		
		{
			UsartPrintf(USART_DEBUG, "OneNet_Publish\r\n");
			
			sprintf(MQTTbuf,"{\"Temp\":%d.%d,\"Hum\":%d.%d,\"Lightlx\":%.1f,\"Beep\":%d,\"Led\":%d}",temperature_H,temperature_L,
			humidity_H,humidity_L,light,isBeep,0);
			OneNet_Publish("/qcmqtt/pub", MQTTbuf); //发送到MQTT
			
			timeCount = 0;
			ESP8266_Clear();
		}
		
		if(isOnMQTT) {//接收下行命令
			dataPtr = ESP8266_GetIPD(3);
			if(dataPtr != NULL) {
				OneNet_RevPro(dataPtr);
			}
		}
		
		//每4秒检测一次网络是否断开              //查看当前AP状态
		if(timeCount % 160 == 0 && !ESP8266_SendCmd("AT+CWJAP?\r\n","No AP")) {
			UsartPrintf(USART_DEBUG,"网络已断开");
			isOnWLAN = 0;  isOnMQTT = 0;
			ESP8266_On_WLAN(20);  //连接WLAN
			
			while(OneNet_DevLink(2));	//接入OneNET
				if(isOnMQTT) {
				  BEEP = 1;isBeep = 1;
				  delay_ms(300); 
				  BEEP = 0;isBeep = 0;
				  OneNet_Subscribe(subtopics, 1); //订阅mqtt
			}	
			timeCount -= 20000;
		}
		delay_ms(10);
	}
}

