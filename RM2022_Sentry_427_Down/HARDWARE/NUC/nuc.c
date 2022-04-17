#include "nuc.h"
#define RCC_AHBPeriphClockCmd_GPIO_NUC RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE)
#define RCC_APBPeriphClockCmd_USART_NUC RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE)

#define GPIO_NUC GPIOD
#define GPIO_PinSource_NUC_1 GPIO_PinSource9
#define GPIO_PinSource_NUC_2 GPIO_PinSource8

#define GPIO_AF_USART_NUC GPIO_AF_USART3
#define GPIO_Pin_NUC_1 GPIO_Pin_9
#define GPIO_Pin_NUC_2 GPIO_Pin_8
#define USART_NUC USART3

#define DMA_Channel_NUC_RX DMA_Channel_4
#define DMA_Stream_NUC_RX DMA1_Stream1

#define DMA_Channel_NUC_TX DMA_Channel_4
#define DMA_Stream_NUC_TX DMA1_Stream3

#define DMA_Stream_NUC_IRQn DMA1_Stream3_IRQn
#define DMA_Stream_NUC_TX_IRQHandler DMA1_Stream3_IRQHandler
#define DMA_IT_TCIF_NUC DMA_IT_TCIF3

#define USART_NUC_IRQHandler USART3_IRQHandler

#define USART_NUC_IRQn USART3_IRQn
#define RCC_AHBPeriphClockCmd_DMA_NUC RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE)


#define NUC_RX_BUFF_SIZE 10
#define NUC_TX_BUFF_SIZE 23

uint8_t NUC_rx_buff[2][NUC_RX_BUFF_SIZE];
uint8_t NUC_tx_buff[NUC_TX_BUFF_SIZE];

NUC_Typedef NUC_Data;

/*
*@Description������8��ʼ��
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void NUC_USART_Init()
{
	GPIO_InitTypeDef   GPIO_InitStruct;
	USART_InitTypeDef  USART_InitStruct;
	DMA_InitTypeDef    DMA_InitStruct;
	NVIC_InitTypeDef   NVIC_InitStruct;
	
	
	RCC_AHBPeriphClockCmd_GPIO_NUC;//�˴������û����㣬��Ϊ�ǿ���λ��
	RCC_APBPeriphClockCmd_USART_NUC;
	

	GPIO_PinAFConfig(GPIO_NUC, GPIO_PinSource_NUC_1, GPIO_AF_USART_NUC);
	GPIO_PinAFConfig(GPIO_NUC, GPIO_PinSource_NUC_2, GPIO_AF_USART_NUC);
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_NUC_1|GPIO_Pin_NUC_2;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIO_NUC, &GPIO_InitStruct);
	
	USART_InitStruct.USART_BaudRate            = 115200;
	USART_InitStruct.USART_WordLength          = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits            = USART_StopBits_1;
	USART_InitStruct.USART_Parity              = USART_Parity_No;
	USART_InitStruct.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART_NUC, &USART_InitStruct);
	
	USART_ClearFlag(USART_NUC, USART_FLAG_IDLE);
	USART_ITConfig(USART_NUC, USART_IT_IDLE, ENABLE);	
	
	USART_Cmd(USART_NUC, ENABLE);  //ʹ�ܴ���8
}
void NUC_USART_DMA_Init()
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd_DMA_NUC;
	
	//DMA1_Stream6_Channel5
	//UART8_RX
	DMA_InitStructure.DMA_Channel = DMA_Channel_NUC_RX;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (USART_NUC->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)NUC_rx_buff[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = NUC_RX_BUFF_SIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA_Stream_NUC_RX, &DMA_InitStructure);
	
	DMA_DoubleBufferModeConfig(DMA_Stream_NUC_RX, (uint32_t)NUC_rx_buff[1], DMA_Memory_0);
	DMA_DoubleBufferModeCmd(DMA_Stream_NUC_RX, ENABLE);
	
	USART_DMACmd(USART_NUC, USART_DMAReq_Rx, ENABLE);	
	
	DMA_Cmd(DMA_Stream_NUC_RX, ENABLE);
	

	//DMA1_Stream0_Channel5
	//UART8_TX
	DMA_InitStructure.DMA_Channel = DMA_Channel_NUC_TX;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (USART_NUC->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)NUC_tx_buff;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = NUC_TX_BUFF_SIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA_Stream_NUC_TX, &DMA_InitStructure);
	
	USART_DMACmd(USART_NUC, USART_DMAReq_Tx, ENABLE);

	DMA_ITConfig(DMA_Stream_NUC_TX,DMA_IT_TC,ENABLE);
	
	
	DMA_Cmd(DMA_Stream_NUC_TX, ENABLE);

}


void NUC_USART_NVIC_Init()
{
	//����8���տ����ж�
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART_NUC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//�����ж�
	NVIC_InitStructure.NVIC_IRQChannel                   =DMA_Stream_NUC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        =0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                =ENABLE ;
	NVIC_Init(&NVIC_InitStructure);
}

void NUC_Init()
{
	NUC_USART_Init();
	NUC_USART_NVIC_Init();
	NUC_USART_DMA_Init();
}
/*
*@title��NUC�����ж�
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void DMA_Stream_NUC_TX_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA_Stream_NUC_TX, DMA_IT_TCIF_NUC) == SET)
	{
		DMA_Cmd(DMA_Stream_NUC_TX, DISABLE);
		DMA_SetCurrDataCounter(DMA_Stream_NUC_TX, NUC_TX_BUFF_SIZE); 
	}
	DMA_ClearITPendingBit(DMA_Stream_NUC_TX, DMA_IT_TCIF_NUC);
}

static u8 bit32TObit8(int index_need,int bit32)
{
	union
	{
    int  f;
		u8  byte[4];
	}u32val;
   u32val.f = bit32;
	return u32val.byte[index_need];
}
static char shortTou8(char bit,short data)
{
	union
	{
		short i;
		char byte[2];
	}u16val;
	u16val.i=data;
	return u16val.byte[bit];
}
float Yaw_Angle_NUC,Pitch_Angle_NUC,Yaw_Speed_NUC,Pitch_Speed_NUC;

void NUC_Send_Data()
{
			Yaw_Angle_NUC=(float)(CAN_Gimbal[0].Current_MechAngle-6083)/8192.0f*180.0f*10;
//			
			// Pitch��ĽǶȾ�����ʵ�Ļ�е�Ǵ���֮���͹�ȥ.8192��Ӧ180�ȣ���λ����*10
	    Pitch_Angle_NUC=(float)(CAN_Gimbal[1].Current_MechAngle - 10254)/8192.0f*180.0f*10;
	
	     //��λ����/s*10
			Yaw_Speed_NUC=((float)CAN_Gimbal[0].Current_Speed*360.0f)/(60.0f)*10.0f;
	
	     //��λ����/s*10
	    Pitch_Speed_NUC=((float)CAN_Gimbal[1].Current_Speed*360.0f)/(60.0f)*10.0f;
//			
//			Bullet_Speed =Up_Bullet_Speed;
			
			NUC_tx_buff[0]=0x7a;
//			NUC_tx_buff[1]=Bullet_Speed;
			NUC_tx_buff[2]=bit32TObit8(0,(short)Yaw_Angle_NUC);
			NUC_tx_buff[3]=bit32TObit8(1,(short)Yaw_Angle_NUC);	
			NUC_tx_buff[4]=bit32TObit8(2,(short)Yaw_Angle_NUC);
			NUC_tx_buff[5]=bit32TObit8(3,(short)Yaw_Angle_NUC);	
			NUC_tx_buff[6]=shortTou8(0,(short)Pitch_Speed_NUC);//�����ٶ������Ӿ���û��ʹ��
			NUC_tx_buff[7]=shortTou8(1,(short)Pitch_Speed_NUC);
			NUC_tx_buff[8]=shortTou8(0,(short)Yaw_Speed_NUC);
			NUC_tx_buff[9]=shortTou8(1,(short)Yaw_Speed_NUC);	
			//��9�����������ж������ڱ���̨�ǲ��Ǵ����ҵ����˵�״̬�Լ��������ɫ��ʲô��00��ʾ�췽�����ҵ�����
//		  Send_NUC[10]=shortTou8(0,(short)Up_To_Down_TypeStruct.Chassis_Real_Position );//λ�ã���λmm
//	    Send_NUC[11]=shortTou8(1,(short)Up_To_Down_TypeStruct.Chassis_Real_Position );	
//			Send_NUC[12]=(Find_Target_1)|(Up_To_Down_TypeStruct.Which_Color_I_Am<<1)|(Output_On<<2)|(Swing_Flag<<3);//������ɫ���������з��͹����ģ��������Ǻ췽ʱ����0��������ʱ�����1��
//			Send_NUC[13]=Down_To_Up_TypeStruct.Enemy_Armor_ID;
//    	Send_NUC[14]=shortTou8(0,(short)Up_To_Down_TypeStruct.Chassis_Speed_Real );//�ٶȣ���λmm/s
//	    Send_NUC[15]=shortTou8(1,(short)Up_To_Down_TypeStruct.Chassis_Speed_Real );
//			Send_NUC[16]=Up_To_Down_TypeStruct.Enemy_State;  
//	    Send_NUC[17]=Up_To_Down_TypeStruct.Enemy_HP_HL;
			NUC_tx_buff[18]=shortTou8(0,(short)Pitch_Angle_NUC);
			NUC_tx_buff[19]=shortTou8(1,(short)Pitch_Angle_NUC);
//			Send_NUC[20]=shortTou8(0,Chassis_Accel);
//			Send_NUC[21]=shortTou8(1,Chassis_Accel);
			
			
			
			NUC_tx_buff[22]=0x7b;
		
		DMA_Cmd(DMA_Stream_NUC_TX,ENABLE);
}
extern Down_To_Up_Typedef  Down_To_Up_Data;

void NUC_Data_Decode(uint8_t *buff)
{
	if((buff[0] + buff[3] + buff[5]) % 255 == buff[9])
	{
		//������
		NUC_Data.NUC_Shoot_Allow_Flag = (buff[0] == 0x1f);

		NUC_Data.Yaw_Angle   = (short)(buff[2] << 8 | buff[1]);
		NUC_Data.Pitch_Angle = (short)(buff[4] << 8 | buff[3]);
		NUC_Data.Armor_Type  =  buff[6]; //װ������ 0δʶ�� 1Сװ�� 2��װ��
		
		if(NUC_Data.Armor_Type == 0)
		{
		NUC_Data.Yaw_Angle   = 0;
		NUC_Data.Pitch_Angle = 0;			
		}
	}
}
/*
*@Description������8�жϴ�����
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void USART_NUC_IRQHandler()
{
			if(USART_GetITStatus(USART_NUC, USART_IT_IDLE) != RESET)	//�ж��Ƿ�Ϊ�����ж�
    {
        uint16_t this_time_rx_len = 0;
        USART_ReceiveData(USART_NUC);

        if(DMA_GetCurrentMemoryTarget(DMA_Stream_NUC_RX) == DMA_Memory_0)	//��ȡ��ǰĿ���ڴ��Ƿ�Ϊ DMA_Memory_0
        {
            //��������DMA
            DMA_Cmd(DMA_Stream_NUC_RX, DISABLE);
            this_time_rx_len = DMA_GetCurrDataCounter(DMA_Stream_NUC_RX);
            DMA_SetCurrDataCounter(DMA_Stream_NUC_RX, NUC_RX_BUFF_SIZE);
            DMA_Cmd(DMA_Stream_NUC_RX, ENABLE);
            if(this_time_rx_len == NUC_RX_BUFF_SIZE)	//У�����
            {
                //��������
               NUC_Data_Decode(NUC_rx_buff[1]);
            }
        }
        else
        {
            //��������DMA
            DMA_Cmd(DMA_Stream_NUC_RX, DISABLE);
            this_time_rx_len = DMA_GetCurrDataCounter(DMA_Stream_NUC_RX);
            DMA_SetCurrDataCounter(DMA_Stream_NUC_RX, NUC_RX_BUFF_SIZE);
            DMA_Cmd(DMA_Stream_NUC_RX, ENABLE);
            if(this_time_rx_len == NUC_RX_BUFF_SIZE)	//У�����
            {
                //��������
               NUC_Data_Decode(NUC_rx_buff[0]);
            }
        }
    }
}