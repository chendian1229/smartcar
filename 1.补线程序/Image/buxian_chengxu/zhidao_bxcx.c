#include "common.h"
#include "_small_func_.h"
#include "_buxian_chengxu_.h"




/**********************************************************************************************
名称/功能：                           直道补线程序

详细描述 ： 
入口参数 ： 
出口参数 ： 

备    注 ： 无特殊要求时给形参bx_start_i传入79即可
**********************************************************************************************/
n_edge_t tmp_edge_num;   //跳变沿个数 临时
edge_t   tmp_edge_arr[6];//跳变沿数组 临时

uint8 kuandu_left;       //用于判断障碍在左侧还是右侧
uint8 kuandu_right;      //同上

zb_imgarr_t left_za_xgd; //左侧障碍下拐点
zb_imgarr_t left_zzd;    //左侧最低点

zb_imgarr_t right_za_xgd;//右侧障碍下拐点
zb_imgarr_t right_zzd;   //右侧最低点

extern int64 EncoderCounter_L;
extern int64 EncoderCounter_R;

extern uint8 enable_gbsdlx;
int8  flag_ywza;         //有无障碍标志位 0无障碍 -1左侧障碍 +1右侧障碍
uint8 flag_zabxnbjbp = 0;//障碍补线内部计步跑标志位 默认为0 即不计步

void zhidao_bxcx(uint8 img_wbx[][IMG_W_USED],uint8 bx_start_i,uint8 bx_range)
{
  int16 Ci;
  
  //判断有无障碍及有障碍时障碍在哪一侧
  if( flag_zabxnbjbp==0 )
  {
    for( Ci=79 ; Ci>=19 ; Ci-- )
    {
      //在第79行找到19行范围内找障碍
      tmp_edge_num=f__edge_sp_all(img_wbx[Ci],tmp_edge_arr);//水平遍历此行
      if( tmp_edge_num.all!=4 )
      {
        //无障碍
        flag_ywza=0;
      }
      else
      {
        //有障碍
        kuandu_left =tmp_edge_arr[1].wz-tmp_edge_arr[0].wz;
        kuandu_right=tmp_edge_arr[3].wz-tmp_edge_arr[2].wz;
        if(kuandu_left>kuandu_right)//右侧障碍
        {
          flag_ywza=1;
          
          right_za_xgd.i=Ci;
          tmp_edge_num=f__edge_sp_all(img_wbx[Ci-3],tmp_edge_arr);
          right_za_xgd.j=tmp_edge_arr[1].wz;
        }
        else//左侧障碍
        {
          flag_ywza=-1;
          
          left_za_xgd.i=Ci;
          tmp_edge_num=f__edge_sp_all(img_wbx[Ci-3],tmp_edge_arr);
          left_za_xgd.j=tmp_edge_arr[2].wz;
        }
        //有障碍时给左右侧最低点赋值开始
        tmp_edge_num=f__edge_sp_all(img_wbx[79],tmp_edge_arr);//水平遍历79行
        if( tmp_edge_num.all==4 )//有障碍时赋值
        {
          kuandu_left =tmp_edge_arr[1].wz-tmp_edge_arr[0].wz;
          kuandu_right=tmp_edge_arr[3].wz-tmp_edge_arr[2].wz;
          if(kuandu_left>kuandu_right)
          {
            //右侧障碍赋值
            left_zzd.i=79;
            left_zzd.j=tmp_edge_arr[0].wz;
            
            right_zzd.i=79;
            right_zzd.j=tmp_edge_arr[1].wz;
          }
          else
          {
            //左侧障碍赋值
            left_zzd.i=79;
            left_zzd.j=tmp_edge_arr[2].wz;
            
            right_zzd.i=79;
            right_zzd.j=tmp_edge_arr[3].wz;
          }
        }
        if( tmp_edge_num.all==2 )//无障碍时赋值
        {
          left_zzd.i=79;
          left_zzd.j=tmp_edge_arr[0].wz;
          
          right_zzd.i=79;
          right_zzd.j=tmp_edge_arr[1].wz;
        }
        
        break;//查找到障碍了 终止for循环 终止查找障碍
      }
      
    }
  }
  
  if( (flag_ywza==-1)||(flag_ywza==1) )
  {
    //直道赛道类型下一旦识别出障碍 不允许再改变赛道类型
    if( enable_gbsdlx==1 ) enable_gbsdlx=0;
    
    if( flag_ywza==-1 )//左侧障碍
    {
      zb_imgarr_t left_za_xgd_tmp;
      left_za_xgd_tmp=left_za_xgd;
      left_za_xgd_tmp.i=left_za_xgd.i-4;
      
      xiang_shang_hua_yan_shen_xian(img_wbx,0,left_za_xgd,left_za_xgd_tmp);
      liang_dian_lian_xian(img_wbx,0,left_za_xgd,left_zzd);
      
      if( (left_za_xgd.i==79)&&(flag_zabxnbjbp==0) )
      {
        flag_zabxnbjbp=1;
        
        EncoderCounter_L=0;
        EncoderCounter_R=0;
      }
      
    }
    if( flag_ywza==1 )//右侧障碍
    {
      zb_imgarr_t right_za_xgd_tmp;
      right_za_xgd_tmp=right_za_xgd;
      right_za_xgd_tmp.i=right_za_xgd.i-4;
      
      xiang_shang_hua_yan_shen_xian(img_wbx,0,right_za_xgd,right_za_xgd_tmp);
      liang_dian_lian_xian(img_wbx,0,right_za_xgd,right_zzd);
      
      if( (flag_zabxnbjbp==0)&&(right_za_xgd.i==79) )
      {
        flag_zabxnbjbp=1;
        
        EncoderCounter_L=0;
        EncoderCounter_R=0;
      }
    }
    
  }
  
  if( (flag_zabxnbjbp==1)&&((EncoderCounter_L+EncoderCounter_R)>4500) )
  {
    flag_zabxnbjbp=0;
    enable_gbsdlx=1;
  }
  
  wandao_bxcx(img_wbx,79,20);
}