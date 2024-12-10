#ifndef __DHT11_H
#define __DHT11_H 
#include "sys.h"   
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//DHT11������ʪ�ȴ�������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/12
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
 
//IO��������
#define DHT11_IO_IN()  {GPIOA->CRH&=0xFFFFFFF0;GPIOA->CRH|=0x00000008;}//������������
#define DHT11_IO_OUT() {GPIOA->CRH&=0xFFFFFFF0;GPIOA->CRH|=0x00000003;}//�����50MHz
////IO��������											   
#define	DHT11_DQ_OUT PAout(8) //���ݶ˿�	PA8 
#define	DHT11_DQ_IN  PAin(8)  //���ݶ˿�	PA8 


u8 DHT11_Init(void);//��ʼ��DHT11
u8 DHT11_Read_Data(u8 *temp_H,u8 *temp_L,u8 *humi_H,u8 *humi_L);  
u8 DHT11_Read_Byte(void);//����һ���ֽ�
u8 DHT11_Read_Bit(void);//����һ��λ
u8 DHT11_Check(void);//����Ƿ����DHT11
void DHT11_Rst(void);//��λDHT11    
#endif















