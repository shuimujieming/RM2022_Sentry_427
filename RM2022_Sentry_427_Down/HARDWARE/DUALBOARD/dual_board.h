#ifndef __Dual_Board_H
#define __Dual_Board_H



#include "main.h"
void Dual_Board_Init();
void Dual_Board_Send_Data();

typedef struct
{
	int8_t Get_Hit_flag;                    //2-û���ܵ�������0-װ��0�ܵ�������1-װ��1�ܵ�������3-����װ�׶��ܵ��˹���
	int8_t Bass_Status;                     //2-100%������1-50%���� �� 0-�޷���
	short  Chassis_Real_Position;           //��λmm
	short  Chassis_Real_Number;             //ÿ��10s��һ����
	int8_t Shoot_Allow_Flag;                //ʣ��Ѫ������50
	int8_t Sentry_Satus;                    //1-���ذ�ȫ��0-�����ܵ�����
	short  Chassis_Speed_Real;              //�����ٶ�
	char   Bullet_Type;                     //1--����   2--С����  
	char   Bullet_Speed;
	char   Locked_Flag;
	char   Which_Color_I_Am;                //��0  ��1
	char   Output_On;
  char   Enemy_Armor_ID;
	u8     Enemy_State;                //�з������˵Ĵ��״̬
	u8     Enemy_HP_HL;                //�з�������Ѫ���Ӹߵ������У�01��1��Ӣ�ۣ�11��3�Ų�����00���ĺŲ�����10��5�Ų���,��λѪ����
	
}Up_To_Down_Typedef;

//�����巢�͵����������ݽṹ��
typedef struct
{
	int8_t Target_Locked;                  //0-û���ҵ�Ŀ�꣬1-�ҵ�Ŀ�꣬2-����Ŀ��
	short  Target_Position;
	char   Enemy_Armor_ID;
	short  Yaw_Angle_Real;
	char   NUC_Shoot_Allow_Flag;
	short  Distance;
}Down_To_Up_Typedef;

#endif