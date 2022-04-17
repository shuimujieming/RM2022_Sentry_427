#include "shoot_task.h"

Shoot_Mode_Enum Shoot_Mode;

void Shoot_Stop_Control();
void Shoot_Run_Control();
void Shoot_Run_Down_Control();

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
		//����̨�������
		Shoot_Run_Down_Control();
		
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
/*
*@Description��������������
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Shoot_Stop_Control()
{
	
	CAN_Trigger[1].Target_Current = Pid_Calc(&PID_Trigger[1],CAN_Trigger[1].Current_Speed,0);

	CAN_Shoot[0].Target_Current = Pid_Calc(&PID_Shoot[0],CAN_Shoot[0].Current_Speed,0);
	CAN_Shoot[1].Target_Current = Pid_Calc(&PID_Shoot[1],CAN_Shoot[1].Current_Speed,0);		
}


void Bullet_Block_Up_Control();
int Shoot_Speed_Up = 0;

extern NUC_Typedef NUC_Data;

int Cover_Value = COVER_CLOSE_VAL;
extern Down_To_Up_Typedef  Down_To_Up_Data;
extern Up_To_Down_Typedef  Up_To_Down_Data;

int shoot_speed_test = 9000;

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
		Shoot_Speed_Up = 0;			
	}
	else
	{
		Cover_Value = COVER_OPEN_VAL;	
		Shoot_Speed_Up = 2000;	
	}

	//���俨������
	//�Ϲ���
	Bullet_Block_Up_Control();
	
	//�������ƣ���ֹ���������������ŵ�����������֤����Ƶ�²�������
//	if(game_robot_state.shooter_id1_17mm_cooling_limit < power_heat_data.shooter_id1_17mm_cooling_heat + 30)
//	{
//		Cover_Value = COVER_CLOSE_VAL;
//		Shoot_Speed_Up = 0;
//		//�رն��
//	}

	//�������
	//����д������Ϊ�˷�ֹ������Ƶ�PWMƵ������
	TIM4->CCR4 = Cover_Value;
	

	//�Ϲ���
	CAN_Trigger[1].Target_Current = Pid_Calc(&PID_Trigger[1],CAN_Trigger[1].Current_Speed,Shoot_Speed_Up);							

	
	CAN_Shoot[0].Target_Current = Pid_Calc(&PID_Shoot[0],CAN_Shoot[0].Current_Speed,shoot_speed_test);
	CAN_Shoot[1].Target_Current = Pid_Calc(&PID_Shoot[1],CAN_Shoot[1].Current_Speed,-shoot_speed_test);
	
	if(DBUS.RC.Switch_Left == RC_SW_DOWN)
	{
		CAN_Trigger[1].Target_Current = 0;	
	}

}

void Bullet_Block_Down_Control();
int Shoot_Speed_Down = 0;

/*
*@Description������̨�������
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Shoot_Run_Down_Control()
{
		
	//����̨Ҫ����
	if(Down_To_Up_Data.NUC_Shoot_Allow_Flag == 1)
	{
		Shoot_Speed_Down = 4000;	
	}
	else
	{
		Shoot_Speed_Down = 0;			
	}	
	//���俨������
	//�¹���
	Bullet_Block_Down_Control();
	
	//�������ƣ���ֹ���������������ŵ�����������֤����Ƶ�²�������
//	if(game_robot_state.shooter_id2_17mm_cooling_limit < power_heat_data.shooter_id2_17mm_cooling_heat + 30)
//	{
//		Shoot_Speed_Down = 0;
//		//�����°����ڽ�ֹ����
//		Up_To_Down_Data.Shoot_Allow_Flag = 0;
//	}	
//	else
//	{
//		//������
//		Up_To_Down_Data.Shoot_Allow_Flag = 1;
//	}
	
	//�¹���
	CAN_Trigger[0].Target_Current = Pid_Calc(&PID_Trigger[0],CAN_Trigger[0].Current_Speed,Shoot_Speed_Down);	
	
	if(DBUS.RC.Switch_Left == RC_SW_DOWN)
	{
		CAN_Trigger[0].Target_Current = 0;	
	}
}

//��ת���ڼ�ʱ
int Block_Time_Up = 0;
//��תʱ���ʱ
int Block_Reverse_Time_Up = 0;

//�Ϲ���������
void Bullet_Block_Up_Control()
{
	//�����������ʱ��ⷴת
	if(CAN_Trigger[1].Current > 10000)
	{
		Block_Time_Up ++;
	}
	
	//��תʱ��10*�����������ڣ�2ms��= 20ms
	if(Block_Time_Up == 10)
	{
		Block_Reverse_Time_Up = 1;
	}
	
	//��ת
	if(Block_Reverse_Time_Up >= 1)
	{
		Block_Reverse_Time_Up++;
		
		//��תʱ�� 3 * 2ms = 6ms
		if(Block_Reverse_Time_Up > 3)
		{
			Block_Reverse_Time_Up = 0;
			Block_Time_Up = 0;
		}
		else
		{
			//��תʱ������Сʱ������ת����ֹ��ת��ת
			if(CAN_Trigger[1].Current > -4000)
			{Shoot_Speed_Up = -2000;}
			//�����ϴ�ָ���ת
			else
			{
				Block_Reverse_Time_Up = 0;
				Block_Time_Up = 0;
			}
		}
	}
	
}


//��ת���ڼ�ʱ
int Block_Time_Down = 0;
//��תʱ���ʱ
int Block_Reverse_Time_Down = 0;

//�¹���������
void Bullet_Block_Down_Control()
{
	//�����������ʱ��ⷴת
	if(CAN_Trigger[0].Current > 10000)
	{
		Block_Time_Down ++;
	}
	
	//��תʱ��10*�����������ڣ�2ms��= 20ms
	if(Block_Time_Down == 10)
	{
		Block_Reverse_Time_Down = 1;
	}
	
	//��ת
	if(Block_Reverse_Time_Down >= 1)
	{
		Block_Reverse_Time_Down++;
		
		//��תʱ�� 3 * 2ms = 6ms
		if(Block_Reverse_Time_Down > 3)
		{
			Block_Reverse_Time_Down = 0;
			Block_Time_Down = 0;
		}
		else
		{
			//��תʱ������Сʱ������ת����ֹ��ת��ת
			if(CAN_Trigger[0].Current > -4000)
			{Shoot_Speed_Down = -2000;}
			//�����ϴ�ָ���ת
			else
			{
				Block_Reverse_Time_Down = 0;
				Block_Time_Down = 0;
			}
		}
	}
	
}