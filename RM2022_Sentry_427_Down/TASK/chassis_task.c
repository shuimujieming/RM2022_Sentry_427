#include "chassis_task.h"
void Chassis_Speed_Calc(float vx,float vy,float vw);
void Chassis_Follow_Control();
void Chassis_Stop_Control();
void Chassis_Rotate_Control();

TaskHandle_t CHASSIS_Task_Handler;
void chassis_task(void *p_arg);

Chassis_Speed_Typedef Chassis_Speed;
Chassis_Mode_Enum Chassis_Mode;
Chassis_Control_Speed_Typedef rc;
Chassis_Control_Speed_Typedef keyboard;	
/*
*@title���������񴴽�
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void chassis_task_create()
{
		xTaskCreate((TaskFunction_t )chassis_task,          //������
							(const char*    )"chassis_task",          //��������
							(uint16_t       )CHASSIS_STK_SIZE,        //�����ջ��С
							(void*          )NULL,                //���ݸ��������Ĳ���
							(UBaseType_t    )CHASSIS_TASK_PRIO,       //�������ȼ�
							(TaskHandle_t*  )&CHASSIS_Task_Handler);  //������  
}

/*
1.�涯ģʽ
2.����ģʽ
3.����ģʽ
4.����ģʽ
*/

/*
*@title����������
*@description������3ms
*@param 1��	
*@param 2��	
*@return:��	
*/
void chassis_task(void *p_arg)
{
	
	const TickType_t TimeIncrement=pdMS_TO_TICKS(3);
	TickType_t	PreviousWakeTime;
	PreviousWakeTime=xTaskGetTickCount();	
	
	while(1)
	{
		switch(Chassis_Mode)
		{
			//����ģʽ
			case CHASSIS_MODE_FOLLOW:
				Chassis_Follow_Control();
				break;
			//����ģʽ
			case CHASSIS_MODE_ROTATE:
				Chassis_Rotate_Control();
				break;
			//����ģʽ
			case CHASSIS_MODE_STOP:
				Chassis_Stop_Control();
				break;
			//���ģʽ
			case CHASSIS_MODE_RUNE:
				Chassis_Stop_Control();
				break;
			default:
				break;
		}
		
		//�����ٶȽ���
		Chassis_Speed_Calc(Chassis_Speed.vx,Chassis_Speed.vy,Chassis_Speed.vw);
		
//		//���̹��ʿ���
//		if(power_heat_data.chassis_power_buffer < 60)
//		{
//			for(int i =0;i<4;i++)
//			{
//				Chassis_Speed.wheel_speed[i] *= power_heat_data.chassis_power_buffer/60.0f;
//			}
//		}
		
		
		//���Ŀ�������ֵ
		for(int i = 0;i<4;i++)
		{
			CAN_Chassis[i].Target_Current = Pid_Calc(&PID_Chassis[i], CAN_Chassis[i].Current_Speed, Chassis_Speed.wheel_speed[i]);		
		}
			
		//CAN���ݸ��·���
		xEventGroupSetBits(EventGroupHandler,CHASSIS_SIGNAL);	//�����¼�����λ		
		vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);	
	}

}

//��һ��ֵ�Ƕ�
short Angle_Last = 10226;
//��ʼ�Ƕ�
short Init_Angle = 10226;
//�ٶȷ����־λ
int Init_Dir = 1;

extern void Encoder_Data_Process(CAN_Data_TypeDef *encoder_data,short init_angle);
/*
*@Description�����̸������
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Chassis_Follow_Control()
{
	//ս���ٶȷ�����ֵ
	Chassis_Speed.vx = rc.vx + keyboard.vx;
	Chassis_Speed.vy = rc.vy + keyboard.vy;
	Chassis_Speed.vw = Pid_Calc(&PID_Chassis_Omega,(CAN_Gimbal[0].Current_MechAngle - Init_Angle)/8192.0*360.0f, 0); //��̨Yaw����Խ�PID �����ת�ٶȷ���
	
	/*
		ʵ�ָ���״̬ʱ��С��תʱ�䣬��ͷβ����
	*/
{
	//�Ƕ�����
	if(abs(CAN_Gimbal[0].Current_MechAngle - Angle_Last) > 4000)
	{
		
		if(CAN_Gimbal[0].Critical_MechAngle == YAW_ANGLE_INIT+4096)
		{
			//�������Ϊ��ǰ��
			Encoder_Data_Process(&CAN_Gimbal[0],YAW_ANGLE_INIT+4096);
			if(CAN_Gimbal[0].Origin_MechAngle < CAN_Gimbal[0].Critical_MechAngle)
			{
				CAN_Gimbal[0].Current_MechAngle = CAN_Gimbal[0].Origin_MechAngle + 8192;
			}
			else
			{
				CAN_Gimbal[0].Current_MechAngle = CAN_Gimbal[0].Origin_MechAngle;
			}			
			}
		else
		{
			//�������Ϊ��ǰ��
			Encoder_Data_Process(&CAN_Gimbal[0],YAW_ANGLE_INIT);
			if(CAN_Gimbal[0].Origin_MechAngle < CAN_Gimbal[0].Critical_MechAngle)
			{
				CAN_Gimbal[0].Current_MechAngle = CAN_Gimbal[0].Origin_MechAngle + 8192;
			}
			else
			{
				CAN_Gimbal[0].Current_MechAngle = CAN_Gimbal[0].Origin_MechAngle;
			}
		}
		
		Init_Angle = CAN_Gimbal[0].Current_MechAngle;
		Init_Dir =-Init_Dir;
	}
	
	Angle_Last = CAN_Gimbal[0].Current_MechAngle;
	
	Chassis_Speed.vx *= Init_Dir;
	Chassis_Speed.vy *= Init_Dir;
}


	
	//��ת�ٶ����������پ�ֹ������ϵ����
	if(fabs(Chassis_Speed.vw) < 200)
	Chassis_Speed.vw = 0;
	
}

#define ANGLE_TO_RAD 0.01745329251994329576923690768489f
/*
*@Description���������ݿ���
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Chassis_Rotate_Control()
{
	static double Rotate_Angle = 0;
	
	Rotate_Angle = (CAN_Gimbal[0].Current_MechAngle - 10226)/8192.0*360.0f;
	if(Rotate_Angle <= 0)
	{
		Rotate_Angle = Rotate_Angle + 360.0f;
	}
	
	Chassis_Speed.vx = -(rc.vy + keyboard.vy) * sin(Rotate_Angle * ANGLE_TO_RAD) + (rc.vx + keyboard.vx) * cos(Rotate_Angle * ANGLE_TO_RAD);
	Chassis_Speed.vy = (rc.vy + keyboard.vy) * cos(Rotate_Angle * ANGLE_TO_RAD) + (rc.vx + keyboard.vx) * sin(Rotate_Angle * ANGLE_TO_RAD);
	Chassis_Speed.vw = -3000;

	if(abs(CAN_Gimbal[0].Current_MechAngle - Angle_Last) > 4000)
	{
		
		if(CAN_Gimbal[0].Critical_MechAngle == YAW_ANGLE_INIT+4096)
		{
			//�������Ϊ��ǰ��
			Encoder_Data_Process(&CAN_Gimbal[0],YAW_ANGLE_INIT+4096);
			if(CAN_Gimbal[0].Origin_MechAngle < CAN_Gimbal[0].Critical_MechAngle)
			{
				CAN_Gimbal[0].Current_MechAngle = CAN_Gimbal[0].Origin_MechAngle + 8192;
			}
			else
			{
				CAN_Gimbal[0].Current_MechAngle = CAN_Gimbal[0].Origin_MechAngle;
			}			
			}
		else
		{
			//�������Ϊ��ǰ��
			Encoder_Data_Process(&CAN_Gimbal[0],YAW_ANGLE_INIT);
			if(CAN_Gimbal[0].Origin_MechAngle < CAN_Gimbal[0].Critical_MechAngle)
			{
				CAN_Gimbal[0].Current_MechAngle = CAN_Gimbal[0].Origin_MechAngle + 8192;
			}
			else
			{
				CAN_Gimbal[0].Current_MechAngle = CAN_Gimbal[0].Origin_MechAngle;
			}
		}
		
		Init_Angle = CAN_Gimbal[0].Current_MechAngle;
		Init_Dir =-Init_Dir;
	}

	Angle_Last = CAN_Gimbal[0].Current_MechAngle;
}
/*
*@Description��������������
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Chassis_Stop_Control()
{
	Chassis_Speed.vx = 0;
	Chassis_Speed.vy = 0;
	Chassis_Speed.vw = 0;
}

//vxΪǰ���˶��ٶȣ�vyΪ����ƽ���˶��ٶȣ�vwΪ��ת�ٶ�
/*
*@Description�������ٶȽ��㣨���֣�
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Chassis_Speed_Calc(float vx,float vy,float vw)
{
	float Speed_Max = 0;
	
	Chassis_Speed.wheel_speed[0] = +vx - vy + vw;
	Chassis_Speed.wheel_speed[1] = -vx - vy + vw;
	Chassis_Speed.wheel_speed[2] = -vx + vy + vw;
	Chassis_Speed.wheel_speed[3] = +vx + vy + vw;
	
	//�ٶ��޷�����
	//���ֵѰ��
	for(int i = 0;i<4;i++)
	{
		if(abs(Chassis_Speed.wheel_speed[i]) > Speed_Max)
		{
			Speed_Max = abs(Chassis_Speed.wheel_speed[i]);
		}
	}
	//�ȱ����µ��ٶ�
	if(Speed_Max > 8500)
	{
		for(int i = 0;i<4;i++)
		{
			Chassis_Speed.wheel_speed[i] *= 8500/Speed_Max;
		}
	}
	
}