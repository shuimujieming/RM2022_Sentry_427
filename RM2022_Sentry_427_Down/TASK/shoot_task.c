#include "shoot_task.h"

Shoot_Mode_Enum Shoot_Mode;

void Shoot_Stop_Control();
void Shoot_Run_Control();

TaskHandle_t SHOOT_Task_Handler;
void shoot_task(void *p_arg);

/*
*@title���������񴴽�
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void shoot_task_create()
{
		xTaskCreate((TaskFunction_t )shoot_task,          //������
							(const char*    )"shoot_task",          //��������
							(uint16_t       )SHOOT_STK_SIZE,        //�����ջ��С
							(void*          )NULL,                //���ݸ��������Ĳ���
							(UBaseType_t    )SHOOT_TASK_PRIO,       //�������ȼ�
							(TaskHandle_t*  )&SHOOT_Task_Handler);  //������  
}
int freq = 0;
/*
*@Description����ȡ����Ƶ��
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
int Get_Freq()
{
	static uint32_t time_last;
	
	freq = imu_tick_counts - time_last;
	time_last = imu_tick_counts;
	
	return freq;
}
UBaseType_t imu_stack_surplus;

Cover_Mode_Enum Cover_Mode;

/*
*@title����������
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void shoot_task(void *p_arg)
{
	const TickType_t TimeIncrement=pdMS_TO_TICKS(2);
	TickType_t	PreviousWakeTime;
	PreviousWakeTime=xTaskGetTickCount();	
	
	while(1)
	{
		switch(Shoot_Mode)
		{
			//����ֹͣ
			case SHOOT_MODE_STOP:
				Shoot_Stop_Control();
				break;
			//��������
			case SHOOT_MODE_RUN:
				Shoot_Run_Control();
				break;

			default:
				break;
		}
				
		//��ȡ����Ƶ��
		Get_Freq();
		
		//��ȡStack
		imu_stack_surplus = uxTaskGetStackHighWaterMark(NULL);

		xEventGroupSetBits(EventGroupHandler,SHOOT_SIGNAL);	//�����¼�����λ
		vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);	
	}

}
extern Down_To_Up_Typedef  Down_To_Up_Data;

/*
*@Description��������������
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Shoot_Stop_Control()
{
	//�ر���������ƵĲ�����
	Down_To_Up_Data.NUC_Shoot_Allow_Flag = 0;

	CAN_Shoot[0].Target_Current = Pid_Calc(&PID_Shoot[0],CAN_Shoot[0].Current_Speed,0);
	CAN_Shoot[1].Target_Current = Pid_Calc(&PID_Shoot[1],CAN_Shoot[1].Current_Speed,0);		
}

int Cover_Value = COVER_CLOSE_VAL;
extern Up_To_Down_Typedef  Up_To_Down_Data;
/*
*@Description���������п���
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Shoot_Run_Control()
{
		//���ݿ���ģʽȷ������Ƿ��
	if(Cover_Mode == COVER_MODE_CLOSE)
	{
		Cover_Value = COVER_CLOSE_VAL;
		Down_To_Up_Data.NUC_Shoot_Allow_Flag = 0;
	}
	else
	{
		Cover_Value = COVER_OPEN_VAL;		
		Down_To_Up_Data.NUC_Shoot_Allow_Flag = 1;
	}
	
	//�ϰ巢�����Ľ�ֹ�����־λ
	if(Up_To_Down_Data.Shoot_Allow_Flag == 0)
	{
		Cover_Value = COVER_CLOSE_VAL;		
	}
	
	TIM4->CCR4 = Cover_Value;

	CAN_Shoot[0].Target_Current = Pid_Calc(&PID_Shoot[0],CAN_Shoot[0].Current_Speed,-7200);
	CAN_Shoot[1].Target_Current = Pid_Calc(&PID_Shoot[1],CAN_Shoot[1].Current_Speed,7200);

}