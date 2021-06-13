#include "Status_Update.h"

extern GimbalControl_Mode_Enum Gimbal_Control;
extern MotionMode_Enum		MontionMode;
extern FricStatus_Enum		FricStatus;
extern DBUSDecoding_Type	DBUS_ReceiveData,LASTDBUS_ReceiveData;
extern Communicate_info		Comm;
extern Sentry_info 			SentryInfo;
uint8_t DBUSFrameRate=0;//gimbal_monitor
void Control_Status_Init(void)
{
//	Control_Mode = Auto_Control_Mode;
}


void Control_Status_Update(portTickType Tick)
{
    static int Revenge_Tick = -12000;

#if Sentry == 0  //���߲���
    /*
    receive data :	1 chassis rc
					3 chassis relax
    				2 chsssis patrol
    */
    if(Comm.CRecriveMotionMode == 1)
    {
        MontionMode = MONTION_RC; 			//ң��������
    }
    else if(Comm.CRecriveMotionMode == 3)
    {
        MontionMode = MONTION_RELAX;	//��ȫģʽ
    }
//    else if(Comm.CRecriveMotionMode == 2 || Comm.CRecriveMotionMode == 4)
//    {
//        MontionMode = MONTION_PATROL;	//ȫ�Զ�
//    }
    else if(Comm.CRecriveMotionMode == 0)
    {
        MontionMode = MONTION_RELAX;
    }
    else                            //Ĭ��ȫ�Զ�
    {
		MontionMode = MONTION_RELAX;
        /*//Ĭ��ȫ�Զ�   �˴���Ҫ�޸�   ��Ϊ������*/
    }
#elif Sentry == 1 //��̨����          
	 /*************���߲��ֿ�����Ϣ����**************/
	if(DBUS_ReceiveData.switch_left == 1)
    {
        Comm.GSendMotionMode = 1;			//���߲���ң�ؿ���
    }
	else if(DBUS_ReceiveData.switch_left == 2)
    {
        Comm.GSendMotionMode = 2;			//���߲���תΪѲ��ģʽ
        if(SentryInfo.nobullet)				//???????????????????????????????????????????
            Comm.GSendMotionMode = 4;
    }
    //��̨���ֿ���ģʽѡ��
    if(DBUS_ReceiveData.switch_left == 3 )
    {
        Gimbal_Control = GIMABL_RC;		//��̨ģʽתΪң�ؿ���
    }
    else
    {
        Gimbal_Control = GIMBAL_RELAX;	//follow
		if(USART2_MiniPC[5] == 1){
			Gimbal_Control = GIMBAL_HEAT;//תΪtoo too too ģʽ
		}
    }


	if(Gimbal_Control == GIMABL_RC)
    {
        /*******************  ��  Ħ���ֿ��Ʒ�ʽ  ��  *******************/
        if(DBUS_ReceiveData.switch_right == 2 && DBUS_ReceiveData.switch_right_last != 2)//���䵽2
        {
            FricStatus = FricStatus_Stop;
        }
        else if(DBUS_ReceiveData.switch_right == 3 && DBUS_ReceiveData.switch_right_last != 3)//���䵽3
        {
            FricStatus = FricStatus_Working;
        }

    }
//    else if(Gimbal_Control == GIMBAL_FLOOW || Gimbal_Control == GIMBAL_HEAT)
//    {
//        FricStatus = FricStatus_Working; //Ħ�������Զ�ģʽ��Ĭ�Ϲ���
//    }
    /*******************  ��  ��̨�Ƕ��趨  ��  *******************/
    // װ���յ��˺�
//    if(Gimbal_Control == GIMBAL_FLOOW)
//    {
//        if((((SentryInfo.remain_last_hp - judge_rece_mesg.game_information.remain_hp) >= 50)\
//                || ((xTaskGetTickCount() - Revenge_Tick) < 4500)) && xTaskGetTickCount() > 2000)
//        {
//            if((SentryInfo.remain_last_hp - judge_rece_mesg.game_information.remain_hp) >= 50)
//                Revenge_Tick = xTaskGetTickCount();

//            GimbalMotionMode = Arm_Revenge_Mode;   //װ�׸��� @ @

//            SentryInfo.remain_last_hp = judge_rece_mesg.game_information.remain_hp;
//        }
//        else
//        {
//            GimbalMotionMode = Auto_Normal_Mode;	//������ת
//        }

//    }
    /*******************  ��  ��̨�Ƕ��趨  ��  *******************/
#endif
	DBUS_ReceiveData.switch_left_last = DBUS_ReceiveData.switch_left;
    DBUS_ReceiveData.switch_right_last = DBUS_ReceiveData.switch_right;
}
