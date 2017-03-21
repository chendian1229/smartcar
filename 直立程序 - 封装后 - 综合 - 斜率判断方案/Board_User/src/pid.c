/*
 * 包含头文件
 */
#include "common.h"
#include "pid.h"


float Kp_an=140;//直立PD控制
float Ki_an=0;
float Kd_an=220;

//float Kp_sp=0.08;//速度PI控制   可以用的参数 备份一下
//float Ki_sp=0.025;
//float Kd_sp=0;

float Kp_sp=0.08;//速度PI控制   可以用的参数 备份一下
float Ki_sp=0.015;
float Kd_sp=0;


float Kp_dir=2.5;//方向PD控制   //speed=10  p 4 d 30
float Ki_dir=0;
float Kd_dir=10;


/**********************************************************************************************
                                    角度环PID(位置式)
**********************************************************************************************/
int16 Angle_PID(float measured,float target)
{ 
  int16 result;//函数返回值
  
  float bias;//本次偏差
//  static float bias_last=0;//上次偏差
  static float bias_integration=0;//偏差积分
  
  bias=measured-target;//计算偏差
  bias_integration+=bias;//对偏差积分
  extern float gyr_y;                                                    //Kd_an*(bias-bias_last)
  result=(int16)( Kp_an*bias+Ki_an*bias_integration+Kd_an*(-gyr_y/100) );//Kd_an*(-gyr_y/100)
//  bias_last=bias;//更新此值
  
  if(result>500) result=500;//角度环pwm限幅
  if(result<-500) result=-500;
  
  return result;
}
/**********************************************************************************************
                                     速度环PID(位置式)
**********************************************************************************************/
float Speed_PID(float measured,float target)
{
  float result;//函数返回值
  float bias;//本次偏差
  static float bias_last=0;//上次偏差
  static float bias_integration=0;//偏差积分
  
  bias=measured-target;//计算偏差
  bias_integration+=bias;//对偏差积分
  
  if( bias_integration >  5/Ki_sp ) bias_integration= 5/Ki_sp;//速度环积分项输出限幅
  if( bias_integration < -5/Ki_sp ) bias_integration=-5/Ki_sp;//幅值:5
  
  result=Kp_sp*bias+Ki_sp*bias_integration+Kd_sp*(bias-bias_last);
  bias_last=bias;//更新此值
  
  if(result>5) result=5;//速度环输出整体限幅
  if(result<-5) result=-5;
  
  return result;
}
/**********************************************************************************************
                                    方向环PID(位置式)
**********************************************************************************************/
int16 Dir_PID(int16 measured,int16 target)
{
  int16 result;//函数返回值
  
  int16 bias;//本次偏差
  static int16 bias_last=0;//上次偏差
  static int16 bias_integration=0;//偏差积分
  
  bias=measured-target;//计算偏差
  bias_integration+=bias;//对偏差积分
  result=(int16)( Kp_dir*bias+Ki_dir*bias_integration+Kd_dir*(bias-bias_last) );
  bias_last=bias;//更新此值
  
  if(result>200) result=200;//方向环pwm限幅
  if(result<-200) result=-200;
  
  return result;
}