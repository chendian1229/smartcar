/*
 * 包含头文件
 */
#include "common.h"
#include "pid.h"

uint8 en_AngPID_output=0;
float Kp_an=90;//直立PD控制
float Ki_an=0;
float Kd_an=220;

uint8 en_SpdPID_output=0;
float Kp_sp=10;//速度PI控制 可以用的参数
float Ki_sp=1;
float Kd_sp=0;

uint8 en_DirPID_output=0;
float Kp_dir=7;//方向PD控制
float Ki_dir=0;
float Kd_dir=40;

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
  
//  if(result>500) result=500;//角度环pwm限幅
//  if(result<-500) result=-500;
  
  if(en_AngPID_output==1)
  {
    return result;
  }
  else
  {
    return 0;
  }
}
/**********************************************************************************************
                                     速度环PID(位置式)
**********************************************************************************************/
float bias_integration_sp=0;//偏差积分
float Speed_PID(float measured,float target)
{
  float result;//函数返回值
  float bias;//本次偏差
  static float bias_last=0;//上次偏差
  
  bias=measured-target;//计算偏差
  bias_integration_sp+=bias;//对偏差积分
  
  if( bias_integration_sp >  150 ) bias_integration_sp= 150;//速度环积分项输出限幅
  if( bias_integration_sp < -150 ) bias_integration_sp=-150;
  
  result=Kp_sp*bias+Ki_sp*bias_integration_sp+Kd_sp*(bias-bias_last);
  bias_last=bias;//更新此值
  
//  if(result>600) result=600;//速度环输出整体限幅
//  if(result<-600) result=-600;
  
  if(en_SpdPID_output==1)
  {
    return result;
  }
  else
  {
    return 0;
  }
}
/**********************************************************************************************
                                    方向环PID(位置式)
**********************************************************************************************/
float dir_pid_d_out;
float dir_pid_p_out;
int16 Dir_PID(int16 measured,int16 target)
{
  int16 result;//函数返回值
  
  int16 bias;//本次偏差
  static int16 bias_last=0;//上次偏差
  static int16 bias_integration=0;//偏差积分
  
  bias=measured-target;//计算偏差
  
  int16 bias_delta_for_p;
  bias_delta_for_p=bias-bias_last;
//  if(bias_delta_for_p> 20) bias_delta_for_p=20;
//  if(bias_delta_for_p<-20) bias_delta_for_p=-20;
  bias=bias_last+bias_delta_for_p;
  
  int16 bias_delta_for_d;
  bias_delta_for_d=bias-bias_last;
  if( bias_delta_for_d>5  ) bias_delta_for_d=5;
  if( bias_delta_for_d<-5 ) bias_delta_for_d=-5;
  
  bias_integration+=bias;//对偏差积分
  
  dir_pid_p_out=Kp_dir*bias;
  dir_pid_d_out=Kd_dir*bias_delta_for_d;
  
  result=(int16)( dir_pid_p_out+dir_pid_d_out );
  
  bias_last=bias;//更新此值
  
  if(result>400) result=400;//方向环pwm限幅
  if(result<-400) result=-400;
  
  if(en_DirPID_output==1)
  {
    return result;
  }
  else
  {
    return 0;
  }
}