#ifndef __CONSTANT_H__
#define __CONSTANT_H__
#include "stdint.h"

/*���߲��ֿ���   0  */
/*��̨�������   1  */
#define Sentry   1//�ڱ����߲��ֿ���0   ��̨�������1

/*
*********************************************************************************************************
*                                           ������ֵ
*********************************************************************************************************
*/
#define  ANGLE_FIT_COE      0.7f

#define DIR_REVERSE_B    	-2820
#define DIR_REVERSE_F    	1400
/*********** shoot  message   ************/
#define SHOOT_FREQ  	 	100 //��Ƶms/����8��/s��
#define HEAT_MAX			  320 //��������
/*********** speed  message   ************/
#define FILTER_TIMES		1000.0f //�ٶ�ƽ������ʱ��ms������float������

/*********** ��̨����޷�   ************/
#define CloudOutMax 		20000
#define MiniPCMax		  	2.0f
#define MAX_COUNT 			799		  //encoder
#define	 S  				    188/800	//encoder��λ(mm)

#define MAXMOTORSPEED 		10000  //���ٶ��޷�(chassis_control)
#define Motor_Out_max 		10000  //��ת�ٶ��޷�(chassis_control)
#define Poked_out_Max 		7000

#define abs_f(x) ((x)>0? (x):(-(x)))

#define mat         arm_matrix_instance_f32 
#define mat_init    arm_mat_init_f32				//��ʼ��
#define mat_add     arm_mat_add_f32					//�ӷ�
#define mat_sub     arm_mat_sub_f32					//����
#define mat_mult    arm_mat_mult_f32				//�˷�
#define mat_trans   arm_mat_trans_f32				//ת��
#define mat_inv     arm_mat_inverse_f32		
/*********DBUS**********/
#define ClearShackTick      50      //����ʱ��50ms
/******************/
#define Power_MAX			20
#define Power_Buffer_MAX	200
#define Shooterheat_MAX		480
#define Shooterheat_Recover 250		//�Լ�����ǹ������
#define Shoot_Speed_MAX		29

#define Hero_Red			0x01
#define Engineering_Red		0x02
#define Infantry_Red_1		0x03
#define Infantry_Red_2		0x04
#define Infantry_Red_3		0x05
#define Air_Red				0x06
#define Sentry_Red			0x07
#define Hero_Blue			0x11
#define Engineering_Blue	0x12
#define Infantry_Blue_1		0x13
#define Infantry_Blue_2		0x14
#define Infantry_Blue_3		0x15
#define Air_Blue			0x16
#define Sentry_Blue			0x17

#endif
