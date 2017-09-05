#include "common.h"
#include "_small_func_.h"




/**********************************************************************************************
名称/功能：                        某点周围画正方形函

详细描述 ： 
入口参数 ： 
出口参数 ： 

备    注 ： 
**********************************************************************************************/
void mdzw_hua_zfx(uint8 img[][IMG_W_USED],uint8 gray_v,zb_imgarr_t I_zfx_mid)
{
  zb_imgarr_t ShangZuo,ShangYou;//正方形上左点坐标 正方形上右点坐标
  zb_imgarr_t XiaZuo,XiaYou;    //正方形下左点坐标 正方形下右点坐标
  
  int16 tmp_i;
  int16 tmp_j;
  
  #define ZFXKD_HZFX 4
  tmp_i=I_zfx_mid.i-ZFXKD_HZFX;
  tmp_j=I_zfx_mid.j-ZFXKD_HZFX;
  if(tmp_i<0  ) tmp_i=0;
  if(tmp_i>79 ) tmp_i=79;
  if(tmp_j<0  ) tmp_j=0;
  if(tmp_j>187) tmp_j=187;
  ShangZuo.i=tmp_i;
  ShangZuo.j=tmp_j;
  
  tmp_i=I_zfx_mid.i-ZFXKD_HZFX;
  tmp_j=I_zfx_mid.j+ZFXKD_HZFX;
  if(tmp_i<0  ) tmp_i=0;
  if(tmp_i>79 ) tmp_i=79;
  if(tmp_j<0  ) tmp_j=0;
  if(tmp_j>187) tmp_j=187;
  ShangYou.i=tmp_i;
  ShangYou.j=tmp_j;
  
  tmp_i=I_zfx_mid.i+ZFXKD_HZFX;
  tmp_j=I_zfx_mid.j+ZFXKD_HZFX;
  if(tmp_i<0  ) tmp_i=0;
  if(tmp_i>79 ) tmp_i=79;
  if(tmp_j<0  ) tmp_j=0;
  if(tmp_j>187) tmp_j=187;
  XiaYou.i=tmp_i;
  XiaYou.j=tmp_j;
  
  tmp_i=I_zfx_mid.i+ZFXKD_HZFX;
  tmp_j=I_zfx_mid.j-ZFXKD_HZFX;
  if(tmp_i<0  ) tmp_i=0;
  if(tmp_i>79 ) tmp_i=79;
  if(tmp_j<0  ) tmp_j=0;
  if(tmp_j>187) tmp_j=187;
  XiaZuo.i=tmp_i;
  XiaZuo.j=tmp_j;
  #undef ZFXKD_HZFX
  
  liang_dian_lian_xian(img,gray_v,ShangZuo,ShangYou);
  liang_dian_lian_xian(img,gray_v,ShangYou,XiaYou);
  liang_dian_lian_xian(img,gray_v,XiaYou,XiaZuo);
  liang_dian_lian_xian(img,gray_v,XiaZuo,ShangZuo);
}