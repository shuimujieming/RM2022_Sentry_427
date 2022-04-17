#include "gimbal.h"




void Encoder_Data_Process(CAN_Data_TypeDef *encoder_data,short init_angle)
{
	if(init_angle + 4096 >8191)
	{
		encoder_data->Critical_MechAngle = init_angle + 4096 - 8192;
	}
	else
	{
		encoder_data->Critical_MechAngle = init_angle + 4096;
	}
}
//10226
void Gimbal_Reset()
{
	short Aim_Yaw_Angle = 10226;
	short Aim_Pitch_Angle = 10711;
		Aim_Pitch_Angle = CAN_Gimbal[1].Current_MechAngle;
		Aim_Yaw_Angle = CAN_Gimbal[0].Current_MechAngle;
	
	
	while(abs(CAN_Gimbal[0].Current_MechAngle - 10226)>8  || abs(CAN_Gimbal[1].Current_MechAngle - 10650)>8)
	{

	if(Aim_Pitch_Angle  < 10711 - 7)
		{
			Aim_Pitch_Angle += 3;
		}
		else if(Aim_Pitch_Angle  > 10711 + 7)
		{
			Aim_Pitch_Angle -= 3;
		}
	
		Pid_Calc(&PID_Gimbal_Angle[1],CAN_Gimbal[1].Current_MechAngle,Aim_Pitch_Angle);	
		CAN_Gimbal[1].Target_Current = Pid_Calc(&PID_Gimbal_Speed[1],mpu_data.gy*0.1f,PID_Gimbal_Angle[1].PIDout);

		
		
		if(Aim_Yaw_Angle  < 10226 - 7)
		{
			Aim_Yaw_Angle += 8;
		}
		else if(Aim_Yaw_Angle  > 10226 + 7)
		{
			Aim_Yaw_Angle -= 8;
		}


		
		Pid_Calc(&PID_Gimbal_Angle[0],CAN_Gimbal[0].Current_MechAngle,Aim_Yaw_Angle);
		
		CAN_Gimbal[0].Target_Current = Pid_Calc(&PID_Gimbal_Speed[0],mpu_data.gz*0.1f,PID_Gimbal_Angle[0].PIDout);

		CAN1_TX_Gimbal();	// CANÊý¾Ý·¢ËÍ
		
	}
	
	
}

void Gimbal_Init()
{
		Pid_Reset(&PID_Gimbal_Angle[0]);
		Pid_Reset(&PID_Gimbal_Angle[1]);
		Pid_Reset(&PID_Gimbal_Speed[0]);
		Pid_Reset(&PID_Gimbal_Speed[1]);
	
//	
//			Pid_Set(&PID_Gimbal_Angle[1],0.35,0,0,10000,0,30000,10000,1,30000,0,2);	//Pitch
//		Pid_Set(&PID_Gimbal_Speed[1],60,1.2,30,30000,5000,30000,30000,1,30000,0,2); //Pitch
//		PID_Gimbal_Speed[1].PIDOutCompensate = -7500;
//	
//	//5 0 0 
//	// 250 0 0
//	
//		//2034
//		Pid_Set(&PID_Gimbal_Angle[0],0.09,0,0,10000,3000,30000,10000,1,30000,0,2);	//Yaw
//		Pid_Set(&PID_Gimbal_Speed[0],40,0.6,10,30000,5000,200,30000,1,30000,0,2); //Yaw
		//2560
	//1.2 0 0
	//150 0 0
	
	//0.2
	//400
	
		Pid_Set(&PID_Gimbal_Angle[1],0.3,0,0,10000,0,30000,10000,1,30000,0,2);	//Pitch
		Pid_Set(&PID_Gimbal_Speed[1],150,0,0,30000,5000,30000,30000,1,30000,0,2); //Pitch
		PID_Gimbal_Speed[1].PIDOutCompensate = -7500;
	
	//5 0 0 
	// 250 0 0
	
		//2034
		Pid_Set(&PID_Gimbal_Angle[0],0.25,0,0,10000,3000,30000,10000,1,30000,0,2);	//Yaw
		Pid_Set(&PID_Gimbal_Speed[0],60,0,0,30000,5000,200,30000,1,30000,0,2); //Yaw
	
		Encoder_Data_Process(&CAN_Gimbal[0],YAW_ANGLE_INIT);
		
		Encoder_Data_Process(&CAN_Gimbal[1],PITCH_ANGLE_INIT);
		
		delay_ms(10);
		
}



