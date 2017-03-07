/*
 * 包含头文件
 */
#include "common.h"
#include "pid.h"




float Kp=70;
float Ki=0;
float Kd=190;
/**********************************************************************************************
																    Angle_PID函数
**********************************************************************************************/
int16 Angle_PID(float measured,float target)//-1000~+1000
{
	int16 result;//函数返回值
	float bias;//本次偏差
	static float bias_last=0;//上次偏差
	static float bias_integration=0;//偏差积分

	bias=measured-target;//计算偏差
	bias_integration+=bias;//对偏差积分
	result=(int16)( Kp*bias+Ki*bias_integration+Kd*(bias-bias_last) );
	bias_last=bias;//更新此值

	if(result>1000) result=1000;
	if(result<-1000) result=-1000;

	return result;
}