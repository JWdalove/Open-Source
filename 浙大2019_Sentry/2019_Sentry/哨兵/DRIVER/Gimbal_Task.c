#include "Gimbal_Task.h"

extern imagepro_info MiniPC;
extern Sentry_info 	SentryInfo;
extern Cloud_info	Yaw,Pitch;
extern Poked_info	Poked;
extern PID_Type 	Pitch_Out_PID,Yaw_Out_PID,Poked_OutPID;
extern Communicate_info 		Comm;
extern GimbalControl_Mode_Enum  Gimbal_Control;
extern DBUSDecoding_Type  		DBUS_ReceiveData,LASTDBUS_ReceiveData;
float Watchscope[4];			  //J-scope���ι۲�����
int Time_Counter = 0,Time2Counter=0;
int Time_PC = 1;
uint8_t JumpToRCFlag = 1,		//״̬��ת��־λ ��ʼ��һΪ�����������һ��ģʽ
		JumpToFloowFlag = 1,
		JumpToHeatFlag = 1,
		JumpToRelaxFlag = 1,
		JumpToHoldFlag = 1,
		JumpToPoked = 1;

// ********** �� ��̨���� �� **********//
void Gimbal_Task(void *Parameters)
{
    portTickType CurrentControlTick = 0;
    for(;;)
    {
        CurrentControlTick = xTaskGetTickCount();
			
    // ********** �� �������˲� �� **********//
        kalman_filter_calc(&motionfliter,Yaw.EnemyDataBuffer[0],Pitch.EnemyDataBuffer[0],0,0 );
        memcpy(SentryInfo.motion_kalman,motionfliter.filtered_value,sizeof(SentryInfo.motion_kalman));
	// ********** �� �������˲� �� **********//
		
		if(Time_PC>6)Time_PC=1;
		PC_Communicate(Yaw.Imu_Euler,Time_PC);
		Time_PC++;

	// ********** �� ʹ��J��scope�۲������ �� **********//
		Watchscope[0] = MiniPC.MiniPC_Msg[1];
		Watchscope[1] = Yaw.EnemyDataBuffer[0];
		Watchscope[2] = Yaw.Imu_Euler;
		Watchscope[3] = SentryInfo.motion_kalman[0];
    // ********** �� ʹ��J��scope�۲������ �� **********//
		
		Time_Counter++;
		if(Time_Counter>5000)Time_Counter = 0;
		Time2Counter++;
		if(Time2Counter>5000)Time2Counter = 0;
		
	// ********** �� ����״̬���� �� **********//
		Control_Status_Update(CurrentControlTick); 
    // ********** �� ����״̬���� �� **********//
		
	// ********** �� Ħ���ֿ��� �� **********//
		FricStatus_Control();
	// ********** �� Ħ���ֿ��� �� **********//
		
	// ********** �� ��̨����ģʽ �� **********//
        switch(Gimbal_Control)
        {
				 //��̨��λ
         case GIMBAL_INIT:{
			        if(Time_Counter%400==0)
			         {
				        __set_FAULTMASK(1);
				        NVIC_SystemReset();
			         }
		        break;}
				 //��̨�ͷ�
         case GIMBAL_RELAX: {
                    GIMBAL__RELAX();
                break;}
				 //RCң����
         case  GIMABL_RC: {
                    GIMABL__RC();
                break;}
				 //��̨����
		 case  GIMBAL_FLOOW:{
                    GIMBAL__FLOOW();
			    break;}
				 //��̨���
         case  GIMBAL_HEAT:{
			        GIMBAL__HEAT();
                break;}
        }
	// ********** �� ��̨����ģʽ �� **********//
		
	// ********** �� Pitch���޷� �� **********//
	  Pitch.Give=Pitch.Give > 2250 ? 2250:Pitch.Give;
	  Pitch.Give=Pitch.Give < 1350 ? 1350:Pitch.Give;
  	// ********** �� Pitch���޷� �� **********//
				
	// ********** �� ���ͨ�� �� **********//
	  CloudMotorDriveCan1(Yaw.Out,0,0,0);
	  CloudMotorDriveCan2(0,Pitch.Out,Poked.Out,0);
      Gimbal_Board_Communicate(Comm.GSendMotionMode,DBUS_ReceiveData.ch2,Pitch.Imu_Euler,0);
    // ********** �� ���ͨ�� �� **********//
				
      vTaskDelayUntil(&CurrentControlTick, 2/ portTICK_RATE_MS);  //2ms����  ������ʱ  �ͷ�CPUռ��Ȩ
    }
}
// ********** �� ��̨���� �� **********//

/***************************************
  * @brief  :����ģʽ
  * @param  :
****************************************/
void GIMBAL__RELAX()
{
	if(JumpToRelaxFlag)       //ģʽ��ʼ��
	{
		Yaw.Give = Yaw.Imu_Euler;
		Pitch_Out_PID.LastError = 0;
		Yaw_Out_PID.LastError = 0;
		Poked.Turn_Angle = Poked.Esc.USE_Machine;
	}
	JumpToRelaxFlag = 0;
	JumpToHoldFlag = 1;
	JumpToRCFlag = 1;
	JumpToFloowFlag = 1;
	JumpToHeatFlag = 1;
	Yaw.Out = 0;
	Pitch.Out = 0;
	Poked.Out = 0;
	TIM4->CCR1=2000;
	TIM4->CCR2=2000;
}
void GIMABL__RC()
{
	if(JumpToRCFlag)          //ģʽ��ʼ��
	{
		Yaw.Give = Yaw.Imu_Euler;
		Pitch.Give = Pitch.Esc.Machine_Angle;
		Pitch_Out_PID.LastError = 0;
		Yaw_Out_PID.LastError = 0;
		Poked_OutPID.LastError = 0;
		Poked.Turn_Angle = Poked.Esc.USE_Machine;
		GPIO_SetBits(GPIOE,GPIO_Pin_1);
	}
	JumpToRelaxFlag = 1;
	JumpToHoldFlag = 1;
	JumpToRCFlag = 0;
	JumpToFloowFlag = 1;
	JumpToHeatFlag = 1;

	Poke_Calculation(1);
	Poked.Out = Poked_Control(Poked.Esc.USE_Machine, Poked.Turn_Angle);

	Cloud_Angle_Set(-DBUS_ReceiveData.ch0,0.0002,DBUS_ReceiveData.ch1,0.03);
	Yaw.Out = Cloud_Control_Yaw(Yaw.Imu_Euler,Yaw.Give);
	Pitch.Out = Cloud_Control_Pitch(Pitch.Esc.Machine_Angle,Pitch.Give);
}
void GIMBAL__FLOOW()
{
	if(JumpToFloowFlag)       //ģʽ��ʼ��
	{
		Yaw.Give = Yaw.Imu_Euler;
		Pitch.Give = Pitch.Esc.Machine_Angle;
		Pitch_Out_PID.LastError = 0;
		Yaw_Out_PID.LastError = 0;
		Poked_OutPID.LastError = 0;
		Poked.Turn_Angle = Poked.Esc.USE_Machine;
		GPIO_SetBits(GPIOE,GPIO_Pin_1);
	}
	JumpToRelaxFlag = 1;
	JumpToHoldFlag = 1;
	JumpToRCFlag = 1;
	JumpToFloowFlag = 0;
	JumpToHeatFlag = 1;


	Poked.Out = 0;	
	if(Pitch.Esc.Machine_Angle>2100)Pitch.rotate_coe = -3.0f;
	if(Pitch.Esc.Machine_Angle<1500)Pitch.rotate_coe = 3.0f;
	if(DBUS_ReceiveData.switch_right == 1)
	Yaw.Give += 0.10f*Yaw.rotate_coe;
	Pitch.Give += Pitch.rotate_coe;
	if(Yaw.Imu_Euler-Yaw.Give>200 || Yaw.Imu_Euler-Yaw.Give<-200)
	{
		Yaw.Out = 0;
		Pitch.Out = 0;
		Yaw.Give = Yaw.Imu_Euler;
		Pitch.Give = Pitch.Esc.Machine_Angle;
	}
	else
	{
		Yaw.Out = Cloud_Control_Yaw(Yaw.Imu_Euler,Yaw.Give);
		Pitch.Out = Cloud_Control_Pitch(Pitch.Esc.Machine_Angle,Pitch.Give);
	}
	
}
void GIMBAL__HEAT()
{
	if(JumpToHeatFlag)        //ģʽ��ʼ��
	{
		Pitch_Out_PID.LastError = 0;
		Yaw_Out_PID.LastError = 0;
		Poked_OutPID.LastError = 0;
		Poked.Turn_Angle = Poked.Esc.USE_Machine;
		GPIO_SetBits(GPIOE,GPIO_Pin_1);
	}
	JumpToRelaxFlag = 1;
	JumpToHoldFlag = 1;
	JumpToRCFlag = 1;
	JumpToFloowFlag = 1;
	JumpToHeatFlag = 0;

	Poke_Calculation(1);
	Poked.Out = Poked_Control(Poked.Esc.USE_Machine, Poked.Turn_Angle);
	
	GimbalHeat_AngleSet(SentryInfo.motion_kalman[0],SentryInfo.motion_kalman[1],SentryInfo.motion_kalman[2]);
	Yaw.Out = Yaw_Heat_Control(Yaw.Imu_Euler,Yaw.Give+Yaw.Plus);
	Pitch.Out = Pitch_Heat_Control(Pitch.Esc.Machine_Angle,Pitch.Give /*-Pitch_Offset*/ );
}
