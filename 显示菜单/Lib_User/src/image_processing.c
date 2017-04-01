/*
 * 包含头文件
 */
#include "common.h"
#include "image_processing.h"

#define ThresholdValue 140

/**********************************************************************************************
                                    图像抽取二值化
**********************************************************************************************/
void ChouQu_ErZhi(uint8 gray[][188],uint8 binarized[][94])
{
  uint8 i=0,j=0;
  uint8 *p=(uint8*)binarized;
  for(i=0;i<120;i=i+2)//像素抽取单行 i=i+2 j=j+2，像素由120*188变为60*94
  {
    for(j=0;j<188;j=j+2)
    {//94*60=5640次循环
      *p=gray[i][j]; 	

      if(*p>ThresholdValue)
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
/**********************************************************************************************
                              扫描一维数组上升沿和下降沿
**********************************************************************************************/
void Scan_Edge(uint8 Line[],uint8 *Rising_Number,uint8 Rising[],uint8 *Falling_Number,uint8 Falling[])
{
  int i;
  *Rising_Number=0;//清零上升沿和下降沿计数
  *Falling_Number=0;
  for(i=0;i<3;i++)
  {
    Rising[i]=0;
    Falling[i]=0;
  }
  for(i=0;i<93;i++)
  {
    if( Line[i]!=Line[i+1] )//从二值化后的数据中查找 只可能为 0或255
    {//不相等
      if(Line[i]<Line[i+1])
      {//上升沿 黑->白
        (*Rising_Number)++;
        Rising[ *Rising_Number - 1 ]=i;
      }
      else
      {//下降沿 白->黑
        (*Falling_Number)++;
        Falling[ *Falling_Number - 1 ]=i+1;
      }
    }
    else
    {//说明相等 什么也不需要干
    }
  }
}