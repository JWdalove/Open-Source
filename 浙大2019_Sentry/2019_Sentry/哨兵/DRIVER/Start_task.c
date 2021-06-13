#include "Start_task.h"
#include "kalman.h"	
#include "matrix.h"

extern TaskHandle_t STARTTASK_Handler;
TaskHandle_t judge_unpack_task_t;
void Task_SysInitConfig(void *Parameters)
{
	/*��������*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //����ϵͳ�ж����ȼ�����4
    CAN_First_InitConfig();						//CAN1��ʼ��
    CAN_Second_InitConfig();					//CAN2��ʼ��
	UART5_DBUS_InitConfig();					//ң������ʼ��
	Beep_Configuration();
	
	Queue_CANSend = xQueueCreate(64,sizeof(CanSend_Type));

#if Sentry == 1
	//��̨��ʼ��
	Kalman_Parameter_Init();
	kalman_filter_init(&motionfliter,&Kalman_init);
	KalMan_PramInit();
	Cloud_PID_Param();
	USART_mpu9250_InitConfig();
	USART_MiniPC_InitConfig();
	PWM_Configuration();
	ECS_Init();
	Laser_Init();
	
    
#elif Sentry == 0
	//���̳�ʼ��
	Switch_Init();
	Chassis_Parm();
	
	//����ϵͳ���
	communicate_param_init();
	osThreadDef(unpackTask, judge_unpack_task, osPriorityNormal, 0, 512);
	judge_unpack_task_t = osThreadCreate(osThread(unpackTask), NULL);
	UART4_Judge_InitConfig();
	
#endif

    //CAN��������
    xTaskCreate(Task_CANSend,
                "Task_CANSend",
                200,
                NULL,
                3,
                NULL);
#if Sentry == 0

    //����������
    xTaskCreate(Chassis_Monitor,
                "Chassis_Monitor",
                100,
                NULL,
                4,
                NULL);

    //���̿�������
    xTaskCreate(Chassis_Task,
                "Chassis_Task",
                512,
                NULL,
                3,
                NULL);
				
	//ͨѶ����
	xTaskCreate(Task_Communication,
				        "Task_Communication",
				        100,
			        	NULL,
			        	2,
			        	NULL);



#elif Sentry == 1
    //����������
    xTaskCreate(Gimbal_Monitor,
                "Gimbal_Monitor",
                100,
                NULL,
                4,
                NULL);

    //��̨��������
    xTaskCreate(Gimbal_Task,
                "Gimbal_Task",
                512,
                NULL,
                3,
                NULL);
				
#endif
    DMA_Cmd(DMA1_Stream0, DISABLE);
    while(DMA_GetCmdStatus(DMA1_Stream0) != DISABLE);
    DMA_SetCurrDataCounter(DMA1_Stream0, 19);
    DMA_Cmd(DMA1_Stream0, ENABLE);


    vTaskDelay(500);
    vTaskDelete(NULL);   //ɾ��NULL����
}
