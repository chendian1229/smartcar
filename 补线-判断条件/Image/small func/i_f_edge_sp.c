#include "common.h"
#include "_small_func_.h"




/**********************************************************************************************
名    称：                    水平从中间同时往两边“找跳变沿”函数

功能描述： 水平从中间同时往两边找跳变沿
入口参数： 一维数组首地址line[]、范围（最大输入93即188个像素全行扫描）
出口参数： 跳变沿存在性及位置

备    注： 只是找跳变沿 并不会判断是上升沿还是下降沿
**********************************************************************************************/
void f__edge_sp_czj(uint8 line[],uint8 range,uint8 *flag_l,uint8 *left,uint8 *flag_r,uint8 *right)
{
  *flag_l=0;
  *flag_r=0;
  //最中间的两个像素点 93 94（下标从0开始）
  //注：忽略了93和94之间的跳变关系
  for( uint8 Cj=1 ; Cj<=range ; Cj++ )//若range==186/2==93 则全部扫描
  {
    //往左边扫
    if( (*flag_l)==0 )
    {
      if( line[93-Cj]==line[93-Cj+1] )
      {
      }
      else
      {
        *left=(93-Cj);
        *flag_l=1;
      }
    }
    //往右边扫
    if( (*flag_r)==0 )
    {
      if( line[94+Cj-1]==line[94+Cj] )
      {
      }
      else
      {
        *right=(94+Cj);
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
    *left =93-range;
  if(*flag_r==0)
    *right=94+range;
}
/**********************************************************************************************
名    称：                水平从某点开始同时往两边“找上升沿”函数

功能描述： 水平从某点开始往两边找上升沿
入口参数： 一维数组、起始的点、范围
出口参数： 是否存在标志位、位置

备    注： 仅当返回的标志位为1时 位置才会返回 不然*p_position指向的变量不会被更新
**********************************************************************************************/
uint8 f_Sedge_sp_cmd(uint8 line[],uint8 start_position,uint8 range,uint8 *p_position)
{
  uint8 flag_l=0;//往左找到上升沿标志位
  uint8 flag_r=0;//往右找到上升沿标志位
  int16 tmp_l;//被比较的左侧像素下标
  int16 tmp_r;//被比较的右侧像素下标
  
  for( uint8 Cj=1 ; Cj<=range ; Cj++ )
  {
    //往左边扫
    if( flag_l==0 )
    {
      //限幅 防止不在0~187范围内
      tmp_l=start_position-Cj;
      tmp_r=start_position-Cj+1;
      if( tmp_l<0 )
        tmp_l=0;
      if(tmp_r>187)
        tmp_r=187;
      
      //找上升沿
      if( line[tmp_l]==line[tmp_r] )
      {
      }
      else
      {
        if( line[tmp_l]<line[tmp_r] )
        {
          flag_l=1;
          *p_position=tmp_l;
        }
        else
        {
          //说明是下降沿 我们要找的是上升沿 故不要
        }
      }
    }
    //往右边扫
    if( flag_r==0 )
    {
      //限幅 防止不在0~187范围内
      tmp_l=start_position+Cj-1;
      tmp_r=start_position+Cj;
      if( tmp_l<0 )
        tmp_l=0;
      if(tmp_r>187)
        tmp_r=187;
      
      //找上升沿
      if( line[tmp_l]==line[tmp_r] )
      {
      }
      else
      {
        if( line[tmp_l]<line[tmp_r] )
        {
          flag_r=1;
          *p_position=tmp_l;
        }
        else
        {
          //说明是下降沿 我们要找的是上升沿 故不要
        }
      }
    }
    //判断是否还需要再执行循环
    if( (flag_l==1)||(flag_r==1) )
    {
      break;
    }
  }
  
  //返回是否找到上升沿标志位
  if( (flag_l==1)||(flag_r==1) ) 
    return 1;
  else
    return 0;
}
/**********************************************************************************************
名    称：                    水平从某点同时往两边“找下降沿”函数

功能描述： 水平从某点往两边找下降沿
入口参数： 一维数组、开始的点、范围
出口参数： 是否存在标志位、位置

备    注： 仅当返回的标志位为1时 位置才会更新
**********************************************************************************************/
uint8 f_Xedge_sp_cmd(uint8 line[],uint8 start_position,uint8 range,uint8 *p_position)
{
  uint8 flag_l=0;//往左找到上升沿标志位
  uint8 flag_r=0;//往右找到上升沿标志位
  int16 tmp_l;//被比较的左侧像素下标
  int16 tmp_r;//被比较的右侧像素下标
  
  for( uint8 Cj=1 ; Cj<=range ; Cj++ )
  {
    //往左边扫
    if( flag_l==0 )
    {
      //限幅 防止不在0~187范围内
      tmp_l=start_position-Cj;
      tmp_r=start_position-Cj+1;
      if( tmp_l<0 )
        tmp_l=0;
      if(tmp_r>187)
        tmp_r=187;
      
      //找上升沿
      if( line[tmp_l]==line[tmp_r] )
      {
      }
      else
      {
        if( line[tmp_l]>line[tmp_r] )
        {
          flag_l=1;
          *p_position=tmp_r;
        }
        else
        {
          //说明是上升沿 我们要找的是下降沿 故不要
        }
      }
    }
    //往右边扫
    if( flag_r==0 )
    {
      //限幅 防止不在0~187范围内
      tmp_l=start_position+Cj-1;
      tmp_r=start_position+Cj;
      if( tmp_l<0 )
        tmp_l=0;
      if(tmp_r>187)
        tmp_r=187;
      
      //找上升沿
      if( line[tmp_l]==line[tmp_r] )
      {
      }
      else
      {
        if( line[tmp_l]>line[tmp_r] )
        {
          flag_r=1;
          *p_position=tmp_r;
        }
        else
        {
          //说明是上升沿 我们要找的是下降沿 故不要
        }
      }
    }
    //判断是否还需要再执行循环
    if( (flag_l==1)||(flag_r==1) )
    {
      break;
    }
  }
  
  //返回是否找到上升沿标志位
  if( (flag_l==1)||(flag_r==1) ) 
    return 1;
  else
    return 0;
}
/**********************************************************************************************
名    称：                    遍历整个一行 返回所有跳变沿 下降沿

功能描述： 
入口参数： 
出口参数： 

备    注： 
**********************************************************************************************/
n_edge_t f__edge_sp_all(uint8 line[],edge_t edge[])
{
  n_edge_t n_edge;//函数返回值
  
  n_edge.all=0;//所有跳变沿个数清零
  n_edge.s_n=0;//上升沿个数清零
  n_edge.x_n=0;//下降沿个数清零
  
  for( uint8 Cj=0 ; Cj<(IMG_W_USED-1) ; Cj++ )
  {
    if( line[Cj]!=line[Cj+1] )
    {
      n_edge.all++;
      if( line[Cj]<line[Cj+1] )//黑0 < 白255 上升沿
      {
        edge[n_edge.all].sx = 1;
        edge[n_edge.all].wz = Cj;
        
        n_edge.s_n++;
      }
      else                     //白255 > 黑0 下降沿
      {
        edge[n_edge.all].sx = -1;
        edge[n_edge.all].wz = Cj+1;
        
        n_edge.x_n++;
      }
    }
  }
  
  return n_edge;
}
/**********************************************************************************************
名    称：                  从提取中线后的图像中查找中线坐标

功能描述： 
入口参数： 
出口参数： 

备    注： 如果没有找到中线坐标 标志位置0 且位置将不会被更新
**********************************************************************************************/
void f__midline(uint8 line[],uint8 gary_v,uint8 range,uint8 *flag,uint8 *position)
{
  *flag=0;
  //最中间的两个像素点 93 94（下标从0开始）
  //注：忽略了93和94之间的跳变关系
  for( uint8 Cj ; Cj<=range ; Cj++ )//若range==186/2==93 则全部扫描
  {
    //往左边扫
    if( (*flag)==0 )
    {
      if( line[93-Cj]==gary_v )
      {
        *flag=1;
        *position=93-Cj;
        break;
      }
    }
    //往右边扫
    if( (*flag)==0 )
    {
      if( line[94+Cj]==gary_v )
      {
        *flag=1;
        *position=94+Cj;
        break;
      }
    }
  }
}