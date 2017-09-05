#include "common.h"
#include "_small_func_.h"
#include "_buxian_chengxu_.h"

#include "Car_Control.h"


/**********************************************************************************************
名称/功能：                           斑马线补线程序

详细描述 ： 
入口参数 ： 
出口参数 ： 

备    注 ： 
**********************************************************************************************/
uint8        counter_bmx=0;

extern uint8 enable_gbsdlx;   //改变赛道类型允许位 默认为1 即允许
uint8        flg_msycbmx_r=0;
extern int64 EncoderCounter_L;
extern int64 EncoderCounter_R;

void bxcx_banmaxian(uint8 img_wbx[][IMG_W_USED])
{
  if(enable_gbsdlx==1) enable_gbsdlx=0;
  if(flg_msycbmx_r==0)
  {
    flg_msycbmx_r=1;
    EncoderCounter_L=0;
    EncoderCounter_R=0;
  }
  if(   ( flg_msycbmx_r==1 )&&( (EncoderCounter_L+EncoderCounter_R)>8000 )   )
  {
    flg_msycbmx_r=0;
    enable_gbsdlx=1;
    
    counter_bmx++;
    if(counter_bmx==2) Stop_Stand();
  }
  
  zb_imgarr_t I_mid_high;
  zb_imgarr_t I_mid_low;
  I_mid_high.i=79;
  I_mid_high.j=IMG_TARGET_POSITION;
  I_mid_low.i=0;
  I_mid_low.j=IMG_TARGET_POSITION;
  liang_dian_lian_xian(img_wbx,1,I_mid_high,I_mid_low);
}