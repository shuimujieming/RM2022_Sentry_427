#include "referee.h"


#define RCC_AHBPeriphClockCmd_GPIO_Referee RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE)
#define RCC_APBPeriphClockCmd_USART_Referee RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE)
#define GPIO_Referee GPIOD
#define GPIO_PinSource_Referee_1 GPIO_PinSource8
#define GPIO_PinSource_Referee_2 GPIO_PinSource9

#define GPIO_Pin_Referee_1 GPIO_Pin_8
#define GPIO_Pin_Referee_2 GPIO_Pin_9

#define USART_Referee USART3

#define RCC_AHBPeriphClockCmd_DMA_Referee RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE)
#define DMA_Channel_Referee_RX DMA_Channel_4
#define DMA_Stream_Referee_RX DMA1_Stream1

#define USART_Referee_IRQn USART3_IRQn
#define USART_Referee_IRQHandler USART3_IRQHandler

//����3�������ݻ���
uint8_t Usart_Referee_Rx_Buf[2][1024];


void Referee_USART_Init()
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_AHBPeriphClockCmd_GPIO_Referee; //ʹ��GPIOBʱ��
	RCC_APBPeriphClockCmd_USART_Referee;//ʹ��USART1ʱ��

	GPIO_PinAFConfig(GPIO_Referee,GPIO_PinSource_Referee_1,GPIO_AF_USART3); 
	GPIO_PinAFConfig(GPIO_Referee,GPIO_PinSource_Referee_2,GPIO_AF_USART3); 


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_Referee_1|GPIO_Pin_Referee_2; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIO_Referee,&GPIO_InitStructure);

	//USART1��ʼ������
	USART_InitStructure.USART_BaudRate = 115200;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART_Referee, &USART_InitStructure); //��ʼ������3

	USART_ClearFlag(USART_Referee, USART_FLAG_IDLE);
	USART_ITConfig(USART_Referee, USART_IT_IDLE, ENABLE);
	
	USART_Cmd(USART_Referee, ENABLE);  //ʹ�ܴ���3
}
void Referee_USART_DMA_Init()
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd_DMA_Referee;
	
	//DMA1_Stream1_Channel4
	//USART3_RX
	DMA_InitStructure.DMA_Channel = DMA_Channel_Referee_RX;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (USART_Referee->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Usart_Referee_Rx_Buf[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 1024;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA_Stream_Referee_RX, &DMA_InitStructure);
	
	
	DMA_DoubleBufferModeConfig(DMA_Stream_Referee_RX, (uint32_t)Usart_Referee_Rx_Buf[1], DMA_Memory_0);
	DMA_DoubleBufferModeCmd(DMA_Stream_Referee_RX, ENABLE);
	
	USART_DMACmd(USART_Referee, USART_DMAReq_Rx, ENABLE);
	
	DMA_Cmd(DMA_Stream_Referee_RX,ENABLE);

//	//DMA1_Stream3_Channel4
//	//USART3_TX
//	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (USART3->DR);
//	DMA_InitStructure.DMA_Memory0BaseAddr = 0;
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
//	DMA_InitStructure.DMA_BufferSize = 10;
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
//	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
//	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
//	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
//	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
//	DMA_Init(DMA1_Stream3, &DMA_InitStructure);
}
void Referee_USART_NVIC_Init()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART_Referee_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void Referee_Init()
{
	Referee_USART_Init();
	Referee_USART_NVIC_Init();
	Referee_USART_DMA_Init();
}



void USART_Referee_IRQHandler(void)
{

	
		if(USART_GetITStatus(USART_Referee, USART_IT_IDLE) != RESET)	//�ж��Ƿ�Ϊ�����ж�
    {
			
			u8 selectedMemory = 0;	//��ǰѡ����ڴ�����	
			u16 index=0;	//��ǰ�������
			u16 data_length;	//֡���ݳ���
			
      (void)USART_Referee->SR;
			(void)USART_Referee->DR;
			
			DMA_Cmd(DMA_Stream_Referee_RX,DISABLE);
			
			if(DMA_GetCurrentMemoryTarget(DMA_Stream_Referee_RX) == DMA_Memory_0)	//��ȡ��ǰĿ���ڴ��Ƿ�Ϊ DMA_Memory_0
			{
				selectedMemory = 0;
				
				
							//����ϵͳ��������ѡ����
				while(Usart_Referee_Rx_Buf[selectedMemory][index]==0XA5) //֡ͷSOFУ��
				{
						if(Verify_CRC8_Check_Sum(&Usart_Referee_Rx_Buf[selectedMemory][index],5) == 1)
						{
							data_length = Usart_Referee_Rx_Buf[selectedMemory][index+2]<<8 | Usart_Referee_Rx_Buf[selectedMemory][index+1] + 9;
							if(Verify_CRC16_Check_Sum(&Usart_Referee_Rx_Buf[selectedMemory][index],data_length))	//CRC16У�飨CRC8������У�飩
							{
								//����ϵͳ���ݽ���
								RefereeInfo_Decode(&Usart_Referee_Rx_Buf[selectedMemory][index]);	
							}			
						}
						index+=data_length;
				}
				
				//�������
				memset(Usart_Referee_Rx_Buf[selectedMemory],0,1024);
				
				
				//�������DMA������
				DMA_SetCurrDataCounter(DMA_Stream_Referee_RX,1024);
				
				DMA_Stream_Referee_RX->CR |= (uint32_t)(DMA_SxCR_CT);
								
				DMA_Cmd(DMA_Stream_Referee_RX,ENABLE);

			}
			else
			{
				selectedMemory = 1;
				
											//����ϵͳ��������ѡ����
				while(Usart_Referee_Rx_Buf[selectedMemory][index]==0XA5) //֡ͷSOFУ��
				{
						if(Verify_CRC8_Check_Sum(&Usart_Referee_Rx_Buf[selectedMemory][index],5) == 1)
						{
							data_length = Usart_Referee_Rx_Buf[selectedMemory][index+2]<<8 | Usart_Referee_Rx_Buf[selectedMemory][index+1] + 9;
							if(Verify_CRC16_Check_Sum(&Usart_Referee_Rx_Buf[selectedMemory][index],data_length))	//CRC16У�飨CRC8������У�飩
							{
								//����ϵͳ���ݽ���
								RefereeInfo_Decode(&Usart_Referee_Rx_Buf[selectedMemory][index]);	
							}			
						}
						index+=data_length;
				}
				
				//�������
				memset(Usart_Referee_Rx_Buf[selectedMemory],0,1024);
				
				//�������DMA������
				DMA_SetCurrDataCounter(DMA_Stream_Referee_RX,1024);
				
				DMA_Stream_Referee_RX->CR &= ~(uint32_t)(DMA_SxCR_CT);  
				
				DMA_Cmd(DMA_Stream_Referee_RX,ENABLE);

			}
			

			
    }
}
/*
*@Description������ϵͳ�����ϴ�
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Referee_Data_Send(uint16_t cmd_id, uint8_t* p_data)
{
	static uint8_t seq = 0;
	uint8_t Tx_Buff[200];
	memset(Tx_Buff,0,200);
	
	//֡ͷ���
	Tx_Buff[0] = 0xA5;
	memcpy(&Tx_Buff[1],(uint8_t*)sizeof(p_data),2);	//data�ĳ���
	Tx_Buff[3] = seq;	//�����
	Append_CRC8_Check_Sum(Tx_Buff,5);
	
	//���������
	memcpy(&Tx_Buff[5],(uint8_t *)&cmd_id,2);
	
	memcpy(&Tx_Buff[7],p_data,(uint8_t)sizeof(p_data));
	Append_CRC16_Check_Sum(Tx_Buff,(uint8_t)sizeof(p_data) + 9);
	
	if(seq == 0xff){seq = 0;}
	else seq++;
	
	
	//�����ϴ�
	USART_ClearFlag(USART_Referee,USART_FLAG_TC);
	for(int i=0;i<(uint8_t)sizeof(p_data) + 9;i++)
	{
	  USART_SendData(USART_Referee,Tx_Buff[i]);
	  while (USART_GetFlagStatus(USART_Referee,USART_FLAG_TC) == RESET); //�ȴ�֮ǰ���ַ��������
	}
	
}