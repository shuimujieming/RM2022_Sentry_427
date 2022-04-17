#include "imu_task.h"


TaskHandle_t IMU_Task_Handler;
void imu_task(void *p_arg);

/*
*@title��IMU���񴴽�
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void imu_task_create()
{
		xTaskCreate((TaskFunction_t )imu_task,          //������
							(const char*    )"imu_task",          //��������
							(uint16_t       )IMU_STK_SIZE,        //�����ջ��С
							(void*          )NULL,                //���ݸ��������Ĳ���
							(UBaseType_t    )IMU_TASK_PRIO,       //�������ȼ�
							(TaskHandle_t*  )&IMU_Task_Handler);  //������  
}


/*
*@title��IMU����
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void imu_task(void *p_arg)
{
	const TickType_t TimeIncrement=pdMS_TO_TICKS(1);
	TickType_t	PreviousWakeTime;
	PreviousWakeTime=xTaskGetTickCount();	
	
	while(1)
	{
		
		IMU_Get_Data();//��ȡ����������
		IMU_Ahrs_Update();//��̬����
		IMU_Attitude_Update();//ŷ����ת��
		
		vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);	
	}

}