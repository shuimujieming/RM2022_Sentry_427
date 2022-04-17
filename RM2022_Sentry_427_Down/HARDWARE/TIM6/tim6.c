#include "tim6.h"

uint32_t imu_tick_counts=0;//ʱ�Ӽ���ֵ
/*
*@Description��TIM6��ʼ������
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void TIM6_Init(int arr)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef         NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);  		//ʹ��TIM6ʱ��

  TIM_TimeBaseInitStructure.TIM_Period 				= arr; 									//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler			=	84 -1; 								//��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode		=	TIM_CounterMode_Up; 	//���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision	=	TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStructure);									//��ʼ��TIM6
	
	
	NVIC_InitStructure.NVIC_IRQChannel										=	TIM6_DAC_IRQn; 	//��ʱ��6�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	=	0; 					//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority					=	0;						//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE); 													//����ʱ��6�����ж�
	TIM_Cmd(TIM6,ENABLE);                                               //ʹ�ܶ�ʱ��6

}

float Yaw_Angle = 0;//�涯�����ֽǶ�
int g_z=0;
/*
*@Description��TIM6�жϴ�����
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void TIM6_DAC_IRQHandler()
{
		if(TIM_GetITStatus(TIM6,TIM_IT_Update) == SET)
	{
			//IMU����ʱ���׼
			imu_tick_counts++;

		{
			g_z=mpu_data.gy * 2.0f;
			
			if(abs(mpu_data.gy) < 30)		//��̨Ʈ
			{g_z = 0;}
			
			Yaw_Angle += g_z  * 0.00003f;		//0.001 ��ʱ����ָպ��ǽǶ�ֵ
		}
			
			
		
			TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
	}
	
}