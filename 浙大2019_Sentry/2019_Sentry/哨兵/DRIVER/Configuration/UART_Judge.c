
#include "uart_judge.h"

extern Judge_rece	Judge_DATA;
/* dma double buffer ˫����*/
uint8_t judge_dma_rxbuff[2][UART_RX_DMA_SIZE];

/*
  @UART4_TX PC10
  @UART4_RX PC11
*/
extern  int8_t  Rx_data1;
//uint8_t JudgeDataBuffer[150];
void UART4_Judge_InitConfig(void)
{
    GPIO_InitTypeDef  gpio;
    USART_InitTypeDef uart4;
    NVIC_InitTypeDef  nvic;
    DMA_InitTypeDef     DMA_InitStructure;
    //ʱ�ӳ�ʼ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

    GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4);
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4);
    //GPIO��ʼ��
    gpio.GPIO_Pin = GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC,&gpio);

    gpio.GPIO_Pin = GPIO_Pin_11;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_OD;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOC,&gpio);

    //UARST��ʼ��
    uart4.USART_BaudRate = 115200;
    uart4.USART_WordLength = USART_WordLength_8b;
    uart4.USART_StopBits = USART_StopBits_1;
    uart4.USART_Parity = USART_Parity_No;
    uart4.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
    uart4.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(UART4,&uart4);

    USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);//ʹ�ܴ��ڿ����ж�	IDLE
    USART_Cmd(UART4,ENABLE);

    USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE);
    USART_DMACmd(UART4, USART_DMAReq_Tx, ENABLE);
    //DMA  RX
    DMA_InitStructure.DMA_Channel           =   DMA_Channel_4;
    DMA_InitStructure.DMA_PeripheralBaseAddr=   (uint32_t)(&UART4->DR);
    DMA_InitStructure.DMA_Memory0BaseAddr   =   (uint32_t)judge_dma_rxbuff[0];
    DMA_InitStructure.DMA_DIR               =   DMA_DIR_PeripheralToMemory; //RX
    DMA_InitStructure.DMA_BufferSize        =   150; //1024
    DMA_InitStructure.DMA_PeripheralInc     =   DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc         =   DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_MemoryDataSize    =   DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_PeripheralDataSize=   DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_Mode              =   DMA_Mode_Circular;   //double buffer
    DMA_InitStructure.DMA_Priority          =   DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode          =   DMA_FIFOMode_Disable; //
    DMA_InitStructure.DMA_FIFOThreshold     =   DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst       =   DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst   =   DMA_PeripheralBurst_Single;
    /*DMA double buffer mode*///˫������ģʽ  2018/2/24
    DMA_DoubleBufferModeConfig(DMA1_Stream2,(uint32_t)judge_dma_rxbuff[0], DMA_Memory_0);	//DMA_Memory_0���ȱ�����
    DMA_DoubleBufferModeCmd(DMA1_Stream2, ENABLE);
    DMA_Init(DMA1_Stream2, &DMA_InitStructure);
    DMA_Cmd(DMA1_Stream2, ENABLE);
    //IRQ
    nvic.NVIC_IRQChannel = UART4_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 10;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    //����DMA���ж� 2018/2/24
    nvic.NVIC_IRQChannel = DMA1_Stream2_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 11;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
    /*DMA �жϿ�*/
    DMA_ITConfig(DMA1_Stream2,DMA_IT_TC,ENABLE);	
}


/***************************************
  * @brief  :����ϵͳ���ݸ���
  * @param  :
****************************************/
void Judge_data_pass()
{
	Judge_DATA.Last_HP		=	Judge_DATA.Remain_HP;
	
	Judge_DATA.Remain_HP 	=	judge_rece_mesg.game_information.remain_HP;
    Judge_DATA.Shoot_Freq  	=	judge_rece_mesg.real_shoot_data.bullet_freq;
    Judge_DATA.Shoot_Speed 	=	judge_rece_mesg.real_shoot_data.bullet_speed;
    Judge_DATA.Volt        	=	judge_rece_mesg.power_heat.chassis_volt;
    Judge_DATA.Current     	=	judge_rece_mesg.power_heat.chassis_current;
    Judge_DATA.Power       	=	judge_rece_mesg.power_heat.chassis_power;
    Judge_DATA.Power_Buffer	=	judge_rece_mesg.power_heat.chassis_power_buffer;
    Judge_DATA.Shooterheat 	=	judge_rece_mesg.power_heat.shooter_heat0;
//	Judge_DATA.Process		=	judge_rece_mesg.game_state.game_progress;      //0-δ��ʼ����	1-׼���׶�	2-�Լ�׶�	3-5�뵹��ʱ	4-��ս��	5-��������
	Judge_DATA.Armor_ID		=	judge_rece_mesg.blood_changed_data.armor_id;
}


void USART4_SendChar(unsigned char b)
{
    USART_SendData(UART4,b);
	while(USART_GetFlagStatus(UART4,USART_FLAG_TC)!=SET);
}
void Uart4_SendBuff ( uint8_t *buff, uint32_t len)
{
    while(len--)
    {
        USART4_SendChar(*buff);
        buff++;
    }
}
/***************************************
  * @brief  :�������ݷ���
  * @param  :
****************************************/
void Send_FrameData(tCmdID cmdid, uint8_t * pchMessage,uint8_t dwLength)
{
    uint8_t i;
    uint8_t *addr;
    static tFrame Frame = {.FrameHeader.SOF = 0xA5};//SOF֡��ʼ�ֽ� 0XA5
    if (dwLength != sizeof(tSelfDefineInfo))    //У���������ݳ���

        return ;

    addr = (uint8_t *)&Frame.Data.GameInfo; //�׵�ַΪDate�еĵ�һ��

    Frame.CmdID = cmdid; //������
    Frame.FrameHeader.DataLength = dwLength;//֡��date���� 12
    Frame.FrameHeader.Seq++; //����� ���ڼ��η��͵İ�
    for(i = 0; i < dwLength; i++)
    {
        *addr++ = *pchMessage++;//���ݿ���
    }
    append_crc8_check_sum((unsigned char *)&Frame.FrameHeader,sizeof(Frame.FrameHeader));//֡ͷCRCУ��
    i = sizeof(Frame.FrameHeader) + sizeof(Frame.CmdID)  + sizeof(Frame.CRC16) + dwLength;//֡����
    append_crc16_check_sum((unsigned char *)&Frame,i); //֡βCRCУ��
    Uart4_SendBuff((uint8_t *)&Frame,i);               //֡����
}

/**
  * @brief  ����ϵͳ�����ж�(UART4)
  * @param  void
  * @retval void
  * @note
�ڴ��ڿ����ж�ʱ����ȡ DMA �����е��������ݲ����棬Ȼ������ DMA���� DMA ���¿�ʼ�������ݣ�
�� DMA �������ж�ʱ����ȡ DMA �����е��������ݲ����棬Ȼ���� DMA ���¿�ʼ�������ݡ����������ʹ�� DMA ˫���壬�����Ч�ʣ�Ҳ����ʹ�ó�������λ DMA ���ջ��壻
ʹ�ÿ����жϴ������ݵ�ʱ����Ҫע�⣬Ҫ��¼��ʱ DMA �����е�����λ����Ȼ����Ϊ����Ķ�дָ�룬��һ�� DMA ���жϻ��߿����ж�ʱ�����λ�ÿ�ʼ��ȡ��
  */

uint8_t UART4temp; //������մ��ڱ�־λ����ʱ����
void UART4_IRQHandler(void)
{

    UART4temp = UART4->DR;
    UART4temp = UART4->SR;
    osSignalSet(judge_unpack_task_t, JUDGE_UART_IDLE_SIGNAL);  //��������֪ͨ
}


//DMA���ж�
void DMA1_Stream2_IRQHandler(void)
{

    if(DMA_GetITStatus(DMA1_Stream2,DMA_IT_TCIF2) != RESET) //�ж��ж����������й�
    {
        if(DMA_GetCurrentMemoryTarget(DMA1_Stream2) == 1)		//��ȡ��ǰDMA��memory
        {
            DMA_MemoryTargetConfig(DMA1_Stream2,(uint32_t)judge_dma_rxbuff[0],DMA_Memory_0);
        }
        else
        {
            DMA_MemoryTargetConfig(DMA1_Stream2,(uint32_t)judge_dma_rxbuff[1],DMA_Memory_1);
        }
        osSignalSet(judge_unpack_task_t, JUDGE_DMA_FULL_SIGNAL);
        DMA_ClearITPendingBit(DMA1_Stream2,DMA_IT_TCIF2);
    }
    else if(DMA_GetITStatus(DMA1_Stream2,DMA_IT_DMEIF2) != RESET)
    {

    }
}

