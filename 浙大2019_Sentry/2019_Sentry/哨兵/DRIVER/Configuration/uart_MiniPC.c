#include "uart_MiniPC.h"

extern Sentry_info		SentryInfo;
extern Cloud_info		Yaw,Pitch;
extern imagepro_info 	MiniPC;
extern Least_Square_INFO	Least_Square_Info;
extern float speed_Euler_Pitch,speed_Euler_Yaw;
extern float Chassis_Offset;
extern u8 Offset_flag_f,Offset_flag_b;
u8 i = 0;
int USARTtemp2;
int MiniPCFrameCounter=0;			//֡�ʼ���
unsigned char USART_RX_TEST[20]= {0};
char USART2_TX_LEN=6;
#define abs_f(x) ((x)>0? (x):(-(x)))

void USART_MiniPC_InitConfig(void)
{
    USART_InitTypeDef usart2;
    GPIO_InitTypeDef  gpio;
    NVIC_InitTypeDef  nvic;
    DMA_InitTypeDef   DMA_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);    //  DMA1 Stream5
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_5| GPIO_Pin_6;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD,&gpio);

    GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2);

    usart2.USART_BaudRate = 115200;
    usart2.USART_WordLength = USART_WordLength_8b;
    usart2.USART_StopBits = USART_StopBits_1;
    usart2.USART_Parity = USART_Parity_No;
    usart2.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
    usart2.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2,&usart2);
    USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);
    USART_Cmd(USART2,ENABLE);

    USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);        //ʹ�ܴ���2DMA����

    DMA_DeInit(DMA1_Stream5);
    DMA_InitStructure.DMA_Channel            =   DMA_Channel_4;
    DMA_InitStructure.DMA_PeripheralBaseAddr =   (uint32_t)(&USART2->DR);
    DMA_InitStructure.DMA_Memory0BaseAddr    =   (uint32_t)(USART_RX_TEST);
    DMA_InitStructure.DMA_DIR                =   DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize         =   20; //1024
    DMA_InitStructure.DMA_PeripheralInc      =   DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          =   DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_MemoryDataSize     =   DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_PeripheralDataSize =   DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_Mode               =   DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           =   DMA_Priority_Medium;
    DMA_InitStructure.DMA_FIFOMode           =   DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold      =   DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst        =   DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst    =   DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream5, &DMA_InitStructure);

    nvic.NVIC_IRQChannel = USART2_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 2;
    nvic.NVIC_IRQChannelSubPriority = 2;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
}

/***************************************
  * @brief  :���ֽڷ���
  * @param  :
  * @retval :
****************************************/
void USART2_Senddata(char D)
{
	USART_SendData(USART2,D);
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
}

/***************************************
  * @brief  :���ڷ���->PC
  * @param  :
  * @retval :
****************************************/
void PC_Communicate(float RealData,u8 Time)
{
	int16_t Data;
	char adad[9];
	if(RealData<0)
	{
		Data = RealData * (-10);
		adad[4] = '-';
	}
	else
	{
		Data=RealData * 10;
		adad[4] = '+';
	}
	
	adad[0] = 0xAF;
	adad[1] = 'R';		  //���飨R���������B��
	adad[2] = 'L';		  //ʶ����ɫ����ɫ��H������ɫ��L��
	adad[3] = 'R';		  //ʶ����룬RΪ�Զ�
	adad[5] = Data>>8;	//Yaw��8
	adad[6] = Data&0xFF;//Yaw��8
	adad[7] = Time;		  //Time
	adad[8] = 0xFA;	  	//֡β   
	for(u8 i=0;i<9;i++)
	{
		USART2_Senddata(adad[i]);
	}
}

/***************************************
  * @brief  :
  * @param  :
  * @retval :
****************************************/
void USART2_IRQHandler(void)
{

    USARTtemp2 = USART2->DR;
    USARTtemp2 = USART2->SR;
    DMA_Cmd(DMA1_Stream5, DISABLE);
	MiniPCFrameCounter++;
	if(USART_RX_TEST[7]>2)USART_RX_TEST[7]=0;
	MiniPC.MiniPC_Msg[4]=USART_RX_TEST[7];//��־λ��1Ϊʶ�𵽣�
	
	if(MiniPC.MiniPC_Msg[4]==1)
	{
		MiniPC.MiniPC_Msg[0]=USART_RX_TEST[0];
		MiniPC.MiniPC_Msg[1]=((float)USART_RX_TEST[1] + (float)USART_RX_TEST[2]/100) - 90.0f;       //Yaw�Ƕ�ƫ��  //88.5f
		MiniPC.MiniPC_Msg[2]=(((float)USART_RX_TEST[3] + (float)USART_RX_TEST[4]/100) - 90.0f)*0.6f;               //Pitch�Ƕ�ƫ��
		MiniPC.MiniPC_Msg[3]=(USART_RX_TEST[5]|USART_RX_TEST[6]<<8)/100.0f;//����
		MiniPC.MiniPC_Msg[5]=USART_RX_TEST[9];       //�˶���1������ֹ��0��
	}
	else
	{
		MiniPC.MiniPC_Msg[0]=MiniPC.MiniPC_Msg[0];
		MiniPC.MiniPC_Msg[1]=0.0f;
		MiniPC.MiniPC_Msg[2]=0.0f;
		MiniPC.MiniPC_Msg[3]=MiniPC.MiniPC_Msg[3];
		MiniPC.MiniPC_Msg[5]=USART_RX_TEST[9];//�˶���1������ֹ��0��
	}
		
    if(MiniPC.MiniPC_Msg[2]>MiniPCMax || MiniPC.MiniPC_Msg[2]<-MiniPCMax)
	{
		if(MiniPC.MiniPC_Msg[2]>=0)MiniPC.MiniPC_Msg[2]=MiniPCMax+1.0f;
		else if(MiniPC.MiniPC_Msg[2]<0)MiniPC.MiniPC_Msg[2]=-MiniPCMax-1.0f;
	}
    MiniPC.Enemy_Count = 0;

    for(i = 99; i > 0; i--)
    {
      MiniPC.Enemy_Flag[i] = MiniPC.Enemy_Flag[i-1];
      MiniPC.Enemy_Count += MiniPC.Enemy_Flag[i];
    }
 
    MiniPC.Enemy_Flag[0] = MiniPC.MiniPC_Msg[4];

    if( (u8)MiniPC.MiniPC_Msg[0] == 0xAA && MiniPC.MiniPC_Msg[4]==1)  //���ݴ���
    {
	  /*******************�˶�Ԥ��*********************/
		MiniPC.Motion_Count=0;
		for(i = 99; i > 0; i--)
		{
			MiniPC.Motion[i] = MiniPC.Motion[i-1];
			MiniPC.Motion_Count += MiniPC.Motion[i];
		}
		for(i = 29; i > 0; i--)
		{
			MiniPC.Angle_error[i] = MiniPC.Angle_error[i-1];
		}
		for(i = 49; i > 0; i--)
		{
			Yaw.EnemyDataBuffer[i] = Yaw.EnemyDataBuffer[i-1];
			Pitch.EnemyDataBuffer[i] = Pitch.EnemyDataBuffer[i-1];
		}
		MiniPC.Angle_error[0] = MiniPC.MiniPC_Msg[1]-Yaw.Plus;
		MiniPC.Motion[0] = MiniPC.MiniPC_Msg[5];
		MiniPC.EnemyError = MiniPC.EnemyError/9.0f;
		Yaw.EnemyDataBuffer[0] = Yaw.Imu_Euler-MiniPC.MiniPC_Msg[1]-speed_Euler_Yaw*MiniPC.Correction_Yaw*0.1f;        //Yaw��е�Ƕ�
		Pitch.EnemyDataBuffer[0] = Pitch.Esc.Machine_Angle + MiniPC.MiniPC_Msg[2]*8191.0f/360.0f;                      //Pitch��е�Ƕ�
		/***************************************************************/
		SentryInfo.targetspeedvx_last = SentryInfo.targetspeedvx;
		SentryInfo.targetspeedvy_last = SentryInfo.targetspeedvy; 
		SentryInfo.targetspeedvx = (Yaw.EnemyDataBuffer[0] - Yaw.EnemyDataBuffer[2]) * 0.015f;                 //�������̨��Ҫ��YAW���ٶ�  
		SentryInfo.targetspeedvy = (Pitch.EnemyDataBuffer[0] - Pitch.EnemyDataBuffer[2]) * 0.02f;              //�������̨��Ҫ��Pitch�ٶ�
		if(abs_f(SentryInfo.targetspeedvx - SentryInfo.targetspeedvx_last) > 3.0f)                             //�������ֵ����������������ٶ�
			SentryInfo.targetspeedvx = SentryInfo.targetspeedvx * 0.4f + SentryInfo.targetspeedvx_last * 0.6f;   //�������ʽ��������ٶ�
		if(abs_f(SentryInfo.targetspeedvy - SentryInfo.targetspeedvy_last) > 3.0f)                             //���ԭ��ͬ��
			SentryInfo.targetspeedvy = SentryInfo.targetspeedvy * 0.4f + SentryInfo.targetspeedvy_last * 0.6f;   //ͬ��

		SentryInfo.Shoot_Dis = MiniPC.MiniPC_Msg[3];
    }
    DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5 | DMA_FLAG_HTIF5);
    while(DMA_GetCmdStatus(DMA1_Stream5) != DISABLE);
    DMA_SetCurrDataCounter(DMA1_Stream5,20);
    DMA_Cmd(DMA1_Stream5, ENABLE);
}

void DMA1_Stream5_IRQHandler(void)
{
    if(DMA_GetFlagStatus(DMA1_Stream5,DMA_FLAG_TCIF5)!=RESET)//�ȴ�DMA2_Steam5�������
    {
        DMA_Cmd(DMA1_Stream5, DISABLE); //�ر�DMA,��ֹ�������������
        DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5 | DMA_FLAG_HTIF5);
        while(DMA_GetCmdStatus(DMA1_Stream5) != DISABLE);
        DMA_SetCurrDataCounter(DMA1_Stream5,20);
        DMA_Cmd(DMA1_Stream5, ENABLE);
    }
}
