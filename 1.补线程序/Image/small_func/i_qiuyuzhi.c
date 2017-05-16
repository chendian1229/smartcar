#include "common.h"
#include "_small_func_.h"


/**********************************************************************************************
名    称：                              图像裁剪

详细描述： 将图像从原始尺寸裁剪为实际所使用的尺寸
入口参数： 
出口参数： 

备    注： 
**********************************************************************************************/
void CaiJian(uint8 img_yssize[][IMG_W_YS],uint8 img_usedsize[][IMG_W_USED])
{
  uint8 i=0,j=0;
  uint8 *p=(uint8*)img_usedsize;
  for(i=0;i<IMG_H_USED;i++)
  {
    for(j=0;j<IMG_W_USED;j++)
    {
      *p=img_yssize[i][j];
      
      p++;
    }
  }
}
/**********************************************************************************************
名    称：                         OTSU算法求阈值

详细描述： 从实际所使用的尺寸的图像中求阈值
入口参数： 
出口参数： 

备    注： 可通过宏定义START FINAL STEP调整函数参数 以优化运行速度
**********************************************************************************************/
uint8 OTSU_FRM_USEDSIEZ_GRAY(uint8 img_usedsize_gray[][IMG_W_USED])
{
  uint8 i,j;
  uint8 T_temp;  //分割阈值
  uint8 T_return;//遍历法求得最佳阈值 函数返回值
  
  float w0,u0;//前景(目标) 灰度值小
  float w1,u1;//背景
  float g;
  float g_max;
  
  //遍历灰度值范围
  #define START 100
  #define FINAL 175
  #define STEP  2
  for(T_temp=START;T_temp<=FINAL;T_temp=T_temp+STEP)
  {
    w0=0;w1=0;u0=0;u1=0;
    for(i=0;i<IMG_H_USED;i=i+2)//IMG_H_USED/2次 可以提高运行速度
    {
      for(j=0;j<IMG_W_USED;j=j+2)//IMG_W_USED/2次
      {
        if( img_usedsize_gray[i][j] < T_temp )
        {
          w0=w0+1;
          u0=u0+img_usedsize_gray[i][j];
        }
        else
        {
          w1=w1+1;
          u1=u1+img_usedsize_gray[i][j];
        }
      }
    }
    //平均灰度
    u0=u0/w0;
    u1=u1/w1;
    //占比
    w0=w0/(IMG_H_USED*IMG_W_USED/4);
    w1=w1/(IMG_H_USED*IMG_W_USED/4);
    //类间方差
    g=w0*w1*(u0-u1)*(u0-u1);//T_temp计算出g
    //第 1 个阈值
    if( T_temp==START )
    {
      g_max=g;
      T_return=T_temp;
    }
    //第 2 3 4... 个阈值
    else
    {
      if( g>g_max )
      {
        //更新g_max及与之对应的T_return
        g_max=g;
        T_return=T_temp;
      }
    }
    //本次T_temp计算结束
  }
  #undef START
  #undef FINAL
  #undef STEP
  
  return T_return;
}