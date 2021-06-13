#include "Cloud_Control.h"
#include "kalman.h"
#include "delay.h"
extern Sentry_info			SentryInfo;
extern Cloud_info			Yaw,Pitch;
extern Chassis_info  		Chassis;
extern imagepro_info 		MiniPC;
extern PID_Type				Poked_InPID,Poked_OutPID;
extern PID_Type				Pitch_In_PID,Pitch_Out_PID,Yaw_In_PID,Yaw_Out_PID;
extern PID_Type				Pitch_Heat_In_PID,Pitch_Heat_Out_PID,Yaw_Heat_In_PID,Yaw_Heat_Out_PID;
extern HeatSubschema_Enum	GimbalMotionMode;

extern float  speed_Euler_Pitch,speed_Euler_Yaw,Euler_Yaw;
extern float  speed_Euler_Yaw_last,speed_Euler_Pitch_last;
int Heat_CloudOutMax=3000;						      //��̨����޷�
u8 Offset_flag_f=0 , Offset_flag_b=0;				//����������־λ
float Yaw_Offset = 0.0f , Chassis_Offset = 0.0f;		//��̨����
float m=0.0f , n=0.0f;
float Actual_Distance=0.0f;
float PC_Distance=0.0f;
float Pitch_Offset=0.0f;

/***************************************
  * @brief  :������ʼ��

****************************************/
void Cloud_PID_Param(void)
{
	MiniPC.Correction_Yaw = -0.01f;
	Yaw.Plus = 0.0f;
	Pitch.Plus = 0.0f;
	Yaw.rotate_coe = 2.0f;
	Pitch.rotate_coe = 3.0f;
	
	Yaw_In_PID.P = 3.0f;
	Yaw_In_PID.D = 0.0f;
	Yaw_In_PID.feed_param = 0.0f;
	Yaw_Out_PID.P = 100.0f;
	Yaw_Out_PID.I = 0.0f;
	Yaw_Out_PID.D = 0.0f;
	
	Yaw_Heat_In_PID.P = 2.0f;//2.5//2.0
	Yaw_Heat_In_PID.D = 0.0f;
	Yaw_Heat_In_PID.feed_param = 0.0f;
	Yaw_Heat_Out_PID.P = 80.0f;//160.0//80.0
	Yaw_Heat_Out_PID.I = 0.0f;
	Yaw_Heat_Out_PID.D = 0.0f;
	
	Pitch_In_PID.P = -5.0f;
	Pitch_In_PID.D = 0.0f;
	Pitch_In_PID.feed_param = 0.0;
	Pitch_Out_PID.P = -40.0f;
	Pitch_Out_PID.I = -0.3f;
	Pitch_Out_PID.D = 0.0f;
	
	Pitch_Heat_In_PID.P = -6.5f;//-4.5//-6.5
	Pitch_Heat_In_PID.I = 0.0f;
	Pitch_Heat_In_PID.D = 0.0f;
	Pitch_Heat_In_PID.feed_param = 0.0;
	Pitch_Heat_Out_PID.P = -15.0f;//-50.0//-15.0
	Pitch_Heat_Out_PID.I = -0.4f;//-0.4 //-0.4
	
	Poked_InPID.P  = 3.0f;
	Poked_OutPID.P = 1.5f;
	Poked_OutPID.D = 15.0f;
	
	Pitch.Give = Pitch.Imu_Euler;
	Yaw.Give = Yaw.Imu_Euler;
}



// ********** �� ��̨�Ƕȿ��� �� **********//
void Cloud_Angle_Set(int16_t Yaw_Angle,float Y_Sensity, int16_t Pitch_Angle,float P_Sensity)
{
	Yaw_Offset  = 0.0f;       //��������
  Yaw.Give   += (Yaw_Angle * Y_Sensity);
	Pitch.Give -= (Pitch_Angle * P_Sensity)*360.0f/8191.0f;
}
// ********** �� ��̨�Ƕȿ��� �� **********//



/***************************************
  * @brief  :����Ԥ�ⲹ��
  * @param  :Offset_flag_f,Offset_flag_b����������־;	MiniPC.Angle_error[]��Ŀ��ĽǶȲ�ֵ����
				     Yaw_Offset����	MiniPC.EnemyErrorǰ5(�����������)�ε��˽Ƕ������ڵ��˽ǶȵĲ�ֵ
  * @details:
****************************************/
// ********** �� ��̨���ģʽ�Ƕȿ��� �� **********//
void GimbalHeat_AngleSet(float Yaw_Kalman,float Pitch_Kalman,float Yaw_Speed_Kalman)
{

}

// ********** �� ��̨���ģʽ�Ƕȿ��� �� **********//



// ********** �� ��̨Pitch����� �� **********//
int16_t Cloud_Control_Pitch(float current_position_206,float target_position_206)
{
    portTickType CurrentTick = xTaskGetTickCount();
    static float target_position_206_last = 0.0f;
	
    // ********** �� Pitch���⻷PID �� **********//
    Pitch_Out_PID.LastError = Pitch_Out_PID.CurrentError;
    Pitch_Out_PID.CurrentError =  -current_position_206 + target_position_206;
    if(abs_f(Pitch_Out_PID.CurrentError) < 600)
        Pitch_Out_PID.Integral += Pitch_Out_PID.CurrentError;
    else
        Pitch_Out_PID.Integral = 0;
    Pitch_Out_PID.Integral = Pitch_Out_PID.Integral > 2000 ? 2000 : Pitch_Out_PID.Integral;
    Pitch_Out_PID.Integral = Pitch_Out_PID.Integral < -2000 ? -2000 : Pitch_Out_PID.Integral;
    Pitch_Out_PID.PIDOut = Pitch_Out_PID.CurrentError * Pitch_Out_PID.P
						 + Pitch_Out_PID.Integral * Pitch_Out_PID.I
						 +(Pitch_Out_PID.CurrentError-Pitch_Out_PID.LastError)*Pitch_Out_PID.D;
    // ********** �� Pitch���⻷PID �� **********//
		
    // ********** �� Pitch���ڻ�PID �� **********//
    Pitch_In_PID.LastError = Pitch_In_PID.CurrentError;
    Pitch_In_PID.CurrentError = -speed_Euler_Pitch + Pitch_Out_PID.PIDOut;
    Pitch_In_PID.PIDOut = Pitch_In_PID.CurrentError * Pitch_In_PID.P
                          + ( speed_Euler_Pitch_last - speed_Euler_Pitch) *  Pitch_In_PID.D /(CurrentTick - Pitch_In_PID.LastTick)      //����Ҫע�ⷽ�� ���˴�δʵ����
                          +(target_position_206-target_position_206_last)*Pitch_In_PID.feed_param;      //ǰ��ϵ������   �������ŷ���  ΢����ʱ��Ĺ�ϵ ���յ��
    Pitch_In_PID.PIDOut = Pitch_In_PID.PIDOut > 14000 ? 14000 : Pitch_In_PID.PIDOut;
    Pitch_In_PID.PIDOut = Pitch_In_PID.PIDOut < -14000 ? -14000 : Pitch_In_PID.PIDOut;
		// ********** �� Pitch���ڻ�PID �� **********//
		
    target_position_206_last = target_position_206;
    speed_Euler_Pitch_last = speed_Euler_Pitch;                  //������˴��߼��Ƿ�����Լ��Ƿ�Ӧ�ý�����������Ƕ�����
    //�������쵼�µ�������Ÿı���ʱɾ��
    Pitch_In_PID.LastTick = CurrentTick;

    return Pitch_In_PID.PIDOut;
}
// ********** �� ��̨Pitch����� �� **********//



// ********** �� ��̨Yaw����� �� **********//
int16_t Cloud_Control_Yaw(float current_position_205,float target_position_205)
{
    static float target_position_205_last = 0.0f;

    // ********** �� Yaw���⻷PID �� **********//
    Yaw_Out_PID.LastError = Yaw_Out_PID.CurrentError;
    Yaw_Out_PID.CurrentError = -target_position_205 + current_position_205;
	if(abs_f(Yaw_Out_PID.CurrentError) < 10)
        Yaw_Out_PID.Integral += Yaw_Out_PID.CurrentError;
    else
        Yaw_Out_PID.Integral = 0;
    Yaw_Out_PID.PIDOut = Yaw_Out_PID.CurrentError * Yaw_Out_PID.P
						 + Yaw_Out_PID.Integral * Yaw_Out_PID.I
						 + (Yaw_Out_PID.CurrentError-Yaw_Out_PID.LastError)*Yaw_Out_PID.D;
    // ********** �� Yaw���⻷PID �� **********//
		
    // ********** �� Yaw���ڻ�PID �� **********//
    Yaw_In_PID.LastError = Yaw_In_PID.CurrentError;
    Yaw_In_PID.CurrentError = -Yaw_Out_PID.PIDOut + speed_Euler_Yaw ;
    Yaw_In_PID.PIDOut = Yaw_In_PID.CurrentError * Yaw_In_PID.P
                        + ( speed_Euler_Yaw_last - speed_Euler_Yaw) *  Yaw_In_PID.D  //����Ҫע�ⷽ�� ���˴�δʵ����
                        +(target_position_205-target_position_205_last)*Yaw_In_PID.feed_param;     //ǰ��ϵ������   �������ŷ���  ΢����ʱ��Ĺ�ϵ ���յ��
    if(GimbalMotionMode == Arm_Revenge_Mode)
    {
        Yaw_In_PID.PIDOut = Yaw_In_PID.PIDOut > CloudOutMax*0.4 ? CloudOutMax*0.4 : Yaw_In_PID.PIDOut;
        Yaw_In_PID.PIDOut = Yaw_In_PID.PIDOut < -CloudOutMax*0.4 ? -CloudOutMax*0.4 : Yaw_In_PID.PIDOut;
    }
    else
    {
        Yaw_In_PID.PIDOut = Yaw_In_PID.PIDOut > CloudOutMax ? CloudOutMax : Yaw_In_PID.PIDOut;
        Yaw_In_PID.PIDOut = Yaw_In_PID.PIDOut < -CloudOutMax ? -CloudOutMax : Yaw_In_PID.PIDOut;
    }
		// ********** �� Pitch���ڻ�PID �� **********//
		
    target_position_205_last = target_position_205;
    speed_Euler_Yaw_last = speed_Euler_Yaw;                  //������˴��߼��Ƿ�����Լ��Ƿ�Ӧ�ý�����������Ƕ�����
    
		return Yaw_In_PID.PIDOut;
}
// ********** �� ��̨Yaw����� �� **********//



// ********** �� ��̨���ģʽPitch��PID���� �� **********//
int16_t Pitch_Heat_Control(float current_position_206,float target_position_206)
{
    portTickType CurrentTick = xTaskGetTickCount();
    
	  // ********** �� Pitch���⻷PID �� **********//
    Pitch_Heat_Out_PID.LastError = Pitch_Heat_Out_PID.CurrentError;
    Pitch_Heat_Out_PID.CurrentError =  -current_position_206 + target_position_206;
    if(abs_f(Pitch_Heat_Out_PID.CurrentError) < 100)
        Pitch_Heat_Out_PID.Integral += Pitch_Heat_Out_PID.CurrentError;
    else
        Pitch_Heat_Out_PID.Integral = 0;
    Pitch_Heat_Out_PID.Integral = Pitch_Heat_Out_PID.Integral > 3000 ? 3000:Pitch_Heat_Out_PID.Integral ;
    Pitch_Heat_Out_PID.Integral = Pitch_Heat_Out_PID.Integral < -3000 ? -3000:Pitch_Heat_Out_PID.Integral ;
    Pitch_Heat_Out_PID.PIDOut = Pitch_Heat_Out_PID.CurrentError * Pitch_Heat_Out_PID.P + Pitch_Heat_Out_PID.Integral * Pitch_Heat_Out_PID.I;
    // ********** �� Pitch���⻷PID �� **********//
		
    // ********** �� Pitch���ڻ�PID �� **********//
    Pitch_Heat_In_PID.LastError = Pitch_Heat_In_PID.CurrentError;
    Pitch_Heat_In_PID.CurrentError = -speed_Euler_Pitch + Pitch_Heat_Out_PID.PIDOut;
    //���ַ���
    if(abs_f(Pitch_Heat_In_PID.CurrentError) < 400)
        Pitch_Heat_In_PID.Integral += Pitch_Heat_In_PID.CurrentError;
    else
        Pitch_Heat_In_PID.Integral = 0;
    Pitch_Heat_In_PID.Integral = Pitch_Heat_In_PID.Integral > 1500 ? 1500:Pitch_Heat_In_PID.Integral ;
    Pitch_Heat_In_PID.Integral = Pitch_Heat_In_PID.Integral < -1500 ? -1500:Pitch_Heat_In_PID.Integral;
    Pitch_Heat_In_PID.PIDOut = Pitch_Heat_In_PID.CurrentError * Pitch_Heat_In_PID.P \
                               + Pitch_Heat_In_PID.Integral * Pitch_Heat_In_PID.I \
                               + (speed_Euler_Pitch_last - speed_Euler_Pitch) *  Pitch_Heat_In_PID.D /(CurrentTick - Pitch_Heat_In_PID.LastTick);
    Pitch_Heat_In_PID.PIDOut = Pitch_Heat_In_PID.PIDOut > CloudOutMax ? CloudOutMax : Pitch_Heat_In_PID.PIDOut;
    Pitch_Heat_In_PID.PIDOut = Pitch_Heat_In_PID.PIDOut < -CloudOutMax ? -CloudOutMax : Pitch_Heat_In_PID.PIDOut;
		// ********** �� Pitch���ڻ�PID �� **********//
    
		speed_Euler_Pitch_last = speed_Euler_Pitch;
    Pitch_Heat_In_PID.LastTick = CurrentTick;
		
    return Pitch_Heat_In_PID.PIDOut;
}
// ********** �� ��̨���ģʽPitch��PID���� �� **********//



// ********** �� ��̨���ģʽYaw��PID���� �� **********//
int16_t Yaw_Heat_Control(float current_position_205,float target_position_205)
{
    static float target_position_205_last = 0.0f;
	  portTickType CurrentTick = xTaskGetTickCount();
	
    // ********** �� Yaw���⻷PID �� **********//
    Yaw_Heat_Out_PID.LastError = Yaw_Heat_Out_PID.CurrentError;
    Yaw_Heat_Out_PID.CurrentError =  -target_position_205 + current_position_205;
	if(abs_f(Yaw_Heat_Out_PID.CurrentError) < 3)
        Yaw_Heat_Out_PID.Integral += Yaw_Heat_Out_PID.CurrentError;
    else
        Yaw_Heat_Out_PID.Integral = 0;
    Yaw_Heat_Out_PID.PIDOut = Yaw_Heat_Out_PID.CurrentError * Yaw_Heat_Out_PID.P
							+ Yaw_Heat_Out_PID.Integral * Yaw_Heat_Out_PID.I
							+(Yaw_Heat_Out_PID.CurrentError -  Yaw_Heat_Out_PID.LastError) * Yaw_Heat_Out_PID.D;
    // ********** �� Yaw���⻷PID �� **********//
		
    // ********** �� Yaw���ڻ�PID �� **********//
    Yaw_Heat_In_PID.LastError = Yaw_Heat_In_PID.CurrentError;
    Yaw_Heat_In_PID.CurrentError =  -speed_Euler_Yaw + Yaw_Heat_Out_PID.PIDOut;
    Yaw_Heat_In_PID.PIDOut = Yaw_Heat_In_PID.CurrentError * Yaw_Heat_In_PID.P
                             + ( speed_Euler_Yaw_last - speed_Euler_Yaw) *  Yaw_Heat_In_PID.D
                             + (target_position_205-target_position_205_last)*Yaw_Heat_In_PID.feed_param;
		Yaw_Heat_In_PID.PIDOut = Yaw_Heat_In_PID.PIDOut > Heat_CloudOutMax ? Heat_CloudOutMax : Yaw_Heat_In_PID.PIDOut;
    Yaw_Heat_In_PID.PIDOut = Yaw_Heat_In_PID.PIDOut < -Heat_CloudOutMax ? -Heat_CloudOutMax : Yaw_Heat_In_PID.PIDOut;
    // ********** �� Yaw���ڻ�PID �� **********//
		
    target_position_205_last = target_position_205;
    speed_Euler_Yaw_last = speed_Euler_Yaw;
	  Yaw_Heat_In_PID.LastTick = CurrentTick;

    return -Yaw_Heat_In_PID.PIDOut;
}
// ********** �� ��̨���ģʽYaw��PID���� �� **********//
