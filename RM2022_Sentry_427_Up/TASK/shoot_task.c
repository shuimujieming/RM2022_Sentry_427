#include "shoot_task.h"

Shoot_Mode_Enum Shoot_Mode;

void Shoot_Stop_Control();
void Shoot_Run_Control();
void Shoot_Run_Down_Control();

TaskHandle_t SHOOT_Task_Handler;
void shoot_task(void *p_arg);

/*
*@title：发射任务创建
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void shoot_task_create()
{
		xTaskCreate((TaskFunction_t )shoot_task,          //任务函数
							(const char*    )"shoot_task",          //任务名称
							(uint16_t       )SHOOT_STK_SIZE,        //任务堆栈大小
							(void*          )NULL,                //传递给任务函数的参数
							(UBaseType_t    )SHOOT_TASK_PRIO,       //任务优先级
							(TaskHandle_t*  )&SHOOT_Task_Handler);  //任务句柄  
}
int freq = 0;
/*
*@Description：获取运行频率
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
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
*@title：发射任务
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void shoot_task(void *p_arg)
{
	const TickType_t TimeIncrement=pdMS_TO_TICKS(2);
	TickType_t	PreviousWakeTime;
	PreviousWakeTime=xTaskGetTickCount();	
	
	while(1)
	{
		//下云台发射控制
		Shoot_Run_Down_Control();
		
		switch(Shoot_Mode)
		{
			//发射停止
			case SHOOT_MODE_STOP:
				Shoot_Stop_Control();
				break;
			//发射运行
			case SHOOT_MODE_RUN:
				Shoot_Run_Control();
				break;

			default:
				break;
		}
				
		//获取运行频率
		Get_Freq();
		
		//获取Stack
		imu_stack_surplus = uxTaskGetStackHighWaterMark(NULL);

		xEventGroupSetBits(EventGroupHandler,SHOOT_SIGNAL);	//发射事件组置位
		vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);	
	}

}
/*
*@Description：发射无力控制
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
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
*@Description：发射运行控制
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Shoot_Run_Control()
{
	//根据控制模式确定舵机是否打开
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

	//发射卡弹控制
	//上供弹
	Bullet_Block_Up_Control();
	
	//热量限制（防止超热量）留出三颗弹丸余量，保证高射频下不超热量
//	if(game_robot_state.shooter_id1_17mm_cooling_limit < power_heat_data.shooter_id1_17mm_cooling_heat + 30)
//	{
//		Cover_Value = COVER_CLOSE_VAL;
//		Shoot_Speed_Up = 0;
//		//关闭舵机
//	}

	//舵机控制
	//单独写这句控制为了防止舵机控制的PWM频繁跳动
	TIM4->CCR4 = Cover_Value;
	

	//上供弹
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
*@Description：下云台发射控制
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Shoot_Run_Down_Control()
{
		
	//下云台要求发射
	if(Down_To_Up_Data.NUC_Shoot_Allow_Flag == 1)
	{
		Shoot_Speed_Down = 4000;	
	}
	else
	{
		Shoot_Speed_Down = 0;			
	}	
	//发射卡弹控制
	//下供弹
	Bullet_Block_Down_Control();
	
	//热量限制（防止超热量）留出三颗弹丸余量，保证高射频下不超热量
//	if(game_robot_state.shooter_id2_17mm_cooling_limit < power_heat_data.shooter_id2_17mm_cooling_heat + 30)
//	{
//		Shoot_Speed_Down = 0;
//		//告诉下板现在禁止发射
//		Up_To_Down_Data.Shoot_Allow_Flag = 0;
//	}	
//	else
//	{
//		//允许发射
//		Up_To_Down_Data.Shoot_Allow_Flag = 1;
//	}
	
	//下供弹
	CAN_Trigger[0].Target_Current = Pid_Calc(&PID_Trigger[0],CAN_Trigger[0].Current_Speed,Shoot_Speed_Down);	
	
	if(DBUS.RC.Switch_Left == RC_SW_DOWN)
	{
		CAN_Trigger[0].Target_Current = 0;	
	}
}

//堵转周期计时
int Block_Time_Up = 0;
//反转时间计时
int Block_Reverse_Time_Up = 0;

//上供弹拨弹盘
void Bullet_Block_Up_Control()
{
	//电机电流过大时检测反转
	if(CAN_Trigger[1].Current > 10000)
	{
		Block_Time_Up ++;
	}
	
	//堵转时间10*发射任务周期（2ms）= 20ms
	if(Block_Time_Up == 10)
	{
		Block_Reverse_Time_Up = 1;
	}
	
	//反转
	if(Block_Reverse_Time_Up >= 1)
	{
		Block_Reverse_Time_Up++;
		
		//反转时间 3 * 2ms = 6ms
		if(Block_Reverse_Time_Up > 3)
		{
			Block_Reverse_Time_Up = 0;
			Block_Time_Up = 0;
		}
		else
		{
			//反转时电流较小时才允许反转，防止反转堵转
			if(CAN_Trigger[1].Current > -4000)
			{Shoot_Speed_Up = -2000;}
			//电流较大恢复正转
			else
			{
				Block_Reverse_Time_Up = 0;
				Block_Time_Up = 0;
			}
		}
	}
	
}


//堵转周期计时
int Block_Time_Down = 0;
//反转时间计时
int Block_Reverse_Time_Down = 0;

//下供弹拨弹盘
void Bullet_Block_Down_Control()
{
	//电机电流过大时检测反转
	if(CAN_Trigger[0].Current > 10000)
	{
		Block_Time_Down ++;
	}
	
	//堵转时间10*发射任务周期（2ms）= 20ms
	if(Block_Time_Down == 10)
	{
		Block_Reverse_Time_Down = 1;
	}
	
	//反转
	if(Block_Reverse_Time_Down >= 1)
	{
		Block_Reverse_Time_Down++;
		
		//反转时间 3 * 2ms = 6ms
		if(Block_Reverse_Time_Down > 3)
		{
			Block_Reverse_Time_Down = 0;
			Block_Time_Down = 0;
		}
		else
		{
			//反转时电流较小时才允许反转，防止反转堵转
			if(CAN_Trigger[0].Current > -4000)
			{Shoot_Speed_Down = -2000;}
			//电流较大恢复正转
			else
			{
				Block_Reverse_Time_Down = 0;
				Block_Time_Down = 0;
			}
		}
	}
	
}