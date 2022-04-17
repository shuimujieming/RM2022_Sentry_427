#ifndef __NUC_H
#define __NUC_H



#include "main.h"
void NUC_Init();
void NUC_Send_Data();


//NUC�������ݽṹ��
typedef struct
{
	int8_t Armor_Type;             //װ������  0-��  1-С  2-��
	float  Yaw_Angle;              //Y�������Ƕ�
  float  Pitch_Angle;            //P�������Ƕ�
	short  Enemy_distance;         //�з�����
	char   Armor_ID;               //�з�װ�׺���
	short  Enemy_Real_Speed;
	char   NUC_Shoot_Allow_Flag;
}NUC_Typedef;

#endif