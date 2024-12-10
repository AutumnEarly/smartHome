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

//����Э���
#include "onenet.h"

//�����豸
#include "esp8266.h"

u8 temperature_H;  //�¶� ����
u8 temperature_L;  //�¶� С��
u8 humidity_H;  //ʪ�� ����
u8 humidity_L;  //ʪ�� С��
float light = 0;//���ն�lx

u8 isBeep; //��������־λ
u8 isOnWLAN; //�����Ƿ����ӱ�־λ
u8 isOnMQTT; //�ж��Ƿ���ȷ����MQTT������
char MQTTbuf[64]; //MQTT�ϴ�����

const char *subtopics[] = {"/qcmqtt/sub"}; //emqxt ���ĵ�ַ �ϴ�
const char *pubtopics = {"/qcmqtt/pub"}; //emqxt ���ĵ�ַ �·�
unsigned short timeCount = 0;	//���ͼ������
unsigned char *dataPtr = NULL;

char OledBuf[64]; //OLED��ʾ����
int main(void)
{
	delay_init();	    	 //��ʱ������ʼ��	  
	OLED_Init();

	TIM2_Int_Init(4999,7199); //10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	LED_Init();		  	 	//��ʼ����LED���ӵ�Ӳ���ӿ�
	BEEP_Init();         	//��ʼ���������˿�
	
	BH1750_Init();
	//while(DHT11_Init());
	Usart1_Init(115200);
	Usart2_Init(115200);
	 
	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
	OLED_StringPlus(2,1,"Ӳ����ʼ���ɹ�");
	delay_ms(1000);
	OLED_Clear();
	
	OLED_StringPlus(2,3,"WIFI������");
	delay_ms(500);
	ESP8266_Init();					//��ʼ��ESP8266
	OLED_Clear();
	
	OLED_StringPlus(1,2,"SmallProject");
	
	if(isOnWLAN) {
		while(OneNet_DevLink(2));			//����OneNET
	}
	
	if(isOnWLAN) {
		BEEP = 1;isBeep = 1;
		delay_ms(300); 
		BEEP = 0;isBeep = 0;
		
		OneNet_Subscribe(subtopics, 1); //����mqtt
	}

	
	while(1)
	{	

		if(timeCount % 40 == 0) { 
		/* ��ʪ�Ȼ�ȡ */
		  DHT11_Read_Data(&temperature_H,&temperature_L,&humidity_H,&humidity_L);	//��ȡ��ʪ��ֵ
		 
				/* ���նȻ�ȡ */
			if(!i2c_CheckDevice(BH1750_Addr)) { //��ȡ����ֵ
				light = LIght_Intensity();
			}
			/* OLED��ʾ��ʪ���Ȳ��ϴ����� */
			sprintf(OledBuf,":%d.%d",temperature_H,temperature_L);
			OLED_StringPlus(2,1,"�¶�"); OLED_ShowString(2,5,OledBuf);
			sprintf(OledBuf,":%d.%d",humidity_H,humidity_L);
			OLED_StringPlus(3,1,"ʪ��"); OLED_ShowString(3,5,OledBuf);
			sprintf(OledBuf,":%.2f",light);
			OLED_StringPlus(4,1,"����"); OLED_ShowString(4,5,OledBuf);
		}

		if(++timeCount >= 80 && isOnMQTT)		
		{
			UsartPrintf(USART_DEBUG, "OneNet_Publish\r\n");
			
			sprintf(MQTTbuf,"{\"Temp\":%d.%d,\"Hum\":%d.%d,\"Lightlx\":%.1f,\"Beep\":%d,\"Led\":%d}",temperature_H,temperature_L,
			humidity_H,humidity_L,light,isBeep,0);
			OneNet_Publish("/qcmqtt/pub", MQTTbuf); //���͵�MQTT
			
			timeCount = 0;
			ESP8266_Clear();
		}
		
		if(isOnMQTT) {//������������
			dataPtr = ESP8266_GetIPD(3);
			if(dataPtr != NULL) {
				OneNet_RevPro(dataPtr);
			}
		}
		
		//ÿ4����һ�������Ƿ�Ͽ�              //�鿴��ǰAP״̬
		if(timeCount % 160 == 0 && !ESP8266_SendCmd("AT+CWJAP?\r\n","No AP")) {
			UsartPrintf(USART_DEBUG,"�����ѶϿ�");
			isOnWLAN = 0;  isOnMQTT = 0;
			ESP8266_On_WLAN(20);  //����WLAN
			
			while(OneNet_DevLink(2));	//����OneNET
				if(isOnMQTT) {
				  BEEP = 1;isBeep = 1;
				  delay_ms(300); 
				  BEEP = 0;isBeep = 0;
				  OneNet_Subscribe(subtopics, 1); //����mqtt
			}	
			timeCount -= 20000;
		}
		delay_ms(10);
	}
}

