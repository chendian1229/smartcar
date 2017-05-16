#include "common.h"
#include "_small_func_.h"




/**********************************************************************************************
名称/功能：                从补完线后的某一行中提取中线位置

详细描述 ： 
入口参数 ： 
出口参数 ： 

备    注 ： 如果没有找到 函数返回0 且位置不会被更新
**********************************************************************************************/
uint8 f__midline_frm_line(uint8 line_bxh[],uint8 gary_v,uint8 *position)
{
  uint8 flag=0;//指示是否查找到中线位置 函数返回值
  
  //最中间的两个像素点 93 94（下标从0开始）
  for( uint8 Cj=0 ; Cj<=93 ; Cj++ )//range为93 即188个像素全部扫描
  {
    //往左边扫
    if( line_bxh[93-Cj]==gary_v )
    {
      flag=1;
      
      *position=93-Cj;
      break;
    }
    //往右边扫
    if( line_bxh[94+Cj]==gary_v )
    {
      flag=1;
      
      *position=94+Cj;
      break;
    }
  }
  
  return flag;
}
/**********************************************************************************************
名称/功能 ：                从补完线后的整幅图像中提取中线位置

详细描述  ： 
入口参数  ： 
出口参数  ： 

备    注  ： 给start_i传入前瞻即可 如果该行没找到 会一直往底行找 直到找着为止
**********************************************************************************************/
uint8 f__midline_frm_img(uint8 img_bxh[][IMG_W_USED],int8 start_i,uint8 gary_v)
{
  uint8 value_return = IMG_TARGET_POSITION;//找到的中线位置 函数返回值
  
  while(start_i<=79)
  {
    if( f__midline_frm_line(img_bxh[start_i],gary_v,&value_return)==1 )
    {
      break;
    }
    
    start_i++;
  }
  
  return value_return;
}