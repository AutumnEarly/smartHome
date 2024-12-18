#ifndef __DHT11_H
#define __DHT11_H 
#include "sys.h"   
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//DHT11数字温湿度传感器驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/12
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
 
//IO方向设置
#define DHT11_IO_IN()  {GPIOA->CRH&=0xFFFFFFF0;GPIOA->CRH|=0x00000008;}//上拉下拉输入
#define DHT11_IO_OUT() {GPIOA->CRH&=0xFFFFFFF0;GPIOA->CRH|=0x00000003;}//输出，50MHz
////IO操作函数											   
#define	DHT11_DQ_OUT PAout(8) //数据端口	PA8 
#define	DHT11_DQ_IN  PAin(8)  //数据端口	PA8 


u8 DHT11_Init(void);//初始化DHT11
u8 DHT11_Read_Data(u8 *temp_H,u8 *temp_L,u8 *humi_H,u8 *humi_L);  
u8 DHT11_Read_Byte(void);//读出一个字节
u8 DHT11_Read_Bit(void);//读出一个位
u8 DHT11_Check(void);//检测是否存在DHT11
void DHT11_Rst(void);//复位DHT11    
#endif















