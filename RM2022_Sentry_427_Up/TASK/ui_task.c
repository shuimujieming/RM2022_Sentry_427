#include "ui_task.h"

TaskHandle_t UI_Task_Handler;
void ui_task(void *p_arg);

/*
*@title���Զ���UI���񴴽�
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void ui_task_create()
{
		xTaskCreate((TaskFunction_t )ui_task,          //������
							(const char*    )"ui_task",          //��������
							(uint16_t       )UI_STK_SIZE,        //�����ջ��С
							(void*          )NULL,                //���ݸ��������Ĳ���
							(UBaseType_t    )UI_TASK_PRIO,       //�������ȼ�
							(TaskHandle_t*  )&UI_Task_Handler);  //������  
}

/*
*@title���Զ���UI����
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void ui_task(void *p_arg)
{
	const TickType_t TimeIncrement=pdMS_TO_TICKS(10);
	TickType_t	PreviousWakeTime;
	PreviousWakeTime=xTaskGetTickCount();	
	
	while(1)
	{
		//����UI
		Show_UI();
		vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);	
	}

}