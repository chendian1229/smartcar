#include "common.h"
#include "_small_func_.h"
#include "_buxian_chengxu_.h"




/**********************************************************************************************
名称/功能：                          右侧入环形补线程序

详细描述 ： 
入口参数 ： 
出口参数 ： 

备    注 ： 
**********************************************************************************************/
void bxcx_huanxing_ru_r(uint8 img_wbx[][IMG_W_USED])
{
  uint8 xgd_flag_l;//下左拐点是否找到标志位 √
  uint8 xgd_flag_r;//下右拐点是否找到标志位
  zb_imgarr_t xgd_l;//下左拐点 √
  zb_imgarr_t xgd_r;//下右拐点
  
  //提取环形下左右拐点
  f__guaidian_X(img_wbx,&xgd_flag_l,&xgd_l,&xgd_flag_r,&xgd_r);
  
//  uint8 yxcb_flag_r;
  zb_imgarr_t yxcb_r;//环形中心的圆形侧边 右
  
  uint8 i_tmp;
  uint8 i_tmp_last;
  uint8 szkd;
  uint8 szkd_last;
  
  i_tmp_last = f__edge_cz_cktdbqvi( img_wbx , xgd_l.j , xgd_l.i );
  szkd_last =  xgd_l.i - i_tmp_last + 1;
  
  uint8 sfdz_flg=0;//宽度是否递增标志位
  for( uint8 j_tmp=(xgd_l.j+1) ; j_tmp<=187 ; j_tmp++ )
  {
    i_tmp = f__edge_cz_cktdbqvi( img_wbx , j_tmp , xgd_l.i );
    szkd =  xgd_l.i - i_tmp + 1;
    
    if( (szkd-szkd_last)>0)
    {
      sfdz_flg=1;
    }
    else
    {
      sfdz_flg=0;
    }
    
    if( ((szkd-szkd_last)>4)||((sfdz_flg==1)&&(i_tmp==0)&&(j_tmp>75)) )
    {
      yxcb_r.i=i_tmp_last;
      yxcb_r.j=j_tmp-1;
      
      break;
    }
    
    i_tmp_last=i_tmp;
    szkd_last=szkd;
  }
  
  //入环形补边界线
  liang_dian_lian_xian(img_wbx,0,xgd_l,yxcb_r);
  
  //补出赛道中线
  bxcx_wandao(img_wbx,79,20);
}
/**********************************************************************************************
名称/功能：                          左侧入环形补线程序

详细描述 ： 
入口参数 ： 
出口参数 ： 

备    注 ： 
**********************************************************************************************/
void bxcx_huanxing_ru_l(uint8 img_wbx[][IMG_W_USED])
{
  uint8 xgd_flag_l;//下左拐点是否找到标志位
  uint8 xgd_flag_r;//下右拐点是否找到标志位 √
  zb_imgarr_t xgd_l;//下左拐点
  zb_imgarr_t xgd_r;//下右拐点 √
  
  //提取环形下左右拐点
  f__guaidian_X(img_wbx,&xgd_flag_l,&xgd_l,&xgd_flag_r,&xgd_r);
  
//  uint8 yxcb_flag_l;
  zb_imgarr_t yxcb_l;//环形中心的圆形侧边 左
  
  uint8 i_tmp;
  uint8 i_tmp_last;
  uint8 szkd;
  uint8 szkd_last;
  
  i_tmp_last = f__edge_cz_cktdbqvi( img_wbx , xgd_r.j , xgd_r.i );//从右往左找
  szkd_last =  xgd_r.i - i_tmp_last + 1;
  
  uint8 sfdz_flg=0;//宽度是否递增标志位
  for( int16 j_tmp=(xgd_r.j-1) ; j_tmp>=0 ; j_tmp-- )
  {
    i_tmp = f__edge_cz_cktdbqvi( img_wbx , j_tmp , xgd_r.i );
    szkd =  xgd_r.i - i_tmp + 1;
    
    if( (szkd-szkd_last)>0)
    {
      sfdz_flg=1;
    }
    else
    {
      sfdz_flg=0;
    }
    
    if( ((szkd-szkd_last)>4)||((sfdz_flg==1)&&(i_tmp==0)&&(j_tmp<110)) )
    {
      yxcb_l.i=i_tmp_last;
      yxcb_l.j=j_tmp+1;
      
      break;
    }
    
    i_tmp_last=i_tmp;
    szkd_last=szkd;
  }
  
  //入环形补边界线
  liang_dian_lian_xian(img_wbx,0,xgd_r,yxcb_l);//环形下右拐点 圆形左侧边
  
  //补出赛道中线
  bxcx_wandao(img_wbx,79,20);
}