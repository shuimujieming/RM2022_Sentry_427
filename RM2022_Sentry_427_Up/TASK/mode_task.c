#include "mode_task.h"

void Get_Chassis_Mode();
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


extern Chassis_Mode_Enum Chassis_Mode;
extern Gimbal_Mode_Enum Gimbal_Mode;
extern Shoot_Mode_Enum Shoot_Mode;
extern Cover_Mode_Enum Cover_Mode;

extern Chassis_Control_Speed_Typedef rc;
extern Chassis_Control_Speed_Typedef keyboard;	


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
		//�����ٶȻ�ȡ
		Get_Chassis_RC();		
		
		//������ģʽ״̬����
		Get_Gimbal_Mode();
		Get_Chassis_Mode();
		Get_Shoot_Mode();
		Get_Cover_Mode();
		
		//�ź����ݼ����ڴ�֮ǰ��ֵӦ�ò�Ϊ0,������Ϊң�����ϵ�
		DR16_Signal --;
		if(DR16_Signal <= 0)
		{
			Chassis_Mode = CHASSIS_MODE_STOP;
			Gimbal_Mode = GIMBAL_MODE_STOP;
			Shoot_Mode = SHOOT_MODE_STOP;
			DR16_Signal = 0;
		}
		
		vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);	
	}

}
//ң������ȡ�����ٶ�
void Get_Chassis_RC()
{
	//�ֶ���������ٶ�
	rc.vx = 0;
	rc.vy = DBUS.RC.ch0 / 660.0f * 10000.0f;
	rc.vw = 0;
	
}
//����״̬��־λ
typedef enum
{
	CHASSIS_STATUS_STOP = 0,
	CHASSIS_STATUS_ROTATE,
	CHASSIS_STATUS_FOLLOW,

}Chassis_Status_Enum;
//����״̬�������ã�
Chassis_Status_Enum Chassis_Status = CHASSIS_STATUS_FOLLOW;

/*
*@Description����ȡ����ģʽ
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Get_Chassis_Mode()
{	
	//ң��������̵���ģʽ����
	if(DBUS.RC.Switch_Left == RC_SW_UP )
	{
		Chassis_Mode = CHASSIS_MODE_FOLLOW;
	}
	if(DBUS.RC.Switch_Left == RC_SW_MID && DBUS.RC.Switch_Right == RC_SW_MID)
	{
		Chassis_Mode = CHASSIS_MODE_AUTO;
	}
	//˫�µ����ٶ�Ϊ0
	if(DBUS.RC.Switch_Left == RC_SW_DOWN)
	{
		Chassis_Mode = CHASSIS_MODE_STOP;
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
	//��������ʱ����̨��ң��������
	if(DBUS.RC.Switch_Left == RC_SW_UP && DBUS.RC.Switch_Right == RC_SW_UP)
	{
		Gimbal_Mode = GIMBAL_MODE_FOLLOW;
	}
	
	//�������ϣ�����̨����ģʽ
	else if(DBUS.RC.Switch_Left == RC_SW_MID && DBUS.RC.Switch_Right == RC_SW_UP)
	{
		Gimbal_Mode = GIMBAL_MODE_AUTOAIM;
	}
	
	//�������У�����̨����ģʽ	
	else if(DBUS.RC.Switch_Left == RC_SW_MID && DBUS.RC.Switch_Right == RC_SW_MID)
	{
		Gimbal_Mode = GIMBAL_MODE_AUTOAIM;
	}
	
	//������λ����̨Ϊ����ģʽ
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
	
	 //����̨���Ҳ���λ���ϱ߲���Ч
		if(DBUS.RC.Switch_Right == RC_SW_UP)
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

//����ֹͣ
	if(DBUS.RC.Switch_Left == RC_SW_DOWN)
{
	Shoot_Mode = SHOOT_MODE_STOP;
}
	

}

extern NUC_Typedef NUC_Data;

//���ָǷ�ת��־λ
int Cover_Ready_Flag = 0; //ң����
/*
*@Description����ȡ���ָ�ģʽ
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Get_Cover_Mode()
{
	 //����̨���Ҳ���λ���ϱ߲���Ч
	if(DBUS.RC.Switch_Right == RC_SW_UP)
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

	
	//Ħ���ֹ�����λ���Ҳ��
	if(Shoot_Mode == SHOOT_MODE_STOP)
	{
		TIM4->CCR4 = COVER_CLOSE_VAL;
		Cover_Mode = COVER_MODE_CLOSE;
	}
	
}