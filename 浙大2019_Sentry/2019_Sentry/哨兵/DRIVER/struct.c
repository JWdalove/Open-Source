#include "struct.h"

//��̨PID����
PID_Type 	Pitch_In_PID , Pitch_Out_PID , Pitch_Heat_In_PID , Pitch_Heat_Out_PID ,
Yaw_In_PID , Yaw_Out_PID , Yaw_Heat_In_PID , Yaw_Heat_Out_PID,Chassis_Follow_PID;

//����PID����
PID_Type	                   Poked_InPID , Poked_OutPID;

//������Ϣ
Chassis_info	               Chassis;

//��̨��Ϣ
Cloud_info		               Yaw,Pitch;

//������Ϣ
Poked_info		               Poked;

//�Ӿ�ʶ��������Ϣ,����
imagepro_info 			         MiniPC;

//�ڱ���Ϣ
Sentry_info 			           SentryInfo;

//ͨѶ��Ϣ
Communicate_info 		         Comm;
//λ��
POSITION				             Position;

//֡�ʼ��
FRAME_RATE				           FrameRate;

//����ģʽ
ControlMode_Enum		         Control_Mode;

//Ħ����״̬
FricStatus_Enum			         FricStatus;

//�˶�ģʽ
MotionMode_Enum			         MontionMode;

//���̴��ģʽ
ChassisMode_Enum   		       Chassis_Heat_Mode;

//�Զ��˶�ģʽ
HeatSubschema_Enum           GimbalMotionMode;

//�Զ���̨ģʽ
GimbalControl_Mode_Enum      Gimbal_Control;

//����ϵͳ��Ϣ
Judge_rece				           Judge_DATA;

//ң����������Ϣ
DBUSDecoding_Type            DBUS_ReceiveData,LASTDBUS_ReceiveData;

//��С���˷�����
Least_Square_INFO		         Least_Square_Info;
