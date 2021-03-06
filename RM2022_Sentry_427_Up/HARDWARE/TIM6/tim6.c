#include "tim6.h"

uint32_t imu_tick_counts=0;//时钟计数值
/*
*@Description：TIM6初始化函数
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void TIM6_Init(int arr)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef         NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);  		//使能TIM6时钟

  TIM_TimeBaseInitStructure.TIM_Period 				= arr; 									//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler			=	84 -1; 								//定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode		=	TIM_CounterMode_Up; 	//向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision	=	TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStructure);									//初始化TIM6
	
	
	NVIC_InitStructure.NVIC_IRQChannel										=	TIM6_DAC_IRQn; 	//定时器6中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	=	0; 					//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority					=	0;						//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE); 													//允许定时器6更新中断
	TIM_Cmd(TIM6,ENABLE);                                               //使能定时器6

}

float Yaw_Angle = 0;//随动，积分角度
int g_z=0;
/*
*@Description：TIM6中断处理函数
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void TIM6_DAC_IRQHandler()
{
		if(TIM_GetITStatus(TIM6,TIM_IT_Update) == SET)
	{
			//IMU积分时间基准
			imu_tick_counts++;

		{
			g_z=mpu_data.gz * 2.0f;
			
			if(abs(mpu_data.gz) < 30)		//云台飘
			{g_z = 0;}
			
			Yaw_Angle += g_z  * 0.00003f;		//0.001 随时间积分刚好是角度值
		}
			
			
		
			TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
	}
	
}