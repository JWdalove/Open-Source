#ifndef __KALMAN_FILTER_H__
#define __KALMAN_FILTER_H__

#include "stdint.h"
#include "arm_math.h"
#define mat         arm_matrix_instance_f32
#define mat_init    arm_mat_init_f32				//初始化
#define mat_add     arm_mat_add_f32					//加法
#define mat_sub     arm_mat_sub_f32					//减法
#define mat_mult    arm_mat_mult_f32				//乘法
#define mat_trans   arm_mat_trans_f32				//转置
#define mat_inv     arm_mat_inverse_f32			//逆矩阵

typedef struct
{
    float raw_value;
    float filtered_value[4];
    mat xhat, xhatminus, z, A, H, AT, HT, Q, R, P, Pminus, K;  //定义矩阵结构（行数、列数、数据内容）
} kalman_filter_t;

//矩阵初始化数据
typedef struct
{
    float raw_value;					//原始数值
    float filtered_value[4];
    float xhat_data[4], xhatminus_data[4], z_data[4],Pminus_data[16], K_data[16];
    float P_data[16];
    float AT_data[4], HT_data[16];
    float A_data[16];
    float H_data[16];
    float Q_data[16];
    float R_data[16];
} kalman_filter_init_t;

void Kalman_Parameter_Init(void);
extern kalman_filter_init_t Kalman_init;
extern kalman_filter_t			 motionfliter;
extern void kalman_filter_init(kalman_filter_t *F, kalman_filter_init_t *I);
float *kalman_filter_calc(kalman_filter_t *F, float signal1, float signal2,float signal3,float signal4);
float Slope_Calculate(int begin,int end,float *p);    //最小二乘法拟合斜率





#endif
