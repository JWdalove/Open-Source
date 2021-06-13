#ifndef __UART4_H__
#define __UART4_H__

#include <stm32f4xx.h>
#include <stdio.h>
#include "judgement_info.h"
#include "cmsis_os.h"
#include "comm_task.h"
#include "task.h"
#include "Start_task.h"
#include "protocol.h"

//#define UART_RX_DMA_SIZE 150
#define JudgeTxBufferLength 3  //����ϵͳ�������ݻ���
#define JudgeFrameLength_1      44
#define JudgeFrameLength_2      11
#define JudgeFrameLength_3      24

#define UART_RX_DMA_SIZE       1024
/* dma double buffer ˫����*/
extern uint8_t judge_dma_rxbuff[2][UART_RX_DMA_SIZE];
extern uint8_t pc_dma_rxbuff[2][UART_RX_DMA_SIZE];

//��ʽת��������
typedef union
{
    uint8_t U[4];
    float F;
    int I;
} FormatTrans;

typedef __packed struct
{
    uint8_t SOF;          //����֡��ʼ�ֽ� 0xA5
    uint16_t DataLength;  //֡��Date����
    uint8_t Seq;          //�����
    uint8_t CRC8;         //֡ͷCRCУ��
} tFrameHeader;         // ����֡ͷ
typedef enum
{
    GameInfo = 0x0001,     //����������Ϣ 0x0001  50Hz
    RealBloodChangedData,  //ʵʱѪ���仯���� 0x0002
    RealShootData,         //ʵʱ������� 0x0003
    SelfDefinedData =0x301, //ѧ���ϴ��Զ�������  200Hz
    Wrong = 0x1301 //������
} tCmdID;                // ������ID

typedef __packed struct
{
    uint8_t flag;
    float x;				//��λΪm��С�������λΪ��Чֵ
    float y;
    float z;
    float compass; //UWB�Ƕ���Ϣ
} tLocData; //LOC״̬

typedef __packed struct
{
    uint32_t remainTime;      //����ʣ��ʱ�䣨�ӵ���ʱ�����ӿ�ʼ���㣬��λ s��
    uint16_t remainLifeValue; //������ʣ��Ѫ��
    float realChassisOutV;    //ʵʱ���������ѹ����λ V��
    float realChassisOutA;    //ʵʱ���������������λ A��
    tLocData locData;         //LOC ״̬���� tLocData �ṹ�嶨��
    float remainPower;        //ʣ����������ֵ 60J��
    float heatlimit;          //ǹ����������
} tGameInfo;                //����������Ϣ


typedef __packed struct
{
		uint16_t data_cmd_id;  //1. �������ݽ�����Ϣ
		uint16_t send_ID;
		uint16_t receiver_ID;	
    uint8_t data1;           //1.�ͻ����Զ������ݣ� cmd_id:0x0301������ ID:0xD180�� ����Ƶ�ʣ����� 10Hz
    uint8_t data2;
    uint8_t data3;
} tSelfDefineInfo;


typedef __packed struct
{
    tFrameHeader    FrameHeader;   //֡ͷ
    tCmdID          CmdID;         // ������ID
    __packed union
    {
        tGameInfo       GameInfo;    //������Ϣ (0x0001)
//        tRealBloodChangedData   RealBloodChangedData; //Ѫ���仯��Ϣ(0x0002)
//        tRealShootData   RealShootData; //�ܴ����Ϣ (0x0003)
        tSelfDefineInfo SelfDefineInfo; //ѧ���Զ�����Ŀ (0x0005)
    } Data;                           // ֡������
    uint16_t        CRC16;            //֡βCRC
} tFrame;                           // ���� ��ʽ

void UART4_Judge_InitConfig(void);
void Judge_data_pass(void);
void Judge_data_send(void);
void Send_FrameData(tCmdID cmdid, uint8_t * pchMessage,uint8_t dwLength);

extern u8 JudgeFrameCount;
#endif
