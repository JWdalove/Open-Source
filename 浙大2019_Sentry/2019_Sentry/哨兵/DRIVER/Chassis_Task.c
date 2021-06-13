#include "Chassis_Task.h"
ramp_t   fb_ramp = RAMP_GEN_DAFAULT;
uint8_t  JumpToPatrolFlag;

extern GimbalControl_Mode_Enum 	Gimbal_Control;
extern MotionMode_Enum	MontionMode;
extern Sentry_info 		SentryInfo;
extern Chassis_info  	Chassis;
extern Poked_info		Poked;
extern POSITION			Position;
extern Judge_rece		Judge_DATA;
extern Communicate_info Comm;
extern ChassisMode_Enum   Chassis_Heat_Mode;
extern u8 front_hurt,edge_hurt;
int Chassiscount=1,ChassisCounter=1;				//����
int hurt_Count=0;
int Edge_position=0;
u8 JumpToNormal = 1,								//״̬��ת��־λ ��ʼ��һΪ�����������һ��ģʽ
   JumpToOHeat = 1,
   JumpToEscape = 1,
   JumpToAmazing = 1;	
u8 turn_one = 1,
   turn_two = 1,
   turn_three = 1,
   mil_flag = 1;
char string;

// ********** �� �������� �� **********//
void Chassis_Task(void *Parameters)
{
    portTickType CurrentControlTick = 0;
    for(;;)
    {
	  CAN2->MSR=0;
      CurrentControlTick = xTaskGetTickCount();   //��ȡ�Կ���������ǰϵͳʱ��
			
	// ********** �� ����ϵͳ���� �� **********//
		Chassiscount++;
    // ********** �� ����ϵͳ���� �� **********//
			
		Judge_data_pass();
		
	// ********** �� ����״̬���� �� **********//
        Control_Status_Update(CurrentControlTick);
	// ********** �� ����״̬���� �� **********//
			
    // ********** �� ʣ���������� �� **********//
		Power_Calculate();
		ShootPower_Calculate();
    // ********** �� ʣ���������� �� **********//
			
	// ********** �� ���̿���ģʽ �� **********//	
		if(Chassis.F.Esc.Zero_Flag<=-75 && Chassis.B.Esc.Zero_Flag>= 75)Edge_position= 1;
		else if(Chassis.F.Esc.Zero_Flag>= 95 && Chassis.B.Esc.Zero_Flag<=-95)Edge_position=-1;
        else Edge_position=0;		
		
        switch(MontionMode)
        {
			//���̸�λ
		    case MONTION_INIT:{
			       __set_FAULTMASK(1);
			       NVIC_SystemReset();}
			//�����ͷ�
			case MONTION_RELAX:{
                   Chassis.F.Out = 0;
                   Chassis.B.Out = 0;
            break;}
			//RCң��
            case MONTION_RC:{
                   Chassis_RC();												    	//ң����ģʽ�µ����ٶ��趨
                   Chassis_MoveCalculate();										//�ڱ��ٶȽ���
                   Chassis_Contral();												  //�ڱ��ٶȱջ�����
            break;}
			//����Ѳ��
            case MONTION_PATROL:{
			       Chassis_Heat_Mode_Update();
                   Chassis_Patrolling();   										//�Զ�Ѳ��ģʽ�µ����ٶ��趨
                   Chassis_MoveCalculate();										//�ڱ��ٶȽ���
		           Chassis_Contral();										  		//�ڱ��ٶȱջ�����
            break;}
            default:
                 MontionMode = MONTION_RELAX;                   					//���ź�  ��Ϊ �����ͷ�//
            break;
        }
	// ********** �� ���̿���ģʽ �� **********//
				
	// ********** �� ���ͨ�� �� **********//
		ChassisMotorDriveCan2(Chassis.F.Out,Chassis.B.Out,0,0);
		Chassis_Board_Communicate(Judge_DATA.Shooterheat,Edge_position,(int)(Chassis.F.Esc.Angle_Speed/100-Chassis.B.Esc.Angle_Speed/100),Judge_DATA.Shoot_Speed);
	// ********** �� ���ͨ�� �� **********//    
				
		vTaskDelayUntil(&CurrentControlTick, 2/ portTICK_RATE_MS);  		//2ms����  ������ʱ  �ͷ�CPUռ��Ȩ
    }
}
// ********** �� �������� �� **********//

/***************************************
  * @brief  :����ģʽ
  * @param  :RC,Patrolling
****************************************/
void Chassis_RC(void)
{
	JumpToNormal = 1;
	JumpToOHeat = 1;
	JumpToEscape = 1;
	Chassis_SpeedSet(15.0f*Comm.CReceiveSpeedData);
}

void Chassis_Patrolling(void)
{
	//6500�պò���������
    /*********************  ��  �ڱ��˶��������� �� ***********************/
	switch(Chassis_Heat_Mode)
	{
		case Normal_Mode:                //����Ѳ��
		{
			if(Chassis.initial_position==0)
			{
				Chassis.F.Esc.Zero_Flag=0;
				Chassis.B.Esc.Zero_Flag=0;
				Chassis.initial_position++;
			}
			if(Chassis.initial_position==1)
			{
			    SentryInfo.Chassis_Speed = 6000.0f;
			    SentryInfo.dir=1;
			    ramp_init(&fb_ramp,400);
			}
			if(Chassis.F.Esc.Zero_Flag<=-80 && Chassis.B.Esc.Zero_Flag>= 80)
		    {
				ramp_init(&fb_ramp,400);
				Chassis.initial_position++;
                SentryInfo.dir = -1;
		    }
		    if(Chassis.F.Esc.Zero_Flag>= 100 && Chassis.B.Esc.Zero_Flag<=-100)
		    {
				ramp_init(&fb_ramp,400);
				Chassis.initial_position++;
				SentryInfo.dir =  1;   
		    }	
			break;
		}
		
		case Escape_Mode:                //��ܵﱤ
		{

			break;
		}
		
		case Ordinary_Heat_Mode:         //������˻�or������
		{

			break;
		}		
		
		case Amazing_Mode:
		{

			break;
		}
		
	}
    /*********************  ��  �ڱ��˶���������  ��  ***********************/
	
    /*********************  ��  �ڱ��ٶ����� �� ***********************/
	Chassis_SpeedSet(SentryInfo.dir * SentryInfo.Chassis_Speed);                //�ڱ������� dir=-1 �ڱ����ҷ���  dir=1
    /*********************  ��  �ڱ��ٶ�����  ��  ***********************/
}
