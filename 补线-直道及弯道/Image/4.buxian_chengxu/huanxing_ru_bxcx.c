#include "common.h"
#include "_small_func_.h"
#include "_buxian_chengxu_.h"



/**********************************************************************************************
名    称：                            入环形补线程序

功能描述： 
入口参数： 
出口参数： 

备    注： 
**********************************************************************************************/
void huanxing_ru_bxcx(uint8 img_bin[][IMG_W_USED])
{
  uint8 xgd_flag_l;//下左拐点是否找到标志位
  uint8 xgd_flag_r;//下右拐点是否找到标志位
  zb_imgarr_t xgd_l;//下左拐点
  zb_imgarr_t xgd_r;//下右拐点
  
  f__guaidian_X(img_bin,&xgd_flag_l,&xgd_l,&xgd_flag_r,&xgd_r);
  
  liang_dian_lian_xian(img_bin,125,xgd_l,xgd_r);
}
/**********************************************************************************************
名    称：                            出环形补线程序

功能描述： 
入口参数： 
出口参数： 

备    注： 
**********************************************************************************************/
void huanxing_chu_bxcx(uint8 img_bin[][IMG_W_USED])
{
}