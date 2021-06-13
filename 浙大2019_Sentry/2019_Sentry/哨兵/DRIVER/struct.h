#ifndef __STRUCT_H__
#define __STRUCT_H__
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "arm_math.h"
#include "constant.h"

/*
*********************************************************************************************************
*                                           �ṹ���������
*********************************************************************************************************
*/

/*******************************  Task_CANSend  **********************************/
typedef struct
{
    uint8_t     CANx;            //CAN���     1 CAN1      2 CAN2
    CanTxMsg    TxMessage;       //��������
} CanSend_Type;
/****************************************   ��  ���������Ϣ ��  ***********************************/
typedef struct
{
    u8  FrameCounter;			  //����֡���
    u16 Machine_Angle; 			//��е�Ƕ�
    u16	Machine_Angle_Last;	//��һ�λ�е�Ƕ�
    int16_t Angle_Speed;		//ת�� ע�����޷��Ŷ����ͣ�����
    int USE_Machine;			  //ʹ�õĻ�е��Ϣ
    int Zero_Flag;				  //������־λ
    
} Esc820_info;              //���̵��

typedef struct
{
    u8  FrameCounter;			  //����֡���
    u16 Machine_Angle; 			//��е�Ƕ�
    u16	Machine_Angle_Last;	//��һ�λ�е�Ƕ�
    int16_t Revenge_Angle;
    int16_t Elec_Measure;		//ת�ص�������ֵ
    int16_t Elec_Give;			//ת�ص�������ֵ
	int16_t Rotate_Speed;
    int Relative_Angle;
    int USE_Machine;			  //ʹ�õĻ�е��Ϣ
    int Zero_Flag;				  //������־λ
    int Angle_Error;
    int16_t Init_Machine_Angle; //�ϵ��ʼ�Ƕ�
    u8  Init;
    u16 Last_Angle;
} Esc6623_info; //��̨���
/***************************************   ��  ���������Ϣ ��   ***********************************/
/****************************************   ��  PID���� ��  ***********************************/

typedef struct
{
    float P;
    float I;
    float D;
    float feed_param;     //ǰ��ϵ��
    float CurrentError;
    float LastError;
    float PIDOut;
    float Integral;
    portTickType LastTick;
} PID_Type;

/***************************************    ��  PID���� ��   ***********************************/
/****************************************   ��  ����   ��  ***********************************/
//���̵��������
typedef struct
{
    Esc820_info Esc;			      //���̵����Ϣ����
    int TargetSpeed;            //���Ŀ���ٶ�
    int Out;					          //���̵�����
    int Current_Error;          //���Ƶ���
} OneChassisParam_info;

//���� chassis
typedef struct
{
    OneChassisParam_info F;					//ǰ
    OneChassisParam_info B;					//��

	  int16_t Angle_Speed;
	  int   initial_position;
    float LastV;									  //��һ���ٶ�
    float TargetV;									//�����ٶ�   ǰ��
    float TargetVfliter;						//ƽ������˲�ֵ
    float TargetOmega;							//������ת�ٶ�	  ��ת
	  float PowerLimit;								//�������
} Chassis_info;



/***************************************    ��  ����   ��   ***********************************/
/****************************************   ��  ��̨   ��  ***********************************/
typedef struct
{
    float Give; 				   	//����ֵ
    int   Out;					   	//���ֵ
	  float Plus;						  //��������
    float Imu_Euler;		   	//������Euler
    float Imu_Euler_Fit;   	//У��Euler
    float rotate_coe; 	   	//��תϵ��
    u8    init_complete;	 	//��ʼ����ɱ�־λ
    float 		Machine_Angle_Max;    		//��е�Ƕ��޷�
    float 		Machine_Angle_Min; 	   		//��е�Ƕ��޷�
	  uint16_t 	Give_Max;    	 	//����ֵ�޷�
    uint16_t 	Give_Min; 	    //����ֵ�޷�
    Esc6623_info Esc;
    float		EnemyDataBuffer[50];//��ȡ��ȥ100������

} Cloud_info;



/***************************************    ��  ��̨   ��   ***********************************/
/****************************************   ��  ����   ��  ***********************************/

//���� poked
typedef struct
{
	int Turn_Angle;
  int Out;              //���ֵ
  int Num_Bullet;       //�ӵ�����
  int Num_Bullet_Last;
  Esc820_info Esc;
  int16_t Preview_n;

} Poked_info;
/***************************************    ��  ����   ��   ***********************************/
/****************************************   ��  �ڱ�������Ϣ ��  ***********************************/
typedef struct
{
  int mil;						   //�ڱ�λ��
  int last_mil;          //�ڱ��ϴ�λ��
  int dir;						   //�ڱ��˶�����
  int last_dir;				   //�ڱ���һ���˶����� ���ڵ����ٶȵ�ƽ������
  u8  infrared;				   //���⿪��״̬
	u8  infrared_last;
	float Yaw_Error;
	float Ultrasonic;		   //������1
	float Ultrasonic2;	   //������2
  int16_t Shoot_Freq;
  int16_t Shoot_Dis;
  float targetspeedvx;   //��̨Yaw����Ҫ���ٶ�  
  float targetspeedvy;   //��̨Pitch����Ҫ���ٶ�
  float targetspeedvx_last;
  float targetspeedvy_last;
  float motion_kalman[4];
  enum  Pos {Behind_Edge,Behind,Behind_Curve,Rail_Mid,Front_Curve,Front,Front_Edge,} Position,LastPosition;						//�ڱ����̶�λ
  enum  Origin_D {Rail_MidBehind,Rail_MidFront, }	Origin;																															//�ڱ���ʼԭ��
  enum  Rail_Direction {B_TO_F,F_TO_B,} Rail_Dir;																																			//�ڱ�����
	float Chassis_Speed;
} Sentry_info;

/***************************************    ��  �ڱ�������Ϣ ��   ***********************************/
/****************************************   ��  ����ϵͳ��Ϣ ��  ************************************/
typedef struct
{
	int Remain_HP;				  //ʣ��Ѫ��
	int Last_HP;				    //�ϴ�HP;
	int Shoot_Freq;				  //��Ƶ
	float Shoot_Speed;			//����
	float Volt;
	float Current;
	float Power;        		//����ʵʱ����
	float Power_Buffer;			//������
	float Shooterheat;			//ǹ������
	u8    Process;				  //��������
	u8	  Armor_ID;				  //װ�װ�
}Judge_rece;

/***************************************    ��  ����ϵͳ��Ϣ ��   ***********************************/
/****************************************   ��  �ڱ�ͨ����Ϣ ��  ************************************/
typedef struct
{
    int16_t CReceiveSpeedData;				//���̽����ٶ�����
    int16_t	CRecriveMotionMode;				//���̽����ƶ�ģʽ
	  /*
    receive data :3  chassis rc        //ң����
    						 1/4 chsssis patrol    //Ѳ��
                  2  chassis relax     //�ͷ�
	                5  chassis init      //��ʼ��
    */
	  int16_t Pitchlmul;
    int16_t	GSendMotionMode;				  //��̨���ݵ����˶�ģʽ
	  /*
	  send data :1 
	             2 ��ȫ
	             3 ң����
	             4 
	             5 ��λ
	  */
    int16_t GSendSpeedData; 				//��̨���͵����ٶ�

} Communicate_info;
/***************************************    ��  �ڱ�ͨ����Ϣ ��   ***********************************/
/****************************************   ��  ֡�ʼ����Ϣ ��  ************************************/
typedef struct
{
	u8  DBUSFrameRate;
	u8  PitchFrameRate;
	u8  YawFrameRate;
	u8  PokeFrameRate;
	u8  F_ChassisFrameRate;
	u8  B_ChassisFrameRate;
	u8  GIMBALBOARD_FrameCounter;	
	u8  CHASSISBOARD_FrameCounter;
	u8  JudgeFrameRate;
	int MiniPCFrameCounter;
	int MPU9250FrameRate;
}FRAME_RATE;

typedef struct
{
	u8 Enemy_Flag[100];
	u8 Enemy_Count;
	u8 Motion_Count;
	float MiniPC_Msg[6];
	float Angle_error[30];
	float Correction_Yaw;      //Yaw����
	float Correction_Pitch;    //Pitch����
	float Motion[100];
	float EnemyError;
} imagepro_info;
/****************************************   ��  ��С���˷�ϵ�� ��  ************************************/
typedef struct
{
	u8 A,B,C; 
} Least_Square_INFO;

/****************************************   ��  ��С���˷�ϵ�� ��   ***********************************/


/******************************DBUS��ؽṹ��***************************************/
typedef struct
{
    int16_t ch0;
    int16_t ch1;
    int16_t ch2;
    int16_t ch3;
    uint8_t switch_left;            /*   1    */
    uint8_t switch_left_last;       /*   3    */
    uint8_t switch_right;		      	/*   2    */
    uint8_t switch_right_last;


    struct
    {
        int16_t x;
        int16_t y;
        int16_t z;
        uint8_t press_left;
        uint8_t press_right;

        uint8_t press_left_hop;//����������
        uint8_t press_right_hop;//����Ҽ�����
    } mouse;

    struct
    {
        uint16_t key;
        uint16_t key_hop; //�����ֵ
    } keyBoard;
} DBUSDecoding_Type;

/*************************************************************************/






/*
*********************************************************************************************************
*                                            ״̬�任
*********************************************************************************************************
*/
//Friction wheel control mode.
typedef enum
{
    FricStatus_Stop = 0,
    FricStatus_Working = 1,
} FricStatus_Enum;

//the base control mode
typedef enum
{
    Security_Control_Mode,          //����ģʽ��ȫ��ֹͣ
    Auto_Control_Mode,			      	//ȫ�Զ�ģʽ
    RC_Contral_Mode,                //ң��������
} ControlMode_Enum;

//the Auto mode subschema
typedef enum
{
    Auto_Normal_Mode,								//����ģʽ
    Arm_Revenge_Mode,								//װ��

} HeatSubschema_Enum;

//the control mode of chaiss motor
typedef enum
{
	  MONTION_INIT,									//���̸�λ
    MONTION_RELAX,							  //�����ͷ�
    MONTION_RC,									  //����ң��
    MONTION_PATROL,							  //fightģʽ����
    Following_Mode,
} MotionMode_Enum;

typedef enum
{
	  Normal_Mode,						//���
	  Escape_Mode,						//���  
	  Ordinary_Heat_Mode,		  //�������
	  Amazing_Mode,						//�ܣ�
} ChassisMode_Enum;

//the set mode of gimbal
//typedef enum
//{
//    Low_Circle_Mode,								//������תģʽ
//    SemiCircle_Mode,								//��Բ����
//    BaseDefend_Mode,								//���ػ���
//    StaticShoot_Mode,								//��ֹ���
//} CloudMode_Enum;

//the control mode of gimbal
typedef enum
{
    GIMBAL_RELAX        = 0,				//��̨�����ͷ�
    GIMBAL_INIT         = 1,				//��̨��ʼ��
    GIMBAL_FLOOW        = 2,				//��̨����
    GIMBAL_HEAT       	= 3,				//��̨���
    GIMABL_RC    		    = 4,				//ң��������
} GimbalControl_Mode_Enum;

//λ��
typedef enum
{
	  Front_edge,
	  Front_road,
	  Front_curve,
	  Mid_road,
	  Behind_curve,
	  Behind_road,
	  Behind_edge,
} POSITION;

#endif
