#include "common.h"
#include "_small_func_.h"
#include "_buxian_chengxu_.h"




/**********************************************************************************************
名称/功能：                           直道补线程序

详细描述 ： 
入口参数 ： 
出口参数 ： 

备    注 ： 
**********************************************************************************************/
edge_t   tmp_edge_arr[6];//跳变沿数组 临时
n_edge_t tmp_edge_num;   //跳变沿个数 临时

uint8 kuandu_left;       //用于有障碍时判断障碍在左侧还是右侧
uint8 kuandu_right;      //同上

zb_imgarr_t left_za_xgd;      //左侧障碍下拐点
zb_imgarr_t left_za_xgd_tmp;  //左侧障碍下拐点临时
zb_imgarr_t left_za_zzd;      //左侧最低点
zb_imgarr_t right_za_xgd;     //右侧障碍下拐点
zb_imgarr_t right_za_xgd_tmp; //右侧障碍下拐点临时
zb_imgarr_t right_za_zzd;     //右侧最低点

int8  zalx;           //障碍类型 0无障碍 -1左侧障碍 +1右侧障碍

extern uint8 enable_gbsdlx;
uint8 flag_msycza = 0;//障碍补线内部计步跑标志位 默认为0 即不计步
extern int64 EncoderCounter_L;
extern int64 EncoderCounter_R;

void bxcx_zhidao(uint8 img_wbx[][IMG_W_USED])
{
  if( enable_gbsdlx==1 ) enable_gbsdlx=0;
  
  int16 Ci;
  
  /*提取左右边界点 开始*/
  #define TQBJ_RABGE  10
  #define TQBJ_StartI 79
  uint8 nomeaning_l;//用于最底部5行 从中间往两边扫赛道边界
  uint8 nomeaning_r;//同上
  
  uint8 n_bj_l=0;//搜索到的左边点界个数
  uint8 n_bj_r=0;//搜索到的右边点界个数
  zb_imgarr_t bj_left[IMG_H_USED]; //左边界点数组
  zb_imgarr_t bj_right[IMG_H_USED];//右边界点数组
  
  uint16 start_bj_j_left; //起始左边界点 j 值（前5行平均值）
  uint16 start_bj_j_right;//起始右边界点 j 值（前5行平均值）
  
  uint8 over_flag_l=0;       //是否结束扫描左边界标志位 0不结束 1结束
  uint8 over_flag_r=0;       //是否结束扫描右边界标志位 0不结束 1结束
  uint8 left_now,right_now;  //本次扫描所使用左右边界点的j值
  uint8 left_last,right_last;//下次扫描所使用左右边界点的j值（即本次结果）
  
  //先从最底部1行找边界
  start_bj_j_left=0;
  start_bj_j_right=0;
  
  Ci=TQBJ_StartI;//假如bx_start_i==79 则是79行 共计1行
  n_bj_l++;
  n_bj_r++;
  f__edge_sp_czj(img_wbx[Ci],93,&nomeaning_l,&(bj_left[n_bj_l-1].j),&nomeaning_r,&(bj_right[n_bj_r-1].j));
  bj_left[n_bj_l-1].i  = Ci;
  bj_right[n_bj_r-1].i = Ci;
  start_bj_j_left   += bj_left[n_bj_l-1].j;
  start_bj_j_right  += bj_right[n_bj_r-1].j;
  start_bj_j_left /=1;//将平均值作为以后的起始边界j值
  start_bj_j_right/=1;//同上
  
  //前1行扫完后 往后每行在上一行的左右边界附近查找本行边界 以节省时间 同时找的更远
  left_last  = start_bj_j_left;
  right_last = start_bj_j_right;
  for( Ci=(TQBJ_StartI-1) ; Ci>=0 ; Ci-- )
  {
    if(over_flag_l==0)
    {
      if( f_Sedge_sp_cmd(img_wbx[Ci],left_last,TQBJ_RABGE,&left_now)==1 )
      {
        n_bj_l++;
        
        bj_left[n_bj_l-1].i=Ci;
        bj_left[n_bj_l-1].j=left_now;
        
        left_last=left_now;
      }
      else
      {
        over_flag_l=1;
      }
    }
    
    if(over_flag_r==0)
    {
      if( f_Xedge_sp_cmd(img_wbx[Ci],right_last,TQBJ_RABGE,&right_now)==1 )
      {
        n_bj_r++;
        
        bj_right[n_bj_r-1].i=Ci;
        bj_right[n_bj_r-1].j=right_now;
        
        right_last=right_now;
      }
      else
      {
        over_flag_r=1;
      }
    }
    
    if( (over_flag_l==1)&&(over_flag_r==1) )
    {
      break;
    }
    
  }
  #undef TQBJ_RABGE
  #undef TQBJ_StartI
  /*提取左右边界点 结束*/
  
  //一定要保证左右边界点个数都大于60
  
  //判断有无障碍及有障碍时障碍在哪一侧
  if( flag_msycza==0 )
  {
    for( Ci=79 ; Ci>=29 ; Ci-- )//在第79行找到29行范围内找障碍 共计51行
    {
      tmp_edge_num=f__edge_sp_all(img_wbx[Ci],bj_left[79-Ci].j-5,bj_right[79-Ci].j+5,tmp_edge_arr);//水平遍历此行
      if( tmp_edge_num.all!=4 )
      {
        //无障碍
        zalx=0;
      }
      else
      {
        //有障碍
        kuandu_left =tmp_edge_arr[1].wz-tmp_edge_arr[0].wz;
        kuandu_right=tmp_edge_arr[3].wz-tmp_edge_arr[2].wz;
        if(kuandu_left>kuandu_right)
        {
          zalx=1;//障碍在右侧
          
          right_za_xgd.i=Ci-4;
          tmp_edge_num=f__edge_sp_all(img_wbx[Ci-4],bj_left[83-Ci].j-5,bj_right[83-Ci].j+5,tmp_edge_arr);
          right_za_xgd.j=tmp_edge_arr[1].wz;
        }
        else
        {
          zalx=-1;//障碍在左侧
          
          left_za_xgd.i=Ci-4;
          tmp_edge_num=f__edge_sp_all(img_wbx[Ci-4],bj_left[83-Ci].j-5,bj_right[83-Ci].j+5,tmp_edge_arr);
          left_za_xgd.j=tmp_edge_arr[2].wz;
        }
        break;//查找到障碍了 终止查找障碍
      }
    }
  }
  //有障碍时给左右侧最低点赋值开始
  if( (zalx==-1)||(zalx==1) )
  {
    tmp_edge_num=f__edge_sp_all(img_wbx[79],bj_left[1-1].j,bj_right[1-1].j,tmp_edge_arr);//水平遍历79行
    if( tmp_edge_num.all==4 )//当最底行有障碍时赋值
    {
      kuandu_left =tmp_edge_arr[1].wz-tmp_edge_arr[0].wz;
      kuandu_right=tmp_edge_arr[3].wz-tmp_edge_arr[2].wz;
      if(kuandu_left>kuandu_right)
      {
        //最底行有右侧障碍赋值
        left_za_zzd.i=79;
        left_za_zzd.j=tmp_edge_arr[0].wz;
        
        right_za_zzd.i=79;
        right_za_zzd.j=tmp_edge_arr[1].wz;
      }
      else
      {
        //最底行有左侧障碍赋值
        left_za_zzd.i=79;
        left_za_zzd.j=tmp_edge_arr[2].wz;
        
        right_za_zzd.i=79;
        right_za_zzd.j=tmp_edge_arr[3].wz;
      }
    }
    //当最底行无障碍时给左右侧最低点赋值
    if( tmp_edge_num.all==2 )
    {
      left_za_zzd.i=79;
      left_za_zzd.j=tmp_edge_arr[0].wz;
      
      right_za_zzd.i=79;
      right_za_zzd.j=tmp_edge_arr[1].wz;
    }
  }
  //长直道赛道类型下一旦识别出障碍 进行障碍补边界线
  if( (zalx==-1)||(zalx==1) )
  {
    if( zalx==-1 )//左侧障碍
    {
      //校正开始
      left_za_xgd.j=left_za_xgd.j-1;
      //校正结束
      
      left_za_xgd_tmp   = left_za_xgd;
      left_za_xgd_tmp.i = left_za_xgd.i-5;
      xiang_shang_hua_yan_shen_xian(img_wbx,0,left_za_xgd,left_za_xgd_tmp);
      
      left_za_zzd.j=left_za_xgd.j;
      liang_dian_lian_xian(img_wbx,0,left_za_xgd,left_za_zzd);
      
      if( (left_za_xgd.i>=67)&&(flag_msycza==0) )
      {
        flag_msycza=1;
        EncoderCounter_L=0;
        EncoderCounter_R=0;
      }
    }
    if( zalx==1 )//右侧障碍
    {
      right_za_xgd_tmp   = right_za_xgd;
      right_za_xgd_tmp.i = right_za_xgd.i-5;
      xiang_shang_hua_yan_shen_xian(img_wbx,0,right_za_xgd,right_za_xgd_tmp);
      
      right_za_zzd.j=right_za_xgd.j;
      liang_dian_lian_xian(img_wbx,0,right_za_xgd,right_za_zzd);
      
      if( (right_za_xgd.i>=67)&&(flag_msycza==0) )
      {
        flag_msycza=1;
        EncoderCounter_L=0;
        EncoderCounter_R=0;
      }
    }
  }
  //当出障碍计够步数时 恢复更新赛道类型
  if( (flag_msycza==1)&&((EncoderCounter_L+EncoderCounter_R)>6000) )
  {
    flag_msycza=0;
    enable_gbsdlx=1;
  }
  //补出赛道中线
  bxcx_wandao(img_wbx,79,20);
}