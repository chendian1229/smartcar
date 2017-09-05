#include "common.h"
#include "_small_func_.h"




/**********************************************************************************************
名    称：                        图像抽取及二值化函数

功能描述：图像抽取及二值化
入口参数：原灰度图像、阈值
出口参数：裁剪及二值化后图像
备    注：原始尺寸及裁剪后尺寸 详见common.h
**********************************************************************************************/
void ChouQu_ErZhi(uint8 gray[][IMG_W_YS],uint8 yz,uint8 binarized[][IMG_W_USED])
{
  uint8 i=0,j=0;
  uint8 *p=(uint8*)binarized;
  for(i=0;i<IMG_H_USED;i++)
  {
    for(j=0;j<IMG_W_USED;j++)
    {
      *p=gray[i][j];
      
      if( *p > yz )
      {
        *p=255;//对应白色
      }
      else
      {
        *p=0;//对应黑色
      }
      
      p++;
    }
  }
}