#include "nuc_task.h"

TaskHandle_t NUC_Task_Handler;
void nuc_task(void *p_arg);

/*
*@title��NUC���񴴽�
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void nuc_task_create()
{
		xTaskCreate((TaskFunction_t )nuc_task,          //������
							(const char*    )"nuc_task",          //��������
							(uint16_t       )NUC_STK_SIZE,        //�����ջ��С
							(void*          )NULL,                //���ݸ��������Ĳ���
							(UBaseType_t    )NUC_TASK_PRIO,       //�������ȼ�
							(TaskHandle_t*  )&NUC_Task_Handler);  //������  
}

/*
*@title��NUC����
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void nuc_task(void *p_arg)
{
	const TickType_t TimeIncrement=pdMS_TO_TICKS(2);
	TickType_t	PreviousWakeTime;
	PreviousWakeTime=xTaskGetTickCount();	
	
	while(1)
	{
		
		NUC_Send_Data();
		Dual_Board_Send_Data();
		
		vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);	
	}

}