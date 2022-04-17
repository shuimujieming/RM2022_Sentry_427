#include "gimbal_task.h"

TaskHandle_t GIMBAL_Task_Handler;
void gimbal_task(void *p_arg);
Gimbal_Mode_Enum Gimbal_Mode;

void Gimbal_Stop_Control();

void Gimbal_Follow_Control();

void Gimbal_AutoAim_Control();

void Gimbal_Keep_Control();

/*
*@title����̨���񴴽�
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void gimbal_task_create()
{
		xTaskCreate((TaskFunction_t )gimbal_task,          //������
							(const char*    )"gimbal_task",          //��������
							(uint16_t       )GIMBAL_STK_SIZE,        //�����ջ��С
							(void*          )NULL,                //���ݸ��������Ĳ���
							(UBaseType_t    )GIMBAL_TASK_PRIO,       //�������ȼ�
							(TaskHandle_t*  )&GIMBAL_Task_Handler);  //������  
}


/*
1.�涯ģʽ
2.����ģʽ
3.����ģʽ
4.����ģʽ
*/
/*
*@title����̨����
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void gimbal_task(void *p_arg)
{
	const TickType_t TimeIncrement=pdMS_TO_TICKS(5);
	TickType_t	PreviousWakeTime;
	PreviousWakeTime=xTaskGetTickCount();	
	
	while(1)
	{
		switch(Gimbal_Mode)
		{
			//��̨����ģʽ
			case GIMBAL_MODE_FOLLOW:
				Gimbal_Follow_Control();
				break;
			//��̨����ģʽ
			case GIMBAL_MODE_AUTOAIM:
				Gimbal_AutoAim_Control();
				break;
			//��̨����ģʽ
			case GIMBAL_MODE_STOP:
				Gimbal_Stop_Control();
				break;
			//��̨����ģʽ
			case GIMBAL_MODE_KEEP:
				Gimbal_Keep_Control();
				break;
			default:
				break;
		}
		
		xEventGroupSetBits(EventGroupHandler,GIMBAL_SIGNAL);	//��̨�¼�����λ
		vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);	
	}

}

float Yaw_Aim_Angle = 6114;
float Yaw_Angle_Max = 8256;
float Yaw_Angle_Min = 4000;

float Pitch_Aim_Angle = 10380;
float Pitch_Angle_Max = 10800;
float Pitch_Angle_Min = 10000;

//��̨�������
/*
*@Description����̨�������
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Gimbal_Follow_Control()
{
		Pitch_Aim_Angle -= DBUS.RC.ch3 / 50.0f;
	
		if(Pitch_Aim_Angle > Pitch_Angle_Max)
		{Pitch_Aim_Angle = Pitch_Angle_Max;}
		else if(Pitch_Aim_Angle < Pitch_Angle_Min)
		{Pitch_Aim_Angle = Pitch_Angle_Min;}			
			
		PID_Gimbal_Angle[1].PIDout = Pid_Calc(&PID_Gimbal_Angle[1],CAN_Gimbal[1].Current_MechAngle,Pitch_Aim_Angle);
		CAN_Gimbal[1].Target_Current = Pid_Calc(&PID_Gimbal_Speed[1],CAN_Gimbal[1].Current_Speed,PID_Gimbal_Angle[1].PIDout);
		
		
		Yaw_Aim_Angle -=DBUS.RC.ch2/ 50.0f;


		if(Yaw_Aim_Angle > Yaw_Angle_Max)
		{Yaw_Aim_Angle = Yaw_Angle_Max;}
		else if(Yaw_Aim_Angle < Yaw_Angle_Min)
		{Yaw_Aim_Angle = Yaw_Angle_Min;}			
		
		PID_Gimbal_Angle[0].PIDout = Pid_Calc(&PID_Gimbal_Angle[0],CAN_Gimbal[0].Current_MechAngle,Yaw_Aim_Angle);
		CAN_Gimbal[0].Target_Current = Pid_Calc(&PID_Gimbal_Speed[0],CAN_Gimbal[0].Current_Speed,PID_Gimbal_Angle[0].PIDout);
}
extern NUC_Typedef NUC_Data;
float Pitch_Speed = 10.0;
float Yaw_Speed = 10.0;
//��̨�������
/*
*@Description����̨�������
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Gimbal_AutoAim_Control()
{
	
	
	//�Ӿ�ʶ��Ŀ������� ��������
	if(NUC_Data.Armor_Type != 0)
	{
		Pitch_Aim_Angle = CAN_Gimbal[1].Current_MechAngle;
		Pitch_Aim_Angle +=NUC_Data.Pitch_Angle;
	
	
		if(Pitch_Aim_Angle > Pitch_Angle_Max)
		{Pitch_Aim_Angle = Pitch_Angle_Max;}
		else if(Pitch_Aim_Angle < Pitch_Angle_Min)
		{Pitch_Aim_Angle = Pitch_Angle_Min;}			
			
		
		Yaw_Aim_Angle = CAN_Gimbal[0].Current_MechAngle;
		Yaw_Aim_Angle -=NUC_Data.Yaw_Angle;
		
		if(Yaw_Aim_Angle > Yaw_Angle_Max)
		{Yaw_Aim_Angle = Yaw_Angle_Max;}
		else if(Yaw_Aim_Angle < Yaw_Angle_Min)
		{Yaw_Aim_Angle = Yaw_Angle_Min;}				
	}
	
	//�Ӿ�δʶ��Ŀ��ʱ�Զ�ҡͷ
	else
	{
		Pitch_Aim_Angle += Pitch_Speed;
		if(Pitch_Aim_Angle > Pitch_Angle_Max || Pitch_Aim_Angle < Pitch_Angle_Min)
		{Pitch_Speed = -Pitch_Speed;}
		
		
		Yaw_Aim_Angle += Yaw_Speed;
		if(Yaw_Aim_Angle > Yaw_Angle_Max || Yaw_Aim_Angle < Yaw_Angle_Min)
		{Yaw_Speed = -Yaw_Speed;}		
		
	}

		PID_Gimbal_Angle[1].PIDout = Pid_Calc(&PID_Gimbal_Angle[1],CAN_Gimbal[1].Current_MechAngle,Pitch_Aim_Angle);
		CAN_Gimbal[1].Target_Current = Pid_Calc(&PID_Gimbal_Speed[1],CAN_Gimbal[1].Current_Speed,PID_Gimbal_Angle[1].PIDout);		
		
		PID_Gimbal_Angle[0].PIDout = Pid_Calc(&PID_Gimbal_Angle[0],CAN_Gimbal[0].Current_MechAngle,Yaw_Aim_Angle);
		CAN_Gimbal[0].Target_Current = Pid_Calc(&PID_Gimbal_Speed[0],CAN_Gimbal[0].Current_Speed,PID_Gimbal_Angle[0].PIDout);
}

/*
*@Description����̨�������
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Gimbal_Keep_Control()
{
		if(Pitch_Aim_Angle > Pitch_Angle_Max)
		{Pitch_Aim_Angle = Pitch_Angle_Max;}
		else if(Pitch_Aim_Angle < Pitch_Angle_Min)
		{Pitch_Aim_Angle = Pitch_Angle_Min;}			
			
		PID_Gimbal_Angle[1].PIDout = Pid_Calc(&PID_Gimbal_Angle[1],CAN_Gimbal[1].Current_MechAngle,Pitch_Aim_Angle);
		CAN_Gimbal[1].Target_Current = Pid_Calc(&PID_Gimbal_Speed[1],CAN_Gimbal[1].Current_Speed,PID_Gimbal_Angle[1].PIDout);
		
		


		if(Yaw_Aim_Angle > Yaw_Angle_Max)
		{Yaw_Aim_Angle = Yaw_Angle_Max;}
		else if(Yaw_Aim_Angle < Yaw_Angle_Min)
		{Yaw_Aim_Angle = Yaw_Angle_Min;}			
		
		PID_Gimbal_Angle[0].PIDout = Pid_Calc(&PID_Gimbal_Angle[0],CAN_Gimbal[0].Current_MechAngle,Yaw_Aim_Angle);
		CAN_Gimbal[0].Target_Current = Pid_Calc(&PID_Gimbal_Speed[0],CAN_Gimbal[0].Current_Speed,PID_Gimbal_Angle[0].PIDout);
}
/*
*@Description����̨�������� 
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Gimbal_Stop_Control()
{
	CAN_Gimbal[0].Target_Current = 0;
	CAN_Gimbal[1].Target_Current = 0;
}