/*
 * 包含头文件
 */
#include "common.h"
#include "image_processing.h"
#include <math.h>

uint8 ThresholdValue=135;//130 对面屋


/**********************************************************************************************
                                    OTSU算法求阈值
**********************************************************************************************/
uint8 OTSU(uint8 image[][188])
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
    for(i=0;i<80;i=i+2)//40次 相当于求动态阈值时降低了分辨率 以提高运行速度
    {
      for(j=0;j<188;j=j+2)//94次
      {
        if( image[i][j] < T_temp )
        {
          w0=w0+1;
          u0=u0+image[i][j];
        }
        else
        {
          w1=w1+1;
          u1=u1+image[i][j];
        }
      }
    }
    //平均灰度
    u0=u0/w0;
    u1=u1/w1;
    //占比
    w0=w0/94*40;
    w1=w1/94*40;
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
/**********************************************************************************************
                                    图像抽取二值化
**********************************************************************************************/
void ChouQu_ErZhi(uint8 gray[][188],uint8 binarized[][188])
{
  uint8 i=0,j=0;
  uint8 *p=(uint8*)binarized;
  for(i=0;i<80;i++)//抽取80行 0~79行
  {
    for(j=0;j<188;j++)
    {
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
                                  水平扫描一维数组
**********************************************************************************************/
void Scan_Edge_ShuiPing(uint8 Line[],uint8 *flag_l,uint8 *Left,uint8 *flag_r,uint8 *Right)
{
  uint8 i;
  *flag_l=0;
  *flag_r=0;
  //最中间的两个像素点 93 94（下标从0开始）
  for(i=1;i<=70;i++)
  {
    //往左边扫
    if( *flag_l==0 )
    {
        if( Line[93-i]==Line[93-i+1] )
        {
        }
        else
        {
          *Left=(93-i);
          *flag_l=1;
        }
    }
    //往右边扫
    if( *flag_r==0 )
    {
        if( Line[94+i-1]==Line[94+i] )
        {
        }
        else
        {
          *Right=(94+i);
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
    *Right=187;
}
/**********************************************************************************************
                                从最中间底部向上垂直扫描一列
**********************************************************************************************/
uint8 Scan_Edge_ChuiZhi(uint8 Line[][188],uint8 start,uint8 final)
{
  uint8 flag=0;
  uint8 i;
  for(i=start;i>=final;i--)
  {
    //从第 93(94也可以) 列处 由下往上 垂直扫描跳变沿
    if( Line[i][93]==Line[i+1][93] )
    {
      //没扫到跳变沿
    }
    else
    {
      //扫到了跳变沿
      flag=1;
      break;//终止整个for循环
    }
  }
  return flag;
}
/**********************************************************************************************
                            输入两个点(i,j) 计算出斜率
**********************************************************************************************/
int8 Calculate_Angle(uint8 i_1,uint8 j_1,uint8 i_2,uint8 j_2)
{
  int8 Angle_return;//函数返回值 角度制 -90° ~ +90° (由 point_1 指向 point_2 的角度)
  
  float K;    //斜率
  float Angle;//用于高精度计算
  
  float X_1,Y_1;
  float X_2,Y_2;
  
  X_1 = j_1 + 1;//坐标变换
  Y_1 = IMAGE_ARR_HEIGHT - i_1;
  X_2 = j_2 + 1;
  Y_2 = IMAGE_ARR_HEIGHT - i_2;
  
  if( j_1!=j_2)//这里用j来判断(uint8型) 而非用X来判断(float型)
  {
    //斜率存在
    K = (Y_1-Y_2) / (X_1-X_2) ;
    Angle = atan(K);//__EFF_NS    __ATTRIBUTES double atan(double);
    Angle = Angle * 180 / 3.141593;//7位有效数字 弧度转角度
    
    Angle_return = (int8)Angle;// int8 <-- float
  }
  else
  {
    //斜率不存在
    if( Y_1>Y_2 )
    {
      Angle_return = -90;
    }
    else//Y_1<Y_2 很明显 斜率判断法里 不可能出现 Y_1==Y_2 这里就不再判断这种情况了
    {
      Angle_return = 90;
    }
  }
  
  
  return Angle_return;
}
/**********************************************************************************************
                           输入两个点(i,j) 在图像上连一条线
**********************************************************************************************/
//void Connect_Line_On_ImageArr(uint8 img_arr[][IMAGE_ARR_WIDTH],uint8 i_1,uint8 j_1,uint8 i_2,uint8 j_2)
//{
//  uint8 i_tmp,j_tmp;//用于交换
//  uint8 i_c,j_c;//用于循环
//  
//  if( j_1!=j_2 )
//  {
//    if( i_1 > i_2 )
//    {
//      i_tmp=i_1;
//      i_1=i_2;
//      i_2=i_tmp;
//      
//      j_tmp=j_1;
//      j_1=j_2;
//      j_2=j_tmp;
//    }
//    else
//    {
//    }
//    
//    //计算斜率
//    float K;
//    float X_1,Y_1;
//    float X_2,Y_2;
//    X_1 = j_1 + 1;//坐标变换
//    Y_1 = IMAGE_ARR_HEIGHT - i_1;
//    X_2 = j_2 + 1;
//    Y_2 = IMAGE_ARR_HEIGHT - i_2;
//    K = (Y_1-Y_2) / (X_1-X_2) ;
//    
//    float X,Y;
//    float b;
//    
//    b=Y_1;
//    for( j_c=j_1 ; j_c<=j_2 ; j_c++ )//X=j+1
//    {
//      X = j_c + 1;
//      Y = K * X + b;
//      
//      //坐标反变换+画点
//      img_arr[ (uint8)(IMAGE_ARR_HEIGHT-Y) ][ j_c ]=127;//灰度为127
//    }
//  }
//  else
//  {
//    //判断y_1 y_2大小 交换为y_1<y_2
//    //from y1~y2 画竖线
//    
//    
//    //if(Y_1)
//    
//  }
//}