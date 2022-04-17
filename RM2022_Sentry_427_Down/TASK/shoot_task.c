#include "shoot_task.h"

Shoot_Mode_Enum Shoot_Mode;

void Shoot_Stop_Control();
void Shoot_Run_Control();

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
extern Down_To_Up_Typedef  Down_To_Up_Data;

/*
*@Description：发射无力控制
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Shoot_Stop_Control()
{
	//关闭上主板控制的拨弹盘
	Down_To_Up_Data.NUC_Shoot_Allow_Flag = 0;

	CAN_Shoot[0].Target_Current = Pid_Calc(&PID_Shoot[0],CAN_Shoot[0].Current_Speed,0);
	CAN_Shoot[1].Target_Current = Pid_Calc(&PID_Shoot[1],CAN_Shoot[1].Current_Speed,0);		
}

int Cover_Value = COVER_CLOSE_VAL;
extern Up_To_Down_Typedef  Up_To_Down_Data;
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
		Down_To_Up_Data.NUC_Shoot_Allow_Flag = 0;
	}
	else
	{
		Cover_Value = COVER_OPEN_VAL;		
		Down_To_Up_Data.NUC_Shoot_Allow_Flag = 1;
	}
	
	//上板发送来的禁止发射标志位
	if(Up_To_Down_Data.Shoot_Allow_Flag == 0)
	{
		Cover_Value = COVER_CLOSE_VAL;		
	}
	
	TIM4->CCR4 = Cover_Value;

	CAN_Shoot[0].Target_Current = Pid_Calc(&PID_Shoot[0],CAN_Shoot[0].Current_Speed,-7200);
	CAN_Shoot[1].Target_Current = Pid_Calc(&PID_Shoot[1],CAN_Shoot[1].Current_Speed,7200);

}