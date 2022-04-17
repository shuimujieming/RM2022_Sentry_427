#include "can_send_task.h"


TaskHandle_t CAN_SEND_Task_Handler;
void can_send_task(void *p_arg);

/*
*@title��CAN���ݷ������񴴽�
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void can_send_task_create()
{
		xTaskCreate((TaskFunction_t )can_send_task,          //������
							(const char*    )"can_send_task",          //��������
							(uint16_t       )CAN_SEND_STK_SIZE,        //�����ջ��С
							(void*          )NULL,                //���ݸ��������Ĳ���
							(UBaseType_t    )CAN_SEND_TASK_PRIO,       //�������ȼ�
							(TaskHandle_t*  )&CAN_SEND_Task_Handler);  //������  
}

/*
*@title��CAN���ݷ�������
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void can_send_task(void *p_arg)
{

	EventBits_t  EventValue;

	
	while(1)
	{
		EventValue = xEventGroupWaitBits((EventGroupHandle_t)EventGroupHandler,
																	(EventBits_t      )CHASSIS_SIGNAL | GIMBAL_SIGNAL | SUPERCAP_SIGNAL |SHOOT_SIGNAL,
																	(BaseType_t       )pdTRUE,
																	(BaseType_t       )pdFALSE,
																	(TickType_t       )portMAX_DELAY);
		
			//��̨�����̡����֡����䡢��������CAN���ݵȴ�����
		if(EventValue & GIMBAL_SIGNAL)
		{
			CAN1_TX_Gimbal();
		}
		else if(EventValue & CHASSIS_SIGNAL)
		{
			CAN1_TX_Chassis();
		}
		else if(EventValue & SHOOT_SIGNAL)
		{
			CAN1_TX_Shoot();
		}
		
	}

}