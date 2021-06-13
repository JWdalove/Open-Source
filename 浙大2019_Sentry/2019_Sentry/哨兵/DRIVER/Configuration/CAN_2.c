#include "CAN_2.h"

xQueueHandle    Queue_CANSend;
CanRxMsg 		rx_message2;

extern Chassis_info  	Chassis;
extern Poked_info		Poked;
extern Cloud_info	    Yaw,Pitch;
extern Communicate_info	Comm;		//��ʱ������,state_update

u8 CAN_RX_Cloud[8];
u8 CAN2_RX_Pitch[8];
u8 CAN2_RX_Poked[8];

u8 CAN_RX_F[8];
u8 CAN_RX_B[8];

int GIMBALBOARD_FrameCounter = 0;
extern float Yaw_Imu_Euler;

// ********** �� CAN2��ʼ������ �� **********//
void CAN_Second_InitConfig(void)
{
    CAN_InitTypeDef        CAN_InitStructure;
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;
    GPIO_InitTypeDef       GPIO_InitStructure;
    NVIC_InitTypeDef       NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);

    GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType  =   GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_PuPd   =   GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_100MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_CAN2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_CAN2);
		
    CAN_InitStructure.CAN_ABOM      =   ENABLE; //5.22
    CAN_InitStructure.CAN_AWUM      =   ENABLE; //5.22
    CAN_InitStructure.CAN_BS1       =   CAN_BS1_9tq;
    CAN_InitStructure.CAN_BS2       =   CAN_BS2_4tq;
    CAN_InitStructure.CAN_Mode      =   CAN_Mode_Normal;
    CAN_InitStructure.CAN_NART      =   DISABLE;
    CAN_InitStructure.CAN_Prescaler =   3;
    CAN_InitStructure.CAN_RFLM      =   DISABLE;
    CAN_InitStructure.CAN_SJW       =   CAN_SJW_1tq;
    CAN_InitStructure.CAN_TTCM      =   DISABLE;
    CAN_InitStructure.CAN_TXFP      =   DISABLE;
    CAN_Init(CAN2, &CAN_InitStructure);
    
    CAN_FilterInitStructure.CAN_FilterActivation    =   ENABLE;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment=   CAN_Filter_FIFO0;
    CAN_FilterInitStructure.CAN_FilterIdHigh        =   0x0000;
    CAN_FilterInitStructure.CAN_FilterIdLow         =   0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh    =   0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow     =   0x0000;
    CAN_FilterInitStructure.CAN_FilterMode          =   CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterNumber        =   14;
    CAN_FilterInitStructure.CAN_FilterScale         =   CAN_FilterScale_32bit;
    CAN_FilterInit(&CAN_FilterInitStructure);
		CAN_ITConfig(CAN2, CAN_IT_FMP0, ENABLE);
		
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	  NVIC_InitStructure.NVIC_IRQChannel						=	CAN2_RX0_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelCmd					=	ENABLE;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	=	1;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority			=	0;
	  NVIC_Init(&NVIC_InitStructure);  
}
// ********** �� CAN2��ʼ������ �� **********//

// ********** �� CAN2��̨�˶����� �� **********//
void CloudMotorDriveCan2(int16_t current_205,int16_t current_206,int16_t current_207,int16_t current_208)
{
    static CanSend_Type   SendData;
	
    SendData.CANx = 2;
    SendData.TxMessage.StdId = 0x1FF;
    SendData.TxMessage.IDE = CAN_Id_Standard;
    SendData.TxMessage.RTR = CAN_RTR_Data;
    SendData.TxMessage.DLC = 0x08;

    SendData.TxMessage.Data[0] = (unsigned char)(current_205 >> 8);  
    SendData.TxMessage.Data[1] = (unsigned char)current_205;
    SendData.TxMessage.Data[2] = (unsigned char)(current_206 >> 8);  //Pitch��
    SendData.TxMessage.Data[3] = (unsigned char)current_206;
    SendData.TxMessage.Data[4] = (unsigned char)(current_207 >> 8);	 //Poked
    SendData.TxMessage.Data[5] = (unsigned char)current_207;
    SendData.TxMessage.Data[6] = (unsigned char)(current_208 >> 8);
    SendData.TxMessage.Data[7] = (unsigned char)current_208;
	
    xQueueSend(Queue_CANSend, &SendData, 10);
}
// ********** �� CAN2��̨�˶����� �� **********//

// ********** �� CAN2�����˶����� �� **********//
void ChassisMotorDriveCan2(int16_t current_201,int16_t current_202,int16_t current_203,int16_t current_204) //���16λ���ݷ���
{
    static CanSend_Type SendData;
	
    SendData.CANx = 2;
    SendData.TxMessage.StdId = 0x200;
    SendData.TxMessage.IDE = CAN_Id_Standard;
    SendData.TxMessage.RTR = CAN_RTR_Data;
    SendData.TxMessage.DLC = 0x08;

    SendData.TxMessage.Data[0] = (unsigned char)(current_201 >> 8);      //Chass.F
    SendData.TxMessage.Data[1] = (unsigned char)current_201;
    SendData.TxMessage.Data[2] = (unsigned char)(current_202 >> 8);      //Chass.B
    SendData.TxMessage.Data[3] = (unsigned char)current_202;
    SendData.TxMessage.Data[4] = (unsigned char)(current_203 >> 8);
    SendData.TxMessage.Data[5] = (unsigned char)current_203;
    SendData.TxMessage.Data[6] = (unsigned char)(current_204 >> 8);
    SendData.TxMessage.Data[7] = (unsigned char)current_204;
	
    xQueueSend(Queue_CANSend, &SendData, 10);
}
// ********** �� CAN2�����˶����� �� **********//

// ********** �� ���̷�����Ϣ �� **********//
void Chassis_Board_Communicate(int16_t data1,int16_t data2,int16_t data3,int16_t data4)
{
    static CanSend_Type   SendData;
	
    SendData.CANx = 2;
    SendData.TxMessage.StdId = 0x166;
    SendData.TxMessage.IDE = CAN_Id_Standard;
    SendData.TxMessage.RTR = CAN_RTR_Data;
    SendData.TxMessage.DLC = 0x08;

    SendData.TxMessage.Data[0] = (unsigned char)(data1 >> 8);      //Judge_DATA.Shooterheat
    SendData.TxMessage.Data[1] = (unsigned char)data1;
    SendData.TxMessage.Data[2] = (unsigned char)(data2 >> 8);      //Judge_DATA.Process
    SendData.TxMessage.Data[3] = (unsigned char)data2;
    SendData.TxMessage.Data[4] = (unsigned char)(data3 >> 8);      //(int)(Chassis.F.Esc.Angle_Speed/100-Chassis.B.Esc.Angle_Speed/100)
    SendData.TxMessage.Data[5] = (unsigned char)data3;
    SendData.TxMessage.Data[6] = (unsigned char)(data4 >> 8);      //Judge_DATA.Shoot_Speed
    SendData.TxMessage.Data[7] = (unsigned char)data4;
	
    xQueueSend(Queue_CANSend, &SendData, 10);
}
// ********** �� ���̷�����Ϣ �� **********//

// ********** �� CAN2�����ж� �� **********//
void CAN2_RX0_IRQHandler(void)
{
    if (CAN_GetITStatus(CAN2,CAN_IT_FMP0)!= RESET)
    {
        CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
        CAN_Receive(CAN2, 0, &rx_message2);
        switch (rx_message2.StdId)
        {
					
        case GIMBALBOARD:      //������̨���ص���Ϣ(0x188)
        {
            GIMBALBOARD_FrameCounter++;
					
            CAN_RX_Cloud[0]=rx_message2.Data[0];
            CAN_RX_Cloud[1]=rx_message2.Data[1];
            CAN_RX_Cloud[2]=rx_message2.Data[2];
            CAN_RX_Cloud[3]=rx_message2.Data[3];
            CAN_RX_Cloud[4]=rx_message2.Data[4];
            CAN_RX_Cloud[5]=rx_message2.Data[5];
            CAN_RX_Cloud[6]=rx_message2.Data[6];
            CAN_RX_Cloud[7]=rx_message2.Data[7];

            Comm.CRecriveMotionMode   =  CAN_RX_Cloud[0]<<8 | CAN_RX_Cloud[1];  //������̨�������߲����˶�ģʽ
					  /*
            receive data : 5  chassis init      //��ʼ��
					                 2  chassis relax     //�ͷ�
					                 3  chassis rc        //ң����
    						          1/4 chsssis patrol    //Ѳ��
            */
            Comm.CReceiveSpeedData	  =  CAN_RX_Cloud[2]<<8 | CAN_RX_Cloud[3];  //������̨�������߲���ң��ģʽ���˶��ٶ�
			      Comm.Pitchlmul            =  CAN_RX_Cloud[4]<<8 | CAN_RX_Cloud[5];  //Pitch�Ƕ�
            break;
        }
				
        case CHASSISF:      //����ǰ��ĵ��(0x201)
        {
            Chassis.F.Esc.FrameCounter++;      //���ϵͳ
					
            CAN_RX_F[0]=rx_message2.Data[0];
            CAN_RX_F[1]=rx_message2.Data[1];
            CAN_RX_F[2]=rx_message2.Data[2];
            CAN_RX_F[3]=rx_message2.Data[3];
            CAN_RX_F[4]=rx_message2.Data[4];
            CAN_RX_F[5]=rx_message2.Data[5];
            CAN_RX_F[6]=rx_message2.Data[6];
            CAN_RX_F[7]=rx_message2.Data[7];

            Chassis.F.Esc.Machine_Angle_Last=Chassis.F.Esc.Machine_Angle;
					
            Chassis.F.Esc.Machine_Angle =  CAN_RX_F[0]<<8 | CAN_RX_F[1];      //��õ���ǰ��ĵ����е�Ƕ�
            Chassis.F.Esc.Angle_Speed   =  CAN_RX_F[2]<<8 | CAN_RX_F[3];      //��õ���ǰ��ĵ���ٶ�ֵ
					
            Chassis.F.Esc.USE_Machine=Chassis.F.Esc.Machine_Angle;	      //����ǰ�����е�Ƕ�ֵ��USE
				
            if(Chassis.F.Esc.Machine_Angle-Chassis.F.Esc.Machine_Angle_Last>4096)
                Chassis.F.Esc.Zero_Flag--;      //������
            else if(Chassis.F.Esc.Machine_Angle-Chassis.F.Esc.Machine_Angle_Last<-4096)
                Chassis.F.Esc.Zero_Flag++;      //������
                Chassis.F.Esc.USE_Machine += (8191*Chassis.F.Esc.Zero_Flag);	                 //��USE��δ��һȦ��+����һȦ
            break;
        }
				
        case CHASSISB:      //���̺���ĵ��(0x202)
        {
            Chassis.B.Esc.FrameCounter++;   //���ϵͳ
					
            CAN_RX_B[0]=rx_message2.Data[0];
            CAN_RX_B[1]=rx_message2.Data[1];
            CAN_RX_B[2]=rx_message2.Data[2];
            CAN_RX_B[3]=rx_message2.Data[3];
            CAN_RX_B[4]=rx_message2.Data[4];
            CAN_RX_B[5]=rx_message2.Data[5];
            CAN_RX_B[6]=rx_message2.Data[6];
            CAN_RX_B[7]=rx_message2.Data[7];
					
            Chassis.B.Esc.Machine_Angle_Last=Chassis.B.Esc.Machine_Angle;
					
            Chassis.B.Esc.Machine_Angle =  CAN_RX_B[0]<<8 | CAN_RX_B[1];  //��õ��̺���ĵ����е�Ƕ�
            Chassis.B.Esc.Angle_Speed   =  CAN_RX_B[2]<<8 | CAN_RX_B[3];  //��õ��̺���ĵ���ٶ�ֵ
					
            Chassis.B.Esc.USE_Machine=Chassis.B.Esc.Machine_Angle;      //����ǰֵ��USE
					
            if(Chassis.B.Esc.Machine_Angle-Chassis.B.Esc.Machine_Angle_Last>4096)
                Chassis.B.Esc.Zero_Flag--;      //������
            else if(Chassis.B.Esc.Machine_Angle-Chassis.B.Esc.Machine_Angle_Last<-4096)
                Chassis.B.Esc.Zero_Flag++;      //������
            Chassis.B.Esc.USE_Machine += (8191*Chassis.B.Esc.Zero_Flag);	                 //��USE��δ��һȦ��+����һȦ
            break;
        }

 	    	case PITCHMOTOR:      //Pitch����(0x206)
		    {
            Pitch.Esc.FrameCounter++;		//���ϵͳ
					
            CAN2_RX_Pitch[0]=rx_message2.Data[0];
            CAN2_RX_Pitch[1]=rx_message2.Data[1];
            CAN2_RX_Pitch[2]=rx_message2.Data[2];
            CAN2_RX_Pitch[3]=rx_message2.Data[3];
            CAN2_RX_Pitch[4]=rx_message2.Data[4];
            CAN2_RX_Pitch[5]=rx_message2.Data[5];
            CAN2_RX_Pitch[6]=rx_message2.Data[6];
            CAN2_RX_Pitch[7]=rx_message2.Data[7];

            Pitch.Esc.Machine_Angle =	CAN2_RX_Pitch[0]<<8 | CAN2_RX_Pitch[1];  //���Pitch���е�Ƕ�
            Pitch.Esc.Rotate_Speed	=	CAN2_RX_Pitch[2]<<8 | CAN2_RX_Pitch[3];  //���Pitch����ת��
            Pitch.Esc.Elec_Measure  =	CAN2_RX_Pitch[4]<<8 | CAN2_RX_Pitch[5];  //���Pitch��ʵ��ת�ص���

            Pitch.Esc.USE_Machine = Pitch.Esc.Machine_Angle;
					
			      break;
		    }
				
		    case POKEDMOTOR:      //�������(0x207)
        {
            Poked.Esc.FrameCounter++;		//���ϵͳ
					
            CAN2_RX_Poked[0]=rx_message2.Data[0];
            CAN2_RX_Poked[1]=rx_message2.Data[1];
            CAN2_RX_Poked[2]=rx_message2.Data[2];
            CAN2_RX_Poked[3]=rx_message2.Data[3];
            CAN2_RX_Poked[4]=rx_message2.Data[4];
            CAN2_RX_Poked[5]=rx_message2.Data[5];
            CAN2_RX_Poked[6]=rx_message2.Data[6];
            CAN2_RX_Poked[7]=rx_message2.Data[7];

            Poked.Esc.Machine_Angle_Last=Poked.Esc.Machine_Angle;            
					
            Poked.Esc.Machine_Angle = CAN2_RX_Poked[0]<<8 | CAN2_RX_Poked[1];  //��ò��������е�Ƕ�
            Poked.Esc.Angle_Speed   = CAN2_RX_Poked[2]<<8 | CAN2_RX_Poked[3];  //��ò������ת��

            if(Poked.Esc.Machine_Angle-Poked.Esc.Machine_Angle_Last>4000)       
						{
                Poked.Esc.Zero_Flag-=1;
            }
            else if(Poked.Esc.Machine_Angle-Poked.Esc.Machine_Angle_Last<-4000) 
						{
                Poked.Esc.Zero_Flag+=1;
            }
						
            Poked.Esc.USE_Machine=8191* Poked.Esc.Zero_Flag+Poked.Esc.Machine_Angle;
            break;
        }
				
        }
    }
}
