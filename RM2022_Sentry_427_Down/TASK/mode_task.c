#include "mode_task.h"

void Get_Gimbal_Mode();
void Get_Chassis_RC();
void Get_Shoot_Mode();
void Get_Cover_Mode();


TaskHandle_t MODE_Task_Handler;
void mode_task(void *p_arg);

/*
*@title��ģʽ״̬�������񴴽�
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void mode_task_create()
{
		xTaskCreate((TaskFunction_t )mode_task,          //������
							(const char*    )"mode_task",          //��������
							(uint16_t       )MODE_STK_SIZE,        //�����ջ��С
							(void*          )NULL,                //���ݸ��������Ĳ���
							(UBaseType_t    )MODE_TASK_PRIO,       //�������ȼ�
							(TaskHandle_t*  )&MODE_Task_Handler);  //������  
}

//ң����˥���ź�
extern int DR16_Signal;

extern Gimbal_Mode_Enum Gimbal_Mode;
extern Shoot_Mode_Enum Shoot_Mode;
extern Cover_Mode_Enum Cover_Mode;

int Cover_Open_Flag = 0; //����
int Shoot_Run_Flag = 0; //Ħ���ֿ���״̬��־λ
int Chassis_Rotate_Flag = 0;

/*
*@title��ģʽ״̬��������
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void mode_task(void *p_arg)
{
	const TickType_t TimeIncrement=pdMS_TO_TICKS(2);
	TickType_t	PreviousWakeTime;
	PreviousWakeTime=xTaskGetTickCount();	
	
	while(1)
	{
		
		//������ģʽ״̬����
		Get_Gimbal_Mode();
		Get_Shoot_Mode();
		Get_Cover_Mode();
		
		//�ź����ݼ����ڴ�֮ǰ��ֵӦ�ò�Ϊ0,������Ϊң�����ϵ�
		DR16_Signal --;
		if(DR16_Signal <= 0)
		{
			Gimbal_Mode = GIMBAL_MODE_STOP;
			Shoot_Mode = SHOOT_MODE_STOP;
			DR16_Signal = 0;
		}
		
		vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);	
	}

}

/*
*@Description����ȡ��̨ģʽ
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Get_Gimbal_Mode()
{
	//��̨ģʽ��ȡ
	//��������
	if(DBUS.RC.Switch_Left == RC_SW_UP && DBUS.RC.Switch_Right == RC_SW_DOWN)
	{
		Gimbal_Mode = GIMBAL_MODE_FOLLOW;
	}
	
	//��������
	else if(DBUS.RC.Switch_Left == RC_SW_MID && DBUS.RC.Switch_Right == RC_SW_DOWN)
	{
		Gimbal_Mode = GIMBAL_MODE_AUTOAIM;
	}
	
	else if(DBUS.RC.Switch_Left == RC_SW_MID && DBUS.RC.Switch_Right == RC_SW_MID)
	{
		Gimbal_Mode = GIMBAL_MODE_AUTOAIM;
	}
	else
	{
		Gimbal_Mode = GIMBAL_MODE_KEEP;
	}
	
	//��������
	if(DBUS.RC.Switch_Left == RC_SW_DOWN)
	{
		Gimbal_Mode = GIMBAL_MODE_STOP;
	}
	
	
	
}



int Shoot_Ready_Flag = 0; //ң����
/*
*@Description����ȡ����ģʽ
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Get_Shoot_Mode()
{
	
		if(DBUS.RC.Switch_Right == RC_SW_DOWN)
{
	//���ֿ��Ƶ��ָǣ����²���������һ��λ�ú���һ��״̬
	if(DBUS.RC.ch4 < -600)
	{
		Shoot_Ready_Flag = 1;
	}
	if(Shoot_Ready_Flag == 1)
	{
		//�������ӽ��м�λ��
		if(DBUS.RC.ch4 > -100 && DBUS.RC.ch4 < 0)
		{
			
			if(Shoot_Mode == SHOOT_MODE_STOP)
			{
				Shoot_Mode = SHOOT_MODE_RUN;
			}
			else if(Shoot_Mode == SHOOT_MODE_RUN)
			{
				Shoot_Mode = SHOOT_MODE_STOP;
			}
			Shoot_Ready_Flag = 0;
		}
	}
}
	if(DBUS.RC.Switch_Left == RC_SW_DOWN)
	{
		Shoot_Mode = SHOOT_MODE_STOP;
	}
	
}



//���ָǷ�ת��־λ
int Cover_Ready_Flag = 0; //ң����
extern NUC_Typedef NUC_Data;

int cover_value = 1000;

/*
*@Description����ȡ���ָ�ģʽ
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Get_Cover_Mode()
{
	if(DBUS.RC.Switch_Right == RC_SW_DOWN)
{
	//���ֿ��Ƶ��ָǣ����²���������һ��λ�ú���һ��״̬
	if(DBUS.RC.ch4 > 600)
	{
		Cover_Ready_Flag = 1;
	}
	if(Cover_Ready_Flag == 1)
	{
		//�������ӽ��м�λ��
		if(DBUS.RC.ch4 < 100 && DBUS.RC.ch4 > 0)
		{
			if(Cover_Mode == COVER_MODE_CLOSE)
			{
				Cover_Mode = COVER_MODE_OPEN;
			}
			else if(Cover_Mode == COVER_MODE_OPEN)
			{
				Cover_Mode = COVER_MODE_CLOSE;
			}		
			Cover_Ready_Flag = 0;
		}
	}
	
}
	
	//����ģʽ���Զ�����
	if(Gimbal_Mode == GIMBAL_MODE_AUTOAIM)
	{
		if(NUC_Data.NUC_Shoot_Allow_Flag == 1)
		{
			Cover_Mode = COVER_MODE_OPEN;
		}		
		else
		{
			Cover_Mode = COVER_MODE_CLOSE;			
		}
	}
	
	if(Shoot_Mode == SHOOT_MODE_STOP)
	{
		TIM4->CCR4 = cover_value;
		Cover_Mode = COVER_MODE_CLOSE;
	}
	
}

