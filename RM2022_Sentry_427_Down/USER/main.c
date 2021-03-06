#include "main.h"

/*********************************************
Copyright(C) 水木皆Ming
All rights reserved
File name:	主函数文件
Author:		shuimujieming
Version:		1.0
Description:	
Others:		
Log:	
*********************************************/

/*
*@Description：底层设备初始化
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void BSP_Init()
{
	Power_Init();				  //初始化电源
	
	LED_Init();		        //初始化LED端口

	Laser_Init();					//激光初始化
	
	DR16_Init();					//遥控器接收机初始化
	
	CAN12_Init();					//CAN驱动初始化

	IMU_Init();						//IMU初始化

	TIM6_Init(1000);			//定时器6初始化

	Gimbal_Init();				//云台初始化
	
	Shoot_Init();					//发射初始化
	
	NUC_Init();						//NUC参数初始化

	Dual_Board_Init();		//双板通信初始化
	
	Cover_Init();					//发射限位遮挡初始化
		
	Beep_Init();					//蜂鸣器初始化
	
}

/*
*@Description：主函数
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
int main(void)
{ 
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	//设置中断优先级分组为
	
	delay_init(168);		  //初始化延时函数
	
	BSP_Init();						//底层外设驱动初始化
	
	Task_Init();					//操作系统任务初始化

	while(1)
	{
		Beep_Ctrl(600,100);
	}
		
}





