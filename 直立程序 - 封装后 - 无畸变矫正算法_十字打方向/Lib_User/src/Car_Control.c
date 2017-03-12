#include "common.h"

#include "ctr_motor.h"
#include "encoder.h"
#include "mpu6050.h"
#include "pid.h"

#include "Car_Control.h"




int16 mpu6050_init_cali[7];//存放用于mpu6050的初始化及校准的数据
int16 mpu6050_buf[7];      //存放mpu6050每次读取的数据

float acc_x;//单位m/s2
float acc_z;//单位m/s2
float gyr_y;//单位°/sec

float acc_x_init;//用于求初始角度
float acc_z_init;//同上
float gyr_y_cali;//用于校准陀螺仪

float angle_acc; //直接由acc分解而得
float angle_gyr; //直接由gyr积分而得
float angle_yjhb;//一阶互补而得
float angle_clhd;//由angle_yjhb处理后的 去掉小数点后抖动的数据 用于angle_pid函数

float speed_left;
float speed_right;
float speed_average;
float Speed_PID_Value;

int64 EncoderCounter_L=0;//左轮编码器计数
int64 EncoderCounter_R=0;//右轮编码器计数

int16 pwm_common;//由角度环PID算得的pwm值
int16 pwm_dir;   //方向环算的pwm值
int16 pwm_left;
int16 pwm_right;


int16 dir_pid_result;


/**********************************************************************************************
                                    直立控制初始化初始化
**********************************************************************************************/
void Car_Control_Init()
{
  //电机控制FTM模块初始化
  Init_Control_Motor();
  
  
  //正交解码模块初始化
  Encoder_Init();
  
  
  //MPU6050初始化及校准
  Configure_MPU6050();
  Init_and_Cali_MPU6050(mpu6050_init_cali);
  acc_x_init = mpu6050_init_cali[0] * 19.61330 / 32767; //转换为实际物理值m/s2
  acc_z_init = mpu6050_init_cali[2] * 19.61330 / 32767; //转换为实际物理值m/s2
  gyr_y_cali = mpu6050_init_cali[5] * 250.0 / 32767;    //转换为实际物理值°/s
  angle_gyr  = atan2(acc_x_init, acc_z_init) * 57.29578;//直接由gyr积分角度的初值
  angle_yjhb = atan2(acc_x_init, acc_z_init) * 57.29578;//一阶互补对应角度的初值
}
/**********************************************************************************************
                        直立控制函数(将这段代码放在10ms的定时器中断函数里)
**********************************************************************************************/
void Car_Control(float TargetSpeed)
{
  static uint8 flag=0;//调节方向控制的执行周期
  float K = 0.005;//一阶互补滤波系数
  
  
  int16 temp_left,temp_right;//用于编码器计数 速度计算 局部变量
  temp_left = Get_Encoder_Left();
  temp_right = Get_Encoder_Right();
  EncoderCounter_L+=temp_left;//编码器计数
  EncoderCounter_R-=temp_right;
  speed_left =( temp_left  )/5.0;//计算速度 编码器的角速度(r/s)
  speed_right=( -temp_right )/5.0;
  speed_average=(speed_left+speed_right)/2;
  Encoder_Clear();//左右编码器清零
  
  
  GetData_MPU6050(mpu6050_buf);//读取数据
  acc_x = mpu6050_buf[0] * 19.61330 / 32767;
  acc_z = mpu6050_buf[2] * 19.61330 / 32767;
  gyr_y = mpu6050_buf[5] * 250.0 / 32767;
  
  
  gyr_y = gyr_y - gyr_y_cali;//消除陀螺仪温漂
  angle_acc=atan2(acc_x, acc_z) * 57.29578;//直接由acc分解出的值
  angle_gyr=angle_gyr-gyr_y*0.01;//直接由gyr积分出的值
  angle_yjhb = K * angle_acc + (1 - K) * (angle_yjhb - gyr_y * 0.01);//一阶互补滤波
  
  
  angle_clhd=(int)(angle_yjhb*100);
  angle_clhd=angle_clhd/100;
  
  
  extern float Kp_an;//角度环和速度环叠加需要用到
  Speed_PID_Value=Speed_PID( speed_average , TargetSpeed );
  //pwm_common = (int16)( -Angle_PID( angle_clhd+29.00 , Speed_PID_Value ) );//串级PID
  pwm_common = (int16)( -Angle_PID( angle_clhd+29+1 , 0 ) );//不控速
  
//  //锁定方向
//  temp = Dir_PID( (int16)(EncoderCounter_L-EncoderCounter_R) , 0 );
//  Ctr_Motor_L(pwm_common-temp);
//  Ctr_Motor_R(pwm_common+temp);
  
  
  flag++;
  if( flag == 5 )//1不分频 2二分频 3三分频 以此类推
  {
    flag=0;
    
    //方向控制周期:10ms*5=50ms
    extern uint8 position;

    dir_pid_result = Dir_PID( position , 49 );

   
  }
  
  
  Ctr_Motor_L(pwm_common+dir_pid_result);
  Ctr_Motor_R(pwm_common-dir_pid_result);
}