#include "Dual_Board.h"
#define RCC_AHBPeriphClockCmd_GPIO_Dual_Board RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE)
#define RCC_APBPeriphClockCmd_USART_Dual_Board RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART8, ENABLE)

#define GPIO_Dual_Board GPIOE
#define GPIO_PinSource_Dual_Board_1 GPIO_PinSource0
#define GPIO_PinSource_Dual_Board_2 GPIO_PinSource1

#define GPIO_AF_USART_Dual_Board GPIO_AF_UART8
#define GPIO_Pin_Dual_Board_1 GPIO_Pin_0
#define GPIO_Pin_Dual_Board_2 GPIO_Pin_1
#define USART_Dual_Board UART8

#define DMA_Channel_Dual_Board_RX DMA_Channel_5
#define DMA_Stream_Dual_Board_RX DMA1_Stream6

#define DMA_Channel_Dual_Board_TX DMA_Channel_5
#define DMA_Stream_Dual_Board_TX DMA1_Stream0

#define DMA_Stream_Dual_Board_IRQn DMA1_Stream0_IRQn
#define DMA_Stream_Dual_Board_TX_IRQHandler DMA1_Stream0_IRQHandler
#define DMA_IT_TCIF_DUAL_BOARD DMA_IT_TCIF0


#define USART_Dual_Board_IRQn UART8_IRQn
#define USART_Dual_Board_IRQHandler UART8_IRQHandler
#define RCC_AHBPeriphClockCmd_DMA_Dual_Board RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE)


#define Dual_Board_RX_BUFF_SIZE 17 
#define Dual_Board_TX_BUFF_SIZE 10

uint8_t Dual_Board_rx_buff[2][Dual_Board_RX_BUFF_SIZE];
uint8_t Dual_Board_tx_buff[Dual_Board_TX_BUFF_SIZE];

Up_To_Down_Typedef  Up_To_Down_Data;
Down_To_Up_Typedef  Down_To_Up_Data;

/*
*@Description������8��ʼ��
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Dual_Board_USART_Init()
{
	GPIO_InitTypeDef   GPIO_InitStruct;
	USART_InitTypeDef  USART_InitStruct;
	DMA_InitTypeDef    DMA_InitStruct;
	NVIC_InitTypeDef   NVIC_InitStruct;
	
	
	RCC_AHBPeriphClockCmd_GPIO_Dual_Board;//�˴������û����㣬��Ϊ�ǿ���λ��
	RCC_APBPeriphClockCmd_USART_Dual_Board;
	

	GPIO_PinAFConfig(GPIO_Dual_Board, GPIO_PinSource_Dual_Board_1, GPIO_AF_USART_Dual_Board);
	GPIO_PinAFConfig(GPIO_Dual_Board, GPIO_PinSource_Dual_Board_2, GPIO_AF_USART_Dual_Board);
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_Dual_Board_1|GPIO_Pin_Dual_Board_2;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIO_Dual_Board, &GPIO_InitStruct);
	
	USART_InitStruct.USART_BaudRate            = 115200;
	USART_InitStruct.USART_WordLength          = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits            = USART_StopBits_1;
	USART_InitStruct.USART_Parity              = USART_Parity_No;
	USART_InitStruct.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART_Dual_Board, &USART_InitStruct);
	
	USART_ClearFlag(USART_Dual_Board, USART_FLAG_IDLE);
	USART_ITConfig(USART_Dual_Board, USART_IT_IDLE, ENABLE);	
	
	USART_Cmd(USART_Dual_Board, ENABLE);  //ʹ�ܴ���8
}
void Dual_Board_USART_DMA_Init()
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd_DMA_Dual_Board;
	
	//DMA1_Stream6_Channel5
	//UART8_RX
	DMA_InitStructure.DMA_Channel = DMA_Channel_Dual_Board_RX;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (USART_Dual_Board->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Dual_Board_rx_buff[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = Dual_Board_RX_BUFF_SIZE;
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
	DMA_Init(DMA_Stream_Dual_Board_RX, &DMA_InitStructure);
	
	DMA_DoubleBufferModeConfig(DMA_Stream_Dual_Board_RX, (uint32_t)Dual_Board_rx_buff[1], DMA_Memory_0);
	DMA_DoubleBufferModeCmd(DMA_Stream_Dual_Board_RX, ENABLE);
	
	USART_DMACmd(USART_Dual_Board, USART_DMAReq_Rx, ENABLE);	
	
	DMA_Cmd(DMA_Stream_Dual_Board_RX, ENABLE);
	

	//DMA1_Stream0_Channel5
	//UART8_TX
	DMA_InitStructure.DMA_Channel = DMA_Channel_Dual_Board_TX;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (USART_Dual_Board->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Dual_Board_tx_buff;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = Dual_Board_TX_BUFF_SIZE;
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
	DMA_Init(DMA_Stream_Dual_Board_TX, &DMA_InitStructure);
	
	USART_DMACmd(USART_Dual_Board, USART_DMAReq_Tx, ENABLE);

	DMA_ITConfig(DMA_Stream_Dual_Board_TX,DMA_IT_TC,ENABLE);
	
	
	DMA_Cmd(DMA_Stream_Dual_Board_TX, ENABLE);

}


void Dual_Board_USART_NVIC_Init()
{
	//����8���տ����ж�
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART_Dual_Board_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//�����ж�
	NVIC_InitStructure.NVIC_IRQChannel                   =DMA_Stream_Dual_Board_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        =0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                =ENABLE ;
	NVIC_Init(&NVIC_InitStructure);
}

void Dual_Board_Init()
{
	Dual_Board_USART_Init();
	Dual_Board_USART_NVIC_Init();
	Dual_Board_USART_DMA_Init();
}
/*
*@title��Dual_Board�����ж�
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void DMA_Stream_Dual_Board_TX_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA_Stream_Dual_Board_TX, DMA_IT_TCIF_DUAL_BOARD) == SET)
	{
		DMA_Cmd(DMA_Stream_Dual_Board_TX, DISABLE);
		DMA_SetCurrDataCounter(DMA_Stream_Dual_Board_TX, Dual_Board_TX_BUFF_SIZE); 
	}
	DMA_ClearITPendingBit(DMA_Stream_Dual_Board_TX, DMA_IT_TCIF_DUAL_BOARD);
}
//�°巢������
void Dual_Board_Send_Data()
{
				//����֡�ṹ
			//֡ͷ
//			Down_To_Up_Data .Yaw_Angle_Real=Yaw_Real_Angle;
//			
			Dual_Board_tx_buff[0] = 0x7A;
			Dual_Board_tx_buff[1] = Down_To_Up_Data.Target_Locked;
			Dual_Board_tx_buff[2] = 0;
			Dual_Board_tx_buff[3] = 0;
			Dual_Board_tx_buff[4] = Down_To_Up_Data.Enemy_Armor_ID;
			Dual_Board_tx_buff[5] = 0;
			Dual_Board_tx_buff[6] = 0;
			Dual_Board_tx_buff[7] = Down_To_Up_Data.NUC_Shoot_Allow_Flag;
			Dual_Board_tx_buff[8] = 30;
			Dual_Board_tx_buff[9] = 0x7B;

		DMA_Cmd(DMA_Stream_Dual_Board_TX,ENABLE);
}

//�����ϰ�����
void Dual_Board_Data_Decode(uint8_t *buff)
{
	
			if( (buff[0]==0x7A) && (buff[16]==0x7B) )
			{
				Up_To_Down_Data .Get_Hit_flag         = buff[1];
				Up_To_Down_Data.Which_Color_I_Am     = (buff[2]&0x01);
				Up_To_Down_Data.Output_On            =(buff[2]>>1);
//				Swing_Flag																 =(buff[2]>>2);
				Up_To_Down_Data.Shoot_Allow_Flag     = buff[3];
				Up_To_Down_Data.Chassis_Real_Position= (short)(buff[5] << 8 | buff[4]);
				Up_To_Down_Data.Sentry_Satus         = buff[6];
				Up_To_Down_Data.Bullet_Speed         = buff[7];
				Up_To_Down_Data.Locked_Flag          = buff[8];
				Up_To_Down_Data.Chassis_Speed_Real   = (short)(buff[10] << 8 | buff[9]);
				Up_To_Down_Data.Enemy_Armor_ID       = buff[11] ; 
        Up_To_Down_Data.Enemy_State          = buff[12] ;
        Up_To_Down_Data.Enemy_HP_HL          = buff[13] ;
//        Chassis_Accel       										   = int8ToShort(Up_To_DOWN_Rx,14)
	
	
			}
}
/*
*@Description������8�жϴ�����
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void USART_Dual_Board_IRQHandler()
{
			if(USART_GetITStatus(USART_Dual_Board, USART_IT_IDLE) != RESET)	//�ж��Ƿ�Ϊ�����ж�
    {
        uint16_t this_time_rx_len = 0;
        USART_ReceiveData(USART_Dual_Board);

        if(DMA_GetCurrentMemoryTarget(DMA_Stream_Dual_Board_RX) == DMA_Memory_0)	//��ȡ��ǰĿ���ڴ��Ƿ�Ϊ DMA_Memory_0
        {
            //��������DMA
            DMA_Cmd(DMA_Stream_Dual_Board_RX, DISABLE);
            this_time_rx_len = DMA_GetCurrDataCounter(DMA_Stream_Dual_Board_RX);
            DMA_SetCurrDataCounter(DMA_Stream_Dual_Board_RX, Dual_Board_RX_BUFF_SIZE);
            DMA_Cmd(DMA_Stream_Dual_Board_RX, ENABLE);
            if(this_time_rx_len == Dual_Board_RX_BUFF_SIZE)	//У�����
            {
                //��������
               Dual_Board_Data_Decode(Dual_Board_rx_buff[1]);
            }
        }
        else
        {
            //��������DMA
            DMA_Cmd(DMA_Stream_Dual_Board_RX, DISABLE);
            this_time_rx_len = DMA_GetCurrDataCounter(DMA_Stream_Dual_Board_RX);
            DMA_SetCurrDataCounter(DMA_Stream_Dual_Board_RX, Dual_Board_RX_BUFF_SIZE);
            DMA_Cmd(DMA_Stream_Dual_Board_RX, ENABLE);
            if(this_time_rx_len == Dual_Board_RX_BUFF_SIZE)	//У�����
            {
                //��������
               Dual_Board_Data_Decode(Dual_Board_rx_buff[0]);
            }
        }
    }
}