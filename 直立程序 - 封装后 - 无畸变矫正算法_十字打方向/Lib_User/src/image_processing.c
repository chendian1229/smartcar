/*
 * 包含头文件
 */
#include "common.h"
#include "image_processing.h"

#define ThresholdValue 150

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
                                    图像抽取二值化
**********************************************************************************************/
uint8 dyd_yld_i_arr[60];
uint8 dyd_yld_j_arr[94]; 
void JiaoZheng(uint8 yuantuxiang[][94],uint8 jiaozhenghou[][94])
{
  float K1   = 1;
  float K2_x = 0.00025;
  float K2_y = 0.00025;
  
  
  //i j用于二位数组下标
  //x y用于桶形失真矫正坐标系
  uint8 i_jzh,j_jzh;//校正后
  uint8 i_yld,j_yld;//原来的
  int8 x_jzh,y_jzh;
  int8 x_yld,y_yld;
  
  static uint8 flag=0;
  if(flag==0)
  {
  //全初始化为255
  for(i_jzh=0;i_jzh<59;i_jzh++)
  {
    dyd_yld_i_arr[i_jzh]=255;
  }
  for(j_jzh=0;j_jzh<93;j_jzh++)
  {
    dyd_yld_j_arr[j_jzh]=255;
  }
 
  
  
  
  for(i_jzh=0;i_jzh<59;i_jzh++)
  {
    for(j_jzh=0;j_jzh<93;j_jzh++)
    {
      jiaozhenghou[i_jzh][j_jzh]=128;//先全赋为128 以便观察没有映射到的点
    }
  }
//      x_jzh=j_jzh-(int)(93/2);
//      y_jzh=i_jzh-(int)(59/2);
  
  
  for(i_yld=0;i_yld<59;i_yld++)//行
  {
    for(j_yld=0;j_yld<93;j_yld++)//列
    {
      x_yld=j_yld-(int)(93/2);
      y_yld=i_yld-(int)(59/2);
      
      x_jzh=(  K1*( 1 + K2_x * (x_yld*x_yld+y_yld*y_yld) )*x_yld  );
      y_jzh=(  K1*( 1 + K2_y * (x_yld*x_yld+y_yld*y_yld) )*y_yld  );

      i_jzh=y_jzh+(int)(59/2);
      j_jzh=x_jzh+(int)(93/2);
      
      if( (i_jzh<=59)&&(j_jzh<=93) )//不连续映射 需要限制范围
      {
        jiaozhenghou[i_jzh][j_jzh]=yuantuxiang[i_yld][j_yld];
        
        dyd_yld_i_arr[i_jzh]=i_yld;
        dyd_yld_j_arr[j_jzh]=j_yld;
      }
    }
  }
  
  //检查有没有没映射到的 有的话用周围任意一个坐标代替
  for(i_jzh=0;i_jzh<59;i_jzh++)
  {
    for(j_jzh=0;j_jzh<93;j_jzh++)
    {
      if( dyd_yld_i_arr[i_jzh]==255 )
      {
        //说明没有对应的点
        dyd_yld_i_arr[i_jzh]=dyd_yld_i_arr[i_jzh-1];
      }
      
      if(dyd_yld_j_arr[j_jzh]==255)
      {
        //说明没有对应的点
        dyd_yld_j_arr[j_jzh]=dyd_yld_j_arr[j_jzh-1];
      }
    }
  }
  
  
    flag=1;
  }
  //重新覆盖 消除没被映射到的点
  for(i_jzh=0;i_jzh<59;i_jzh++)
  {
    for(j_jzh=0;j_jzh<93;j_jzh++)
    {
      jiaozhenghou[i_jzh][j_jzh]=yuantuxiang[  dyd_yld_i_arr[i_jzh]  ][  dyd_yld_j_arr[j_jzh]  ];
    }
  }
  
  

  
  
}
/**********************************************************************************************
                                  水平扫描一维数组
**********************************************************************************************/
void Scan_Edge_ShuiPing(uint8 Line[],uint8 *flag_l,uint8 *Left,uint8 *flag_r,uint8 *Right)
{
  uint8 i;
  *flag_l=0;
  *flag_r=0;
  //最中间的两个像素点 46 47（下标从0开始）
  for(i=1;i<=46;i++)
  {
    //往左边扫
    if( *flag_l==0 )
    {
        if( Line[46-i]==Line[46-i+1] )
        {
        }
        else
        {
          *Left=(46-i);
          *flag_l=1;
        }
    }
    //往右边扫
    if( *flag_r==0 )
    {
        if( Line[47+i-1]==Line[47+i] )
        {
        }
        else
        {
          *Right=(47+i);
          *flag_r=1;
        }
    }
    //判断是否还需要再执行循环
    if( (*flag_l==1)&&(*flag_r==1) )
    {
      break;
    }
    
  }
  if(*flag_l==0)
    *Left=0;
  if(*flag_r==0)
    *Right=93;

}
/**********************************************************************************************
                                从最中间底部向上垂直扫描一列
**********************************************************************************************/
void Scan_Edge_ChuiZhi(uint8 Line[][94],uint8 *flag,uint8 *hight)
{
  uint8 i;
  *flag=0;
  for(i=58;i>=20;i--)//高度60 扫描59次
  {
    
    if( Line[i][48]==Line[i+1][48] )
    {
    }
    else
    {
      
      *flag=1;
      *hight=i;
      break;//终止整个for循环
    }
  }
}