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
*@title：底盘任务创建
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void chassis_task_create()
{
		xTaskCreate((TaskFunction_t )chassis_task,          //任务函数
							(const char*    )"chassis_task",          //任务名称
							(uint16_t       )CHASSIS_STK_SIZE,        //任务堆栈大小
							(void*          )NULL,                //传递给任务函数的参数
							(UBaseType_t    )CHASSIS_TASK_PRIO,       //任务优先级
							(TaskHandle_t*  )&CHASSIS_Task_Handler);  //任务句柄  
}

/*
1.随动模式
2.自瞄模式
3.陀螺模式
4.无力模式
*/

/*
*@title：底盘任务
*@description：周期3ms
*@param 1：	
*@param 2：	
*@return:：	
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
			//跟随模式
			case CHASSIS_MODE_FOLLOW:
				Chassis_Follow_Control();
				break;
			//陀螺模式
			case CHASSIS_MODE_ROTATE:
				Chassis_Rotate_Control();
				break;
			//无力模式
			case CHASSIS_MODE_STOP:
				Chassis_Stop_Control();
				break;
			//打符模式
			case CHASSIS_MODE_RUNE:
				Chassis_Stop_Control();
				break;
			default:
				break;
		}
		
		//底盘速度解算
		Chassis_Speed_Calc(Chassis_Speed.vx,Chassis_Speed.vy,Chassis_Speed.vw);
		
//		//底盘功率控制
//		if(power_heat_data.chassis_power_buffer < 60)
//		{
//			for(int i =0;i<4;i++)
//			{
//				Chassis_Speed.wheel_speed[i] *= power_heat_data.chassis_power_buffer/60.0f;
//			}
//		}
		
		
		//电机目标电流赋值
		for(int i = 0;i<4;i++)
		{
			CAN_Chassis[i].Target_Current = Pid_Calc(&PID_Chassis[i], CAN_Chassis[i].Current_Speed, Chassis_Speed.wheel_speed[i]);		
		}
			
		//CAN数据更新发送
		xEventGroupSetBits(EventGroupHandler,CHASSIS_SIGNAL);	//底盘事件组置位		
		vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);	
	}

}

//上一中值角度
short Angle_Last = 10226;
//初始角度
short Init_Angle = 10226;
//速度方向标志位
int Init_Dir = 1;

extern void Encoder_Data_Process(CAN_Data_TypeDef *encoder_data,short init_angle);
/*
*@Description：底盘跟随控制
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Chassis_Follow_Control()
{
	//战车速度分量赋值
	Chassis_Speed.vx = rc.vx + keyboard.vx;
	Chassis_Speed.vy = rc.vy + keyboard.vy;
	Chassis_Speed.vw = Pid_Calc(&PID_Chassis_Omega,(CAN_Gimbal[0].Current_MechAngle - Init_Angle)/8192.0*360.0f, 0); //云台Yaw轴相对角PID 输出旋转速度分量
	
	/*
		实现跟随状态时最小旋转时间，即头尾合用
	*/
{
	//角度跳变
	if(abs(CAN_Gimbal[0].Current_MechAngle - Angle_Last) > 4000)
	{
		
		if(CAN_Gimbal[0].Critical_MechAngle == YAW_ANGLE_INIT+4096)
		{
			//将反面改为正前方
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
			//将反面改为正前方
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


	
	//旋转速度死区，减少静止底盘轮系抖动
	if(fabs(Chassis_Speed.vw) < 200)
	Chassis_Speed.vw = 0;
	
}

#define ANGLE_TO_RAD 0.01745329251994329576923690768489f
/*
*@Description：底盘陀螺控制
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
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
			//将反面改为正前方
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
			//将反面改为正前方
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
*@Description：底盘无力控制
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Chassis_Stop_Control()
{
	Chassis_Speed.vx = 0;
	Chassis_Speed.vy = 0;
	Chassis_Speed.vw = 0;
}

//vx为前后运动速度，vy为左右平移运动速度，vw为旋转速度
/*
*@Description：底盘速度解算（麦轮）
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Chassis_Speed_Calc(float vx,float vy,float vw)
{
	float Speed_Max = 0;
	
	Chassis_Speed.wheel_speed[0] = +vx - vy + vw;
	Chassis_Speed.wheel_speed[1] = -vx - vy + vw;
	Chassis_Speed.wheel_speed[2] = -vx + vy + vw;
	Chassis_Speed.wheel_speed[3] = +vx + vy + vw;
	
	//速度限幅调整
	//最大值寻找
	for(int i = 0;i<4;i++)
	{
		if(abs(Chassis_Speed.wheel_speed[i]) > Speed_Max)
		{
			Speed_Max = abs(Chassis_Speed.wheel_speed[i]);
		}
	}
	//等比例下调速度
	if(Speed_Max > 8500)
	{
		for(int i = 0;i<4;i++)
		{
			Chassis_Speed.wheel_speed[i] *= 8500/Speed_Max;
		}
	}
	
}