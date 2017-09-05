/*
 * 包含头文件
 */
#include "common.h"
#include "MK60_port.h"
#include "MK60_i2c.h"
#include "mpu6050.h"
/**********************************************************************************************
															       配置MPU6050
**********************************************************************************************/
void Configure_MPU6050()
{
	i2c_init(I2C0,400*1000);

	i2c_write_reg(I2C0,0x68,0x6b,0x00);
}
/**********************************************************************************************
															     从MPU6050获取数据
**********************************************************************************************/
void GetData_MPU6050(int16 data_buffer[])
{
	uint8 byte[14];//定义为uint8 即unsigned char型
	byte[0]=i2c_read_reg(I2C0,0x68,0x3b);//acc_x
	byte[1]=i2c_read_reg(I2C0,0x68,0x3c);data_buffer[0]=byte[0]<<8|byte[1];

	byte[2]=i2c_read_reg(I2C0,0x68,0x3d);//acc_y
	byte[3]=i2c_read_reg(I2C0,0x68,0x3e);data_buffer[1]=byte[2]<<8|byte[3];

	byte[4]=i2c_read_reg(I2C0,0x68,0x3f);//acc_z
	byte[5]=i2c_read_reg(I2C0,0x68,0x40);data_buffer[2]=byte[4]<<8|byte[5];

	byte[6]=i2c_read_reg(I2C0,0x68,0x41);//temp
	byte[7]=i2c_read_reg(I2C0,0x68,0x42);data_buffer[3]=byte[6]<<8|byte[7];

	byte[8]=i2c_read_reg(I2C0,0x68,0x43);//gyr_x
	byte[9]=i2c_read_reg(I2C0,0x68,0x44);data_buffer[4]=byte[8]<<8|byte[9];

	byte[10]=i2c_read_reg(I2C0,0x68,0x45);//gyr_y
	byte[11]=i2c_read_reg(I2C0,0x68,0x46);data_buffer[5]=byte[10]<<8|byte[11];

	byte[12]=i2c_read_reg(I2C0,0x68,0x47);//gyr_z
	byte[13]=i2c_read_reg(I2C0,0x68,0x48);data_buffer[6]=byte[12]<<8|byte[13];
}
/**********************************************************************************************
													  获取初始加速度计值及陀螺仪校准值
												注：data_output数组保存校准及初始化后的数值
**********************************************************************************************/
void Init_and_Cali(int16 data_output[])
{
	int16 mpu6050_buffer[7];//存放来自MPU6050的数据
	int32 mpu6050_buffer_sum[7]={0};//用来求和 一定要初始化为0

	int16 i;
	for(i=0;i<1000;i++)//采样1000次 求和
	{
		GetData_MPU6050(mpu6050_buffer);

		//int32 <-- int16
		mpu6050_buffer_sum[0]+=mpu6050_buffer[0];//acc_x
		mpu6050_buffer_sum[1]+=mpu6050_buffer[1];//acc_y
		mpu6050_buffer_sum[2]+=mpu6050_buffer[2];//acc_z
		mpu6050_buffer_sum[3]+=mpu6050_buffer[3];//temp
		mpu6050_buffer_sum[4]+=mpu6050_buffer[4];//gyr_x
		mpu6050_buffer_sum[5]+=mpu6050_buffer[5];//gyr_y
		mpu6050_buffer_sum[6]+=mpu6050_buffer[6];//gyr_z
	}
	//求平均值
	mpu6050_buffer_sum[0]/=1000;//acc_x
	mpu6050_buffer_sum[1]/=1000;//acc_y
	mpu6050_buffer_sum[2]/=1000;//acc_z
	mpu6050_buffer_sum[3]/=1000;//temp
	mpu6050_buffer_sum[4]/=1000;//gyr_x
	mpu6050_buffer_sum[5]/=1000;//gyr_y
	mpu6050_buffer_sum[6]/=1000;//gyr_z

	//通过data_output数组在函数间传递数据
	//int16 <-- int32
	data_output[0]=mpu6050_buffer_sum[0];
	data_output[1]=mpu6050_buffer_sum[1];
	data_output[2]=mpu6050_buffer_sum[2];
	data_output[3]=mpu6050_buffer_sum[3];
	data_output[4]=mpu6050_buffer_sum[4];
	data_output[5]=mpu6050_buffer_sum[5];
	data_output[6]=mpu6050_buffer_sum[6];
}