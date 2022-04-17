#include "laser.h"

#define RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIO_LASER, ENABLE) 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE)

#define GPIO_Pin_LASER GPIO_Pin_13

#define GPIO_LASER GPIOG


/**
  * @brief  ���⴫������ʼ��
  * @param  void
  * @retval void
  * @notes  ��PH11 C  ��PH12 B  L-�ڵ�Ϊ0��û��Ϊ1
  */
void Infrared_Sensor_Init(void)
{
  GPIO_InitTypeDef GPIO_InitTypeStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH,ENABLE);
	
	GPIO_InitTypeStruct.GPIO_Pin=GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitTypeStruct.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitTypeStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitTypeStruct.GPIO_Speed=GPIO_High_Speed;
	GPIO_InitTypeStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_Init(GPIOH,&GPIO_InitTypeStruct);

	
}

/*
*@Description�����������ʼ��
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Laser_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIO_LASER, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_LASER;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_Init(GPIO_LASER, &GPIO_InitStructure); 
	
	GPIO_SetBits(GPIO_LASER,GPIO_Pin_LASER);
	
	Infrared_Sensor_Init();
	
}




void Limit_Init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure); 
}
