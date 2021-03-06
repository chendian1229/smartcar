#include "common.h"
#include "_small_func_.h"




/**********************************************************************************************
名称/功能：                        图像抽取及二值化函数

详细描述 ： 图像抽取及二值化
入口参数 ： 原灰度图像、阈值
出口参数 ： 裁剪及二值化后图像

备    注 ： 原始尺寸及裁剪后尺寸 详见common.h
**********************************************************************************************/
void CaiJian_ErZhi(uint8 img_yssize_gray[][IMG_W_YS],uint8 yuzhi,uint8 img_usedsize_binarized[][IMG_W_USED])
{
  uint8 i=0,j=0;
  uint8 *p=(uint8*)img_usedsize_binarized;
  for(i=0;i<IMG_H_USED;i++)
  {
    for(j=0;j<IMG_W_USED;j++)
    {
      *p=img_yssize_gray[i][j];
      
      if( *p > yuzhi )
      {
        *p=255;//对应白色 255
      }
      else
      {
        *p=0;//对应黑色 0
      }
      
//      if( (j==0)||(j==187) ) *p=0;//是否将第0列和第187列染成黑色 酌情开启或关闭
      
      p++;
    }
  }
}