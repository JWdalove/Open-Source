#include "Status_Update.h"

extern GimbalControl_Mode_Enum Gimbal_Control;
extern MotionMode_Enum		MontionMode;
extern FricStatus_Enum		FricStatus;
extern DBUSDecoding_Type	DBUS_ReceiveData,LASTDBUS_ReceiveData;
extern Communicate_info		Comm;
extern imagepro_info 		MiniPC;
extern Judge_rece			Judge_DATA;
extern Cloud_info			Yaw,Pitch;
extern Sentry_info 			SentryInfo;
extern FRAME_RATE			FrameRate;
extern HeatSubschema_Enum   GimbalMotionMode;
extern Chassis_info	   Chassis;

extern int Time2Counter; 

// ********** �� ����״̬���� �� **********//
void Control_Status_Update(portTickType Tick)
{
	static u8 Move_Flag=0;
	if(DBUS_ReceiveData.ch3==-660)Move_Flag=0;
	else if(DBUS_ReceiveData.ch3==660)Move_Flag=1;
	
#if Sentry == 0  //���߲���
    /*
    receive data :	5 MONTION_INIT     //��λ
					          2 MONTION_RELAX    //��ȫ
					          3 MONTION_RC       //ң����
					         1/4 MONTION_PATROL  //Ѳ��       
    */
	
	// ********** �� ���̽�����̨���͵�ģʽ �� **********//	
	if(Comm.CRecriveMotionMode == 5)
	{
		    MontionMode = MONTION_INIT;		    //��λ
	}
    else if(Comm.CRecriveMotionMode == 1 || Comm.CRecriveMotionMode == 4)
    {
        MontionMode = MONTION_PATROL;	    //Ѳ��
    }
    else if(Comm.CRecriveMotionMode == 3)
    {
        MontionMode = MONTION_RC; 		    //ң����
			  Chassis.initial_position=0;
    }
    else if(Comm.CRecriveMotionMode == 2)
    {
		    MontionMode = MONTION_RELAX;	    //��ȫ
    }
    else
    {
		    MontionMode = MONTION_RELAX;      //��ȫ
    }
  // ********** �� ���̽�����̨���͵�ģʽ �� **********//
		
#elif Sentry == 1   //��̨����

  // ********** �� ��̨���͸����̵�ģʽ �� **********//	 
	if( DBUS_ReceiveData.ch0==-660&&
		  DBUS_ReceiveData.ch1==-660&&
		  DBUS_ReceiveData.ch2== 660&&
		  DBUS_ReceiveData.ch3==-660  )
	    Comm.GSendMotionMode=5;                 //��λ
	else if(DBUS_ReceiveData.switch_left == 1)  //�Զ�
	{
		if(DBUS_ReceiveData.ch0== -660 && DBUS_ReceiveData.ch1== -660 )      //Ѳ��
			Comm.GSendMotionMode = 1;
		if(DBUS_ReceiveData.ch0==  660 && DBUS_ReceiveData.ch1== -660 )      //ң����
			Comm.GSendMotionMode = 3;
	}
	else if((DBUS_ReceiveData.switch_left == 3) || (DBUS_ReceiveData.switch_left == 1 && Move_Flag==0))
	{
		Comm.GSendMotionMode = 3;      //ң����
	}
	else if(DBUS_ReceiveData.switch_left == 2)
	{
		Comm.GSendMotionMode = 2;      //��ȫ
	}
  // ********** �� ��̨���͸����̵�ģʽ �� **********//
	
	
	
  // ********** �� ��̨ģʽ �� **********//	 	
	if( DBUS_ReceiveData.ch0==-660&&
		  DBUS_ReceiveData.ch1==-660&&
		  DBUS_ReceiveData.ch2== 660&&
		  DBUS_ReceiveData.ch3==-660  )
	    Gimbal_Control = GIMBAL_INIT;           //��λ
    else if(DBUS_ReceiveData.switch_left == 3)
    {
        Gimbal_Control = GIMABL_RC;		        //ң����
    }
	  else if(DBUS_ReceiveData.switch_left == 1)
	  {
		  if(DBUS_ReceiveData.switch_right == 2 || MiniPC.Enemy_Count>=3)
		  {
			  Gimbal_Control = GIMBAL_HEAT;         //���
		  }
		else if(DBUS_ReceiveData.switch_right == 1)
			  Gimbal_Control = GIMBAL_FLOOW;	      //Ѳ��
		else
			  Gimbal_Control = GIMBAL_FLOOW;        //Ѳ��
		  FricStatus = FricStatus_Working;        //Ħ���ֹ���
	  }
    else
    {
        Gimbal_Control = GIMBAL_RELAX;        //��ȫ
			  FricStatus = FricStatus_Stop;
    }
  // ********** �� ��̨ģʽ �� **********//

	// ********** �� ��̨ң����ģʽ��Ħ���ֿ��� �� **********//	
	if(Gimbal_Control == GIMABL_RC)
    {
        if( DBUS_ReceiveData.switch_right ==2 )
				{
            FricStatus = FricStatus_Stop;
        }
        else if( DBUS_ReceiveData.switch_right !=2 )
        {
            FricStatus = FricStatus_Working;
        }

    }
	// ********** �� ��̨ң����ģʽ��Ħ���ֿ��� �� **********//
	
#endif
	DBUS_ReceiveData.switch_left_last = DBUS_ReceiveData.switch_left;
  DBUS_ReceiveData.switch_right_last = DBUS_ReceiveData.switch_right;
}
// ********** �� ����״̬���� �� **********//
