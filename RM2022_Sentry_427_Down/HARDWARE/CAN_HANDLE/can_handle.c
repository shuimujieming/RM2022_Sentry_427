#include "can_handle.h"

//����
CAN_Data_TypeDef CAN_Chassis[4];

//��̨
CAN_Data_TypeDef CAN_Gimbal[2];	//Yaw 205 Pitch 206

//����
CAN_Data_TypeDef CAN_Shoot[2];	//Trigger 0x206 Left_Fric 0x203 Right_Fric 0x204
CAN_Data_TypeDef CAN_Trigger;

unsigned char CAN1_Tx_Message_Flag = 0;									//CAN1������Ϣ��־
unsigned char CAN2_Tx_Message_Flag = 0;									//CAN2������Ϣ��־
/*
*@title��CAN1�����жϺ���
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void CAN1_TX_IRQHandler(void)
{  
  if (CAN_GetITStatus (CAN1,CAN_IT_TME)!=RESET)                             
	{
		CAN1_Tx_Message_Flag=1;
		CAN_ClearITPendingBit(CAN1,CAN_IT_TME);    
	}
}


/*
*@title��CAN2�����жϺ���
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void CAN2_TX_IRQHandler(void)
{  
  if (CAN_GetITStatus (CAN2,CAN_IT_TME)!=RESET)                            
	{
		CAN2_Tx_Message_Flag=1;
		CAN_ClearITPendingBit(CAN2,CAN_IT_TME);    
	}
}


/*
*@Description��CAN���ݽ���
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void CAN_Data_Decode(CAN_Data_TypeDef *CAN_Data,CanRxMsg *CAN_Rx_Message)
{
	CAN_Data->Origin_MechAngle = (CAN_Rx_Message->Data[0]<<8)|(CAN_Rx_Message->Data[1]);
	CAN_Data->Current_Speed 					= (CAN_Rx_Message->Data[2]<<8)|(CAN_Rx_Message->Data[3]);
	CAN_Data->Current								  = (CAN_Rx_Message->Data[4]<<8)|(CAN_Rx_Message->Data[5]);
	CAN_Data->Temperature					    = (CAN_Rx_Message->Data[6]);
	
	//��ǰ����Ϊ��̨���ʹ�ã�����Ƕ�����
	if(CAN_Data->Critical_MechAngle !=0)
	{
		if(CAN_Data->Origin_MechAngle < CAN_Data->Critical_MechAngle)
		{
			CAN_Data->Current_MechAngle = CAN_Data->Origin_MechAngle + 8192;
		}
		else
		{
			CAN_Data->Current_MechAngle = CAN_Data->Origin_MechAngle;
		}		
	}
	//��ͨ���ʹ��
	else
	{
			CAN_Data->Current_MechAngle = CAN_Data->Origin_MechAngle;		
	}
}
/*
*@title��CAN1�����жϺ���
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void CAN1_RX0_IRQHandler(void)                                             
{   
	CanRxMsg CAN1_Rx_Message;	
	if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET)
	{
		CAN_Receive(CAN1,CAN_FIFO0,&CAN1_Rx_Message);
		if ( (CAN1_Rx_Message.IDE == CAN_Id_Standard) && (CAN1_Rx_Message.RTR == CAN_RTR_Data) && (CAN1_Rx_Message.DLC == 8) )//��׼֡������֡�����ݳ���Ϊ8�ֽ�
		{
			switch (CAN1_Rx_Message.StdId)
			{
				case 0x201:
				case 0x202:
				case 0x203:
				{
					CAN_Data_Decode(&CAN_Shoot[0],&CAN1_Rx_Message);
				}break;
				case 0x204:
				{
					CAN_Data_Decode(&CAN_Shoot[1],&CAN1_Rx_Message);
				}break;
				
				case 0x205:
				{
					CAN_Data_Decode(&CAN_Gimbal[0],&CAN1_Rx_Message);
				}break;

				case 0x206:
				{
					CAN_Data_Decode(&CAN_Gimbal[1],&CAN1_Rx_Message);
				}break;		
				
				default:
				{
					break;
				}   
			}
		}
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);		
	}
}
/*
*@title��CAN2�����жϺ���
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void CAN2_RX0_IRQHandler(void)                                                 
{   
	CanRxMsg CAN2_Rx_Message;	
	if (CAN_GetITStatus(CAN2,CAN_IT_FMP0)!= RESET)
	{
		CAN_Receive(CAN2,CAN_FIFO0,&CAN2_Rx_Message);

		if ( (CAN2_Rx_Message.IDE == CAN_Id_Standard) && (CAN2_Rx_Message.RTR == CAN_RTR_Data) && (CAN2_Rx_Message.DLC == 8) )//��׼֡������֡�����ݳ���Ϊ8�ֽ�
		{
			switch (CAN2_Rx_Message.StdId)
			{
				default:
				{
					break; 
				}
					
			}
		}
		CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);		
	}
}

/*
*@title��CAN1�������ݷ���
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void CAN1_TX_Chassis(void)
{
	CanTxMsg CAN1_Tx_Message;

	CAN1_Tx_Message.IDE = CAN_ID_STD;                                               //��׼֡
	CAN1_Tx_Message.RTR = CAN_RTR_DATA;                                             //����֡
	CAN1_Tx_Message.DLC = 0x08;                                                     //֡����Ϊ8
	CAN1_Tx_Message.StdId = 0x200;                               										//֡IDΪ���������CAN_ID

	CAN1_Tx_Message.Data[0] = (CAN_Chassis[0].Target_Current>>8)&0xff;             //201���յ�����8λ
	CAN1_Tx_Message.Data[1] = (CAN_Chassis[0].Target_Current)&0xff;                 //201���յ�����8λ
	CAN1_Tx_Message.Data[2] = (CAN_Chassis[1].Target_Current>>8)&0xff;             //202���յ�����8λ
	CAN1_Tx_Message.Data[3] = (CAN_Chassis[1].Target_Current)&0xff;                //202���յ�����8λ
	CAN1_Tx_Message.Data[4] = (CAN_Chassis[2].Target_Current>>8)&0xff;             //203���յ�����8λ
	CAN1_Tx_Message.Data[5] = (CAN_Chassis[2].Target_Current)&0xff;                 //203���յ�����8λ
	CAN1_Tx_Message.Data[6] = (CAN_Chassis[3].Target_Current>>8)&0xff;             //204���յ�����8λ
	CAN1_Tx_Message.Data[7] = (CAN_Chassis[3].Target_Current)&0xff;                 //204���յ�����8λ

	CAN1_Tx_Message_Flag = 0;
	CAN_Transmit(CAN1,&CAN1_Tx_Message);
	while(CAN1_Tx_Message_Flag == 0); 
}

/*
*@title��CAN1��̨���ݷ���
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void CAN1_TX_Gimbal(void)
{
	CanTxMsg CAN1_Tx_Message;

	CAN1_Tx_Message.IDE = CAN_ID_STD;                                               //��׼֡
	CAN1_Tx_Message.RTR = CAN_RTR_DATA;                                             //����֡
	CAN1_Tx_Message.DLC = 0x08;                                                     //֡����Ϊ8
	CAN1_Tx_Message.StdId = 0x1FF;                               //֡IDΪ���������CAN_ID

	CAN1_Tx_Message.Data[0] = (CAN_Gimbal[0].Target_Current>>8)&0xff;             //201���յ�����8λ
	CAN1_Tx_Message.Data[1] = (CAN_Gimbal[0].Target_Current)&0xff;                 //201���յ�����8λ
	CAN1_Tx_Message.Data[2] = (CAN_Gimbal[1].Target_Current>>8)&0xff;           
	CAN1_Tx_Message.Data[3] = (CAN_Gimbal[1].Target_Current)&0xff;             
	CAN1_Tx_Message.Data[4] = 0;            
	CAN1_Tx_Message.Data[5] = 0;               
	CAN1_Tx_Message.Data[6] = 0;            
	CAN1_Tx_Message.Data[7] = 0;              

	CAN1_Tx_Message_Flag = 0;
	CAN_Transmit(CAN1,&CAN1_Tx_Message);
	while(CAN1_Tx_Message_Flag == 0); 
}


/*
*@title��CAN1����������ݷ���
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void CAN1_TX_Shoot(void)
{
	CanTxMsg CAN1_Tx_Message;

	CAN1_Tx_Message.IDE = CAN_ID_STD;                                               //��׼֡
	CAN1_Tx_Message.RTR = CAN_RTR_DATA;                                             //����֡
	CAN1_Tx_Message.DLC = 0x08;                                                     //֡����Ϊ8
	CAN1_Tx_Message.StdId = 0x200;                               //֡IDΪ���������CAN_ID

	CAN1_Tx_Message.Data[0] = 0;             //201���յ�����8λ
	CAN1_Tx_Message.Data[1] = 0;                 //201���յ�����8λ
	CAN1_Tx_Message.Data[2] = 0;           
	CAN1_Tx_Message.Data[3] = 0;           
	CAN1_Tx_Message.Data[4] = (CAN_Shoot[0].Target_Current>>8)&0xff;           
	CAN1_Tx_Message.Data[5] = (CAN_Shoot[0].Target_Current)&0xff;             
	CAN1_Tx_Message.Data[6] = (CAN_Shoot[1].Target_Current>>8)&0xff;               
	CAN1_Tx_Message.Data[7] = (CAN_Shoot[1].Target_Current)&0xff;        

	CAN1_Tx_Message_Flag = 0;
	CAN_Transmit(CAN1,&CAN1_Tx_Message);
	while(CAN1_Tx_Message_Flag == 0); 
}

