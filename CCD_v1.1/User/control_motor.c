/*
 * 包含头文件
 */
#include "common.h"
#include "MK60_port.h"
#include "MK60_ftm.h"
#include "control_motor.h"
/**********************************************************************************************
			初始化电机控制													  初始化FTM 用于驱动电机
**********************************************************************************************/
void Init_Control_Motor()
{
	//左轮电机部分Left
	ftm_pwm_init(FTM0, FTM_CH0,10*1000,0);//FTM0_CH0 PTC1 10Khz 占空比0
	ftm_pwm_init(FTM0, FTM_CH1,10*1000,0);//FTM0_CH1 PTC2 10Khz 占空比0
	//右轮电机部分Right
	ftm_pwm_init(FTM0, FTM_CH2,10*1000,0);//FTM0_CH2 PTC3 10Khz 占空比0
	ftm_pwm_init(FTM0, FTM_CH3,10*1000,0);//FTM0_CH3 PTC4 10Khz 占空比0
}
/**********************************************************************************************
																       控制左轮电机
入口参数：带符号的pwm值 符号表示方向
出口参数：无
**********************************************************************************************/
void Ctr_Motor_L(int16 pwm)//-1000~+1000
{
	if(pwm>0)
	{
	ftm_pwm_duty(FTM0,FTM_CH0,0);
	ftm_pwm_duty(FTM0,FTM_CH1, (uint32)pwm );
	}
	else
	{
		if(pwm<0)
		{
			ftm_pwm_duty(FTM0,FTM_CH0, (uint32)(-pwm) );
			ftm_pwm_duty(FTM0,FTM_CH1,0);
		}
		else
		{
			ftm_pwm_duty(FTM0,FTM_CH0,0);
			ftm_pwm_duty(FTM0,FTM_CH1,0);
		}
	}
}
/**********************************************************************************************
																       控制右轮电机
入口参数：带符号的pwm值 符号表示方向
出口参数：无
**********************************************************************************************/
void Ctr_Motor_R(int16 pwm)//-1000~+1000
{
	if(pwm>0)
	{
	ftm_pwm_duty(FTM0,FTM_CH2,0);
	ftm_pwm_duty(FTM0,FTM_CH3, (uint32)pwm );
	}
	else
	{
		if(pwm<0)
		{
			ftm_pwm_duty(FTM0,FTM_CH2, (uint32)(-pwm) );
			ftm_pwm_duty(FTM0,FTM_CH3,0);
		}
		else
		{
			ftm_pwm_duty(FTM0,FTM_CH2,0);
			ftm_pwm_duty(FTM0,FTM_CH3,0);
		}
	}
}