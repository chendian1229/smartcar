#ifndef __MPU6050_H__
#define	__MPU6050_H__


void Configure_MPU6050();
void GetData_MPU6050(int16 data_buffer[]);
void Init_and_Cali(int16 data_output[]);


#endif