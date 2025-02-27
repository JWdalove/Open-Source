#include "Kalman_Filter.h"
#include "stdint.h"
#include "constant.h"
#include "arm_math.h"

float A_data[16]  = {1,0,3.0,0,0,1,0,0.5,0,0,1.0,0,0,0,0,1.0};
float AT_data[16] = {0};
float HT_data[16] = {0};
float H_data[16]  = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
float Q_data[16]  = {1,0,0,0,0,1,0,0,0,0,0.2,0,0,0,0,0.1};
float R_data[16]  = {150,0,0,0,0,20,0,0,0,0,0.5,0,0,0,0,0.5};//误差

void Kalman_Parameter_Init(void)
{
    memcpy(&Kalman_init.A_data, A_data,sizeof(A_data));
    memcpy(&Kalman_init.AT_data, AT_data,sizeof(AT_data));
    memcpy(&Kalman_init.HT_data,HT_data,sizeof(HT_data));
    memcpy(&Kalman_init.Q_data,Q_data,sizeof(Q_data));
    memcpy(&Kalman_init.R_data,R_data,sizeof(R_data));
    memcpy(&Kalman_init.H_data,H_data,sizeof(H_data));
}

//-------------------------------------------------------
//						噪声协方差			漂移噪声协方差  //协方差

//-------------------------------------------------------
float angle_fit(float imu_m,float esc_m, int16_t offet)
{
    static float esc_m_trans;
    float fitting_angle ;
    esc_m_trans = (esc_m - offet + 4096)/ 8192.0f * 360.0f;

    fitting_angle = ANGLE_FIT_COE * imu_m + (1 - ANGLE_FIT_COE) * esc_m_trans;
    return fitting_angle;
}



float Slope_Calculate(int begin,int end,float *p)    //最小二乘法拟合斜率
{
    float xsum=0,ysum=0,xysum=0,x2sum=0;
    float i=0;
    float result=0;
    static float resultlast;
    p=p+begin;
    for(i=begin; i<end; i++)
    {
        xsum+=i;
        ysum+=*p;
        xysum+=i*(*p);
        x2sum+=i*i;
        p=p+1;
    }
    if((end-begin)*x2sum-xsum*xsum) //判断除数是否为零
    {
        result=((end-begin)*xysum-xsum*ysum)/((end-begin)*x2sum-xsum*xsum);
        resultlast=result;
    }
    else
    {
        result=resultlast;
    }
    return result;
}


/* second-order kalman filter on stm32 */


/*
//1. xhatminus(k)= A xhat(k-1)-------
//xhatminus 先验估计 								 | 预测部分基础
//2. Pminus（k） = A P(k-1) AT + Q        | 根据已看到的我们可以期望什么
//Pminus先验方差估计   ---------------

//3. K(k) = Pminus(k) HT / (H Pminus(k) HT + R)
//K卡尔曼增益 -------------------------|给出Kalman增益（或称之为混合比例）如何给定新信息相对已知信息的比重

//4. xhat(k) = xhatminus(k) + K(k) (z(k) - H xhatminus(k))--------
//xhat 后验估计									|得到“新的测量之后”，通过Kalman增益
//5. P(k) = (1-K(k)H)Pminus(k)					| 对xhatminus、Pminus做更新得到最优的xhat、P
//P协方差矩阵 后验方差估计------------------------------------------

//H状态变量到测量值的变换矩阵
//Q、R分别是过程噪声和测量噪声的协方差矩阵（R可以根据传感器数据大概估算出来，Q是依赖模型的准确度，需要调试）->对自己预测的不确定度
//A状态转移矩阵、过程增益矩阵
//z(k)测量值
//基本思想：预测值+测量值+他们各自的噪声->最优值
*/
kalman_filter_init_t Kalman_init;
kalman_filter_t			 motionfliter;
float testmat[6] = {1,1,1,1,1,1};
//矩阵初始化
void kalman_filter_init(kalman_filter_t *F,
                        kalman_filter_init_t *I)
{
    mat_init(&F->xhat,4,1,(float *)I->xhat_data);
    mat_init(&F->xhatminus,4,1,(float *)I->xhatminus_data);
    mat_init(&F->A,4,4,(float *)I->A_data);
    mat_init(&F->AT,4,4,(float *)I->AT_data);
    mat_init(&F->K,4,4,(float *)I->K_data);
    mat_init(&F->P,4,4,(float *)I->P_data);
    mat_init(&F->Pminus,4,4,(float *)I->Pminus_data);
    mat_init(&F->Q,4,4,(float *)I->Q_data);
    mat_init(&F->R,4,4,(float *)I->R_data);
    mat_init(&F->z,4,1,(float *)I->z_data);
    mat_init(&F->H,4,4,(float *)I->H_data);
    mat_init(&F->HT,4,4,(float *)I->HT_data);
    mat_trans(&F->A,&F->AT);
    mat_trans(&F->H,&F->HT);
}
//返回float类型的指针
float *kalman_filter_calc(kalman_filter_t *F, float signal1, float signal2,float signal3,float signal4)
{
    float TEMP_data[16] = {0, 0, 0, 0,0,0,0, 0,0,0,0,0,0,0,0,0 };
    float TEMP_data41[4] = {0, 0,0,0};
    mat TEMP,TEMP41;

    mat_init(&TEMP,4,4,(float *)TEMP_data);
    mat_init(&TEMP41,4,1,(float *)TEMP_data41);

    F->z.pData[0] = signal1;
    F->z.pData[1] = signal2;
    F->z.pData[2] = signal3;
    F->z.pData[3] = signal4;
    //1. xhat'(k)= A xhat(k-1)
    mat_mult(&F->A, &F->xhat, &F->xhatminus);

    //2. P'(k) = A P(k-1) AT + Q
    mat_mult(&F->A, &F->P, &F->Pminus);
    mat_mult(&F->Pminus, &F->AT, &TEMP);
    mat_add(&TEMP, &F->Q, &F->Pminus);

    //3. K(k) = P'(k) HT / (H P'(k) HT + R)
    mat_mult(&F->H, &F->Pminus, &F->K);
    mat_mult(&F->K, &F->HT, &TEMP);
    mat_add(&TEMP, &F->R, &F->K);

    testmat[4] = mat_inv(&F->K, &F->P);
    testmat[2] = mat_mult(&F->Pminus, &F->HT, &TEMP);
    testmat[5] = mat_mult(&TEMP, &F->P, &F->K);

    //4. xhat(k) = xhat'(k) + K(k) (z(k) - H xhat'(k))
    mat_mult(&F->H, &F->xhatminus, &TEMP41);
    mat_sub(&F->z, &TEMP41, &F->xhat);
    mat_mult(&F->K, &F->xhat, &TEMP41);
    mat_add(&F->xhatminus, &TEMP41, &F->xhat);

    //5. P(k) = (1-K(k)H)P'(k)
    testmat[1] = mat_mult(&F->K, &F->H, &F->P);
    testmat[0] = mat_sub(&F->Q, &F->P, &TEMP);
    testmat[3] = mat_mult(&TEMP, &F->Pminus, &F->P);

    F->filtered_value[0] = F->xhat.pData[0];
    F->filtered_value[1] = F->xhat.pData[1];
    F->filtered_value[2] = F->xhat.pData[2];
    F->filtered_value[3] = F->xhat.pData[3];
    return F->filtered_value;
}



