#ifndef __MAIN_H
#define __MAIN_H
#include "sys.h"
#include "stdio.h"	
#include <math.h>

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#include "task_init.h"
#include "led_task.h"
#include "imu_task.h"
#include "shoot_task.h"
#include "gimbal_task.h"
#include "chassis_task.h"
#include "ui_task.h"
#include "can_send_task.h"
#include "mode_task.h"
#include "supercap_task.h"
#include "nuc_task.h"


//���ڴ�ӡ�Ĵ�������
#define USART_PRINT USART6

//��������
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "laser.h"
#include "dr16.h"
#include "tim6.h"
#include "pid.h"
#include "can.h"
#include "spi.h"
#include "imu.h"
#include "power.h"
#include "nuc.h"
#include "shoot.h"
#include "cover.h"
#include "referee.h"
#include "beep.h"
#include "dual_board.h"


//����ϵͳ
#include "referee_info.h"
#include "crc.h"
#include "custom_ui.h"

//��̨���̿���
#include "gimbal.h"
#include "chassis.h"
#include "can_handle.h"

#endif