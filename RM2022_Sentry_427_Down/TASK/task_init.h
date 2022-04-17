#ifndef __TASK_INIT_H
#define __TASK_INIT_H

#include "main.h"
extern EventGroupHandle_t EventGroupHandler;

//事件组标志位
#define GIMBAL_SIGNAL   ( 1 << 0 )
#define CHASSIS_SIGNAL ( 1 << 1 )
#define SHOOT_SIGNAL     ( 1 << 2 )
#define SUPERCAP_SIGNAL     ( 1 << 3 )

//开始任务
#define START_TASK_PRIO		1
#define START_STK_SIZE		256
//LED任务
#define LED_TASK_PRIO		2
#define LED_STK_SIZE		128
//底盘任务
#define CHASSIS_TASK_PRIO		3
#define CHASSIS_STK_SIZE		256
//云台任务
#define GIMBAL_TASK_PRIO		4
#define GIMBAL_STK_SIZE		256
//发射任务
#define SHOOT_TASK_PRIO		5
#define SHOOT_STK_SIZE		256
//陀螺仪任务
#define IMU_TASK_PRIO		6
#define IMU_STK_SIZE		256
//CAN数据发送任务
#define CAN_SEND_TASK_PRIO		7
#define CAN_SEND_STK_SIZE		256
//超级电容任务
#define SUPERCAP_TASK_PRIO		8
#define SUPERCAP_STK_SIZE		256
//自定义UI任务
#define UI_TASK_PRIO		9
#define UI_STK_SIZE		256
//NUC任务
#define NUC_TASK_PRIO		9
#define NUC_STK_SIZE		256
//模式状态更新任务
#define MODE_TASK_PRIO		3
#define MODE_STK_SIZE		256
void Task_Init();
#endif