/****************************************************************************
 *  Copyright (C) 2018 RoboMaster.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of?
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.? See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/
/** @file judgement_info.h
 *  @version 1.0
 *  @date June 2017
 *
 *  @brief the information from judgement system
 *
 *  @copyright 2017 DJI RoboMaster. All rights reserved.
 *
 */

#ifndef __JUDGEMENT_INFO_H__
#define __JUDGEMENT_INFO_H__

#include "struct.h"
#include "constant.h"
#define JUDGE_FRAME_BUFLEN 200
extern u8 JudgeFrameCount;
/**
  * @brief  judgement data command id
  */
typedef enum
{
	GAME_STATE_DATA_ID = 0x0001,  //����״̬���ݣ� 1Hz ���ڷ���
	GAME_RESULT_ID     = 0x0002, //����������ݣ������������� [6]
	ROBO_survive_DATA	 = 0x0003, //���������˴�����ݣ� 1Hz ���ڷ���
	
	REAL_FIELD_DATA_ID = 0x0101, //�����¼����ݣ��¼��ı����[5]
	FIELD_depot_DATA	 = 0x0102, //--���ز���վ������ʶ���ݣ������ı����
	depot_Fill_DATA		 = 0x0103, //--���󲹸�վ�������ݣ��ɲ����ӷ��ͣ����� 10Hz����RM �Կ�����δ���ţ�
	
	ROBO_STATE_DATA_ID = 0x0201, //--������״̬���ݣ� 10Hz ���ڷ��� [1]
	HEAT_POWER_DATA_ID = 0x0202, //--ʵʱ�����������ݣ� 50Hz ���ڷ��� [4]
	GAME_POSITION_ID	 = 0x0203, //������λ�����ݣ� 10Hz ���� [8]
	GAIN_BUFF_ID    	 = 0x0204, //�������������ݣ� ����״̬�ı���� [7]

	HURT_STATE_DATA		 = 0x0206, //�˺�״̬���ݣ��˺���������
	REAL_SHOOT_DATA_ID = 0x0207, //--ʵʱ������ݣ��ӵ��������		[3]						 
	STU_CUSTOM_DATA_ID = 0x0301, //--�����˼佻�����ݣ����ͷ��������ͣ����� 10Hz [100]


} judge_data_id_e;


//------------------------------------------------------------------------------------
//2019
//�����ӿ�Э��˵��
//1.����״̬���ݣ� 0x0001�� ����Ƶ�ʣ� 1Hz
typedef __packed struct
{
uint8_t game_type : 4;
uint8_t game_progress : 4;
uint16_t stage_remain_time;
} ext_game_state_t;

//2. ����������ݣ� 0x0002�� ����Ƶ�ʣ�������������
typedef __packed struct
{
uint8_t winner;
} ext_game_result_t;

//3. �����˴�����ݣ� 0x0003�� ����Ƶ�ʣ� 1Hz
typedef __packed struct
{
uint16_t robot_legion;
} ext_game_robot_survivors_t;

//4. �����¼����ݣ� 0x0101�� ����Ƶ�ʣ��¼��ı����
typedef __packed struct
{
uint32_t event_type;
} ext_event_data_t;

//5. ����վ������ʶ�� 0x0102�� ����Ƶ�ʣ������ı����
typedef __packed struct
{
uint8_t supply_projectile_id;
uint8_t supply_robot_id;
uint8_t supply_projectile_step;
uint8_t supply_projectile_num;
} ext_supply_projectile_action_t;

//6. ���󲹸�վ�����ӵ��� cmd_id (0x0103)������Ƶ�ʣ����� 10Hz�� RM �Կ�����δ����
typedef __packed struct
{
uint8_t supply_projectile_id;
uint8_t supply_robot_id;
uint8_t supply_num;
} ext_supply_projectile_booking_t;

//7. ����������״̬�� 0x0201�� ����Ƶ�ʣ� 10Hz
typedef __packed struct
{
uint8_t robot_id;
uint8_t robot_level;
uint16_t remain_HP;
uint16_t max_HP;
uint16_t shooter_heat0_cooling_rate;
uint16_t shooter_heat0_cooling_limit;
uint16_t shooter_heat1_cooling_rate;
uint16_t shooter_heat1_cooling_limit;
uint8_t mains_power_gimbal_output : 1;
uint8_t mains_power_chassis_output : 1;
uint8_t mains_power_shooter_output : 1;
} ext_game_robot_state_t;

//8. ʵʱ�����������ݣ� 0x0202�� ����Ƶ�ʣ� 50Hz
typedef __packed struct
{
uint16_t chassis_volt;
uint16_t chassis_current;
float chassis_power;			//�����������
uint16_t chassis_power_buffer;	//���̻�������
uint16_t shooter_heat0;			//17mm����
uint16_t shooter_heat1;			//42mm����
} ext_power_heat_data_t;

//9. ������λ�ã� 0x0203�� ����Ƶ�ʣ� 10Hz
typedef __packed struct
{
float x;
float y;
float z;
float yaw;
} ext_game_robot_pos_t;

//10. ���������棺 0x0204�� ����Ƶ�ʣ�״̬�ı����
typedef __packed struct
{
uint8_t power_rune_buff;
}ext_buff_musk_t;

//11. ���л���������״̬�� 0x0205�� ����Ƶ�ʣ� 10Hz
typedef __packed struct
{
uint8_t energy_point;
uint8_t attack_time;
} aerial_robot_energy_t;

//12. �˺�״̬�� 0x0206�� ����Ƶ�ʣ��˺���������
typedef __packed struct
{
uint8_t armor_id : 4;
uint8_t hurt_type : 4;
} ext_robot_hurt_t;

//13. ʵʱ�����Ϣ�� 0x0207�� ����Ƶ�ʣ��������
typedef __packed struct
{
uint8_t bullet_type;
uint8_t bullet_freq;
float bullet_speed;
} ext_shoot_data_t;

//���������˼佻������
//1. �������ݽ�����Ϣ�� 0x0301�� ����Ƶ�ʣ����� 10Hz
typedef __packed struct
{
uint16_t data_cmd_id;
uint16_t send_ID;
uint16_t receiver_ID;
}ext_student_interactive_header_data_t;

//1. �ͻ��� �ͻ����Զ������ݣ� cmd_id:0x0301������ ID:0xD180�� ����Ƶ�ʣ����� 10Hz
//typedef __pack struct
//{
//float data1;
//float data2;
//float data3;
//uint8_t masks;
//} client_custom_data_t

//2. �������� �����˼�ͨ�ţ� 0x0301������Ƶ�ʣ����� 10Hz
typedef __packed struct
{
uint8_t data[30];
} robot_interactive_data_t;


/**
  * @brief  the data structure receive from judgement
  */
typedef struct
{
    ext_game_state_t        game_state; //-1
    ext_game_result_t       game_result_data;//-2
    ext_game_robot_state_t  game_information;   //7. ����������״̬�� 0x0201�� ����Ƶ�ʣ� 10Hz
    ext_power_heat_data_t	power_heat;	        //8. ʵʱ�����������ݣ� 0x0202�� ����Ƶ�ʣ� 50Hz
    ext_game_robot_pos_t    game_robo_pos;//-9
    ext_buff_musk_t         get_buff_data;//-10
    ext_robot_hurt_t        blood_changed_data;//-12
    ext_shoot_data_t        real_shoot_data;   //13
    ext_event_data_t        rfid_data;	//-4
	ext_student_interactive_header_data_t	robot_communicate_id;
	robot_interactive_data_t				robot_communicate;
} receive_judge_t;


/* data send (forward) */
/* data receive */
extern receive_judge_t judge_rece_mesg;

void  judgement_data_handle(uint8_t *p_frame);

#endif
