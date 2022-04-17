#include "supercap_task.h"

TaskHandle_t SUPERCAP_Task_Handler;
void supercap_task(void *p_arg);

/*
*@title�������������񴴽�
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void supercap_task_create()
{
		xTaskCreate((TaskFunction_t )supercap_task,          //������
							(const char*    )"supercap_task",          //��������
							(uint16_t       )SUPERCAP_STK_SIZE,        //�����ջ��С
							(void*          )NULL,                //���ݸ��������Ĳ���
							(UBaseType_t    )SUPERCAP_TASK_PRIO,       //�������ȼ�
							(TaskHandle_t*  )&SUPERCAP_Task_Handler);  //������  
}

/*
*@title��������������
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void supercap_task(void *p_arg)
{
	const TickType_t TimeIncrement=pdMS_TO_TICKS(100);
	TickType_t	PreviousWakeTime;
	PreviousWakeTime=xTaskGetTickCount();	
	
	while(1)
	{
		
		xEventGroupSetBits(EventGroupHandler,SUPERCAP_SIGNAL);	//��̨�¼�����λ
		vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);	
	}

}