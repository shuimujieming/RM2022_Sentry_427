#include "main.h"

/*********************************************
Copyright(C) ˮľ��Ming
All rights reserved
File name:	�������ļ�
Author:		shuimujieming
Version:		1.0
Description:	
Others:		
Log:	
*********************************************/

/*
*@Description���ײ��豸��ʼ��
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void BSP_Init()
{
	Power_Init();				  //��ʼ����Դ
	
	LED_Init();		        //��ʼ��LED�˿�

	Laser_Init();					//�����ʼ��
	
	DR16_Init();					//ң�������ջ���ʼ��
	
	CAN12_Init();					//CAN������ʼ��
	
	Referee_Init();				//����ϵͳ��ʼ��

	IMU_Init();						//IMU��ʼ��

	TIM6_Init(1000);			//��ʱ��6��ʼ��

	Gimbal_Init();				//��̨��ʼ��
	
	Chassis_Init();				//���̳�ʼ��
	
	Shoot_Init();					//�����ʼ��
	
	NUC_Init();						//NUC������ʼ��

	Dual_Board_Init();		//˫��ͨ�ų�ʼ��
	
	Cover_Init();					//������λ�ڵ���ʼ��
		
	Beep_Init();					//��������ʼ��
	
}

/*
*@Description��������
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
int main(void)
{ 
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	//�����ж����ȼ�����Ϊ
	
	delay_init(168);		  //��ʼ����ʱ����
	
	BSP_Init();						//�ײ�����������ʼ��
	
	Task_Init();					//����ϵͳ�����ʼ��

	while(1)
	{
		Beep_Ctrl(600,100);
	}
		
}





