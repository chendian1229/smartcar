#include "common.h"
#include "_small_func_.h"
#include "_buxian_chengxu_.h"



extern int64 EncoderCounter_L;
extern int64 EncoderCounter_R;
extern uint8 enable_gbsdlx;    //改变赛道类型允许位 默认为1 即允许
/**********************************************************************************************
名称/功能：                          右侧出环形补线程序

详细描述 ： 
入口参数 ： 
出口参数 ： 

备    注 ： 
**********************************************************************************************/
uint8 flg_msychx_r=0;//马上要出环形并开启出环形计步跑标志位
void bxcx_huanxing_chu_r(uint8 img_wbx[][IMG_W_USED])
{
  int16 Ci;
  
  /*-----------提取左右边界----开始-----------*/
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
  /*-----------提取左右边界----结束-----------*/
  
  
  /*------从右边界找环形上右拐点----开始--------*/
  uint8 sgd_flag_r=0; //上右拐点找到标志位
  zb_imgarr_t sgd_r;  //上右拐点
  uint8 sgd_r_value_n;//上有拐点的n值
  
  slope_t last_K;//第1次算斜率
  zb_imgarr_t last_I_low;
  zb_imgarr_t last_I_high;
  
  slope_t tmp_K;//第2 3 4...次算斜率
  zb_imgarr_t tmp_I_low;
  zb_imgarr_t tmp_I_high;
  
  last_I_low =bj_right[0+1-1];
  last_I_high=bj_right[0+5-1];
  last_K=calcu_slope( covert_zb(last_I_low) , covert_zb(last_I_high) );
  if( last_K.exist==0 )
  {
    last_K.exist=1;//强制为斜率存在
    last_K.value=-1;//-45°
  }
  
  for( uint8 n_bj_r_counter=5 ; n_bj_r_counter<=(n_bj_r-4) ; n_bj_r_counter+=4 )
  {
    tmp_I_low =bj_right[0+n_bj_r_counter-1];
    tmp_I_high=bj_right[0+n_bj_r_counter-1+4];
    
    tmp_K=calcu_slope( covert_zb(tmp_I_low) , covert_zb(tmp_I_high) );
    if( tmp_K.exist==0 ) tmp_K=last_K;//省去了斜率不存在的情况
    
    if( (last_K.value<0)&&(tmp_K.value>0) )
    {
      sgd_r=tmp_I_low;
      sgd_r_value_n=0+n_bj_r_counter-1;
      sgd_flag_r=1;
      break;
    }
    
    last_K=tmp_K;
  }
  /*------从右边界找环形上右拐点----结束--------*/
  
  
  /*-----------找环形上左拐点----开始-----------*/
//  uint8 sgd_flg_l=0;//上左拐点找到标志位
  zb_imgarr_t sgd_l;//上左拐点
  sgd_l.i=0;//假如没有找到出环形的拐点 那么就会使用图像的左上角
  sgd_l.j=0;
  
  //关键变量：公共i值 右侧j值 左侧j值 水平宽度
  int8  last_n;
  uint8 last_i;
  uint8 last_j_r;
  uint8 last_j_l;
  uint8 kuandu_sp_last;
  last_n   = n_bj_r*4/5-1;                             //上次n值
  last_i   = bj_right[last_n].i;                       //上次i值 公共
  last_j_r = bj_right[last_n].j;                       //上次j值 右侧
  last_j_l = f_Sedge_sp_cybjqzbj(img_wbx[last_i],last_j_r);//上次j值 左侧
  kuandu_sp_last = last_j_r - last_j_l + 1;            //上次水平宽度
  
//  //将开始找的i值画线指示出来
//  lcd_draw_spx(last_i,BLUE);
  
  int8  tmp_n;
  uint8 tmp_i;
  uint8 tmp_j_r;
  uint8 tmp_j_l;
  uint8 kuandu_sp_tmp;
  for( tmp_n=(last_n-1) ; tmp_n>=0 ; tmp_n=tmp_n-2 )  //本次n值
  {
    tmp_i   = bj_right[tmp_n].i;                      //上次i值 公共
    tmp_j_r = bj_right[tmp_n].j;                      //本次j值 右侧
    tmp_j_l = f_Sedge_sp_cybjqzbj(img_wbx[tmp_i],tmp_j_r);//本次j值 左侧
    kuandu_sp_tmp = tmp_j_r - tmp_j_l + 1;            //本次水平宽度
    
    if( (kuandu_sp_tmp-kuandu_sp_last)>7 )
    {
      sgd_l.i=last_i;
      sgd_l.j=last_j_l;
//      sgd_flg_l=1;
      break;
    }
    
    last_i  = tmp_i;               //保存i值 公共
    last_j_l = tmp_j_l;            //保存j值 左侧
    kuandu_sp_last = kuandu_sp_tmp;//保存赛道宽度
  }
  /*-----------找环形上左拐点----结束-----------*/
  
  
  
  
  
  
  //一旦切换为出环形补线程序 立即暂停更新赛道类型 赛道类型将保持为出环形
  if( enable_gbsdlx==1 )
  {
    enable_gbsdlx=0;
  }
  //如果找到了环形的上右拐点 分一下两种情况：
  if( sgd_flag_r==1 )
  {
    //情况1：上右拐点不够低时 只是粗略地出环形补边界线
    if( (sgd_r.i)<=35 )//0~35
    {
      zb_imgarr_t I_tmp;
      I_tmp.i=bj_right[sgd_r_value_n+8].i;
      I_tmp.j=f_Sedge_sp_cybjqzbj(img_wbx[bj_right[sgd_r_value_n+4].i],bj_right[sgd_r_value_n+4].j);
      
      bj_left[0].j=bj_right[0].j-90;
      liang_dian_lian_xian(img_wbx,0,I_tmp,bj_left[0]);
    }
    //情况2：上右拐点足够低时 则认为马上要出环形了 开始计步出环形 开始精确地出环形补边界线
    else//36~79
    {
      if( (sgd_r.i)<=60 )//36~60 比较低
      {
        if( flg_msychx_r==0 )
        {
          flg_msychx_r=1;
        }
      }
      else//61~79 相当低了
      {
        if(flg_msychx_r==1)
        {
          flg_msychx_r=2;
          EncoderCounter_L=0;
          EncoderCounter_R=0;
        }
      }
    }
  }
  if( (flg_msychx_r==1)||(flg_msychx_r==2) )//精确地出环形补边界线
  {
    bj_left[0].j=bj_right[0].j-90;
    liang_dian_lian_xian(img_wbx,0,sgd_l,bj_left[0]);
  }
  //当出环形计步完成后 恢复更新赛道类型
  if(  (flg_msychx_r==2)&&( (EncoderCounter_L+EncoderCounter_R)>20000)  )//对应实际距离1.5m左右
  {
    flg_msychx_r=0;
    enable_gbsdlx=1;
  }
  
  //补出赛道中线
  bxcx_wandao(img_wbx,79,15);
}
/**********************************************************************************************
名称/功能：                          左侧出环形补线程序

详细描述 ： 
入口参数 ： 
出口参数 ： 

备    注 ： 
**********************************************************************************************/
uint8 flg_msychx_l=0;//马上要出环形并开启出环形计步跑标志位
void bxcx_huanxing_chu_l(uint8 img_wbx[][IMG_W_USED])
{
  int16 Ci;
  
  /*-----------提取左右边界----开始-----------*/
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
  /*-----------提取左右边界----结束-----------*/
  
  
  /*------从左边界找环形上左拐点----开始--------*/
  uint8 sgd_flag_l=0; //上左拐点找到标志位
  zb_imgarr_t sgd_l;  //上左拐点
  uint8 sgd_l_value_n;//上左拐点的n值
  
  slope_t last_K;//第1次算斜率
  zb_imgarr_t last_I_low;
  zb_imgarr_t last_I_high;
  
  slope_t tmp_K;//第2 3 4...次算斜率
  zb_imgarr_t tmp_I_low;
  zb_imgarr_t tmp_I_high;
  
  last_I_low =bj_left[0+1-1];
  last_I_high=bj_left[0+5-1];
  last_K=calcu_slope( covert_zb(last_I_low) , covert_zb(last_I_high) );
  if( last_K.exist==0 )
  {
    last_K.exist=1;//强制为斜率存在
    last_K.value=1;//45°
  }
  
  for( uint8 n_bj_l_counter=5 ; n_bj_l_counter<=(n_bj_l-4) ; n_bj_l_counter+=4 )
  {
    tmp_I_low =bj_left[0+n_bj_l_counter-1];
    tmp_I_high=bj_left[0+n_bj_l_counter-1+4];
    
    tmp_K=calcu_slope( covert_zb(tmp_I_low) , covert_zb(tmp_I_high) );
    if( tmp_K.exist==0 ) tmp_K=last_K;//省去了斜率不存在的情况
    
    if( (last_K.value>0)&&(tmp_K.value<0) )
    {
      sgd_l=tmp_I_low;
      sgd_l_value_n=0+n_bj_l_counter-1;
      sgd_flag_l=1;
      break;
    }
    
    last_K=tmp_K;
  }
  /*------从右边界找环形上右拐点----结束--------*/
  
  
  /*-----------找环形上左拐点----开始-----------*/
//  uint8 sgd_flg_r=0;//上右拐点找到标志位
  zb_imgarr_t sgd_r;//上右拐点
  sgd_r.i=0;//假如没有找到出环形的拐点 那么就会使用图像的右上角
  sgd_r.j=187;
  
  //关键变量：公共i值 右侧j值 左侧j值 水平宽度
  int8  last_n;
  uint8 last_i;
  uint8 last_j_l;
  uint8 last_j_r;//√
  uint8 kuandu_sp_last;
  last_n   = n_bj_l*4/5-1;                             //上次n值
  last_i   = bj_left[last_n].i;                       //上次i值 公共
  last_j_l = bj_left[last_n].j;                       //上次j值 右侧
  last_j_r = f_Xedge_sp_czbjqybj(img_wbx[last_i],last_j_l);//上次j值 左侧
  kuandu_sp_last = last_j_r - last_j_l + 1;            //上次水平宽度
  
//  //将开始找的i值画线指示出来
//  lcd_draw_spx(last_i,BLUE);
  
  int8  tmp_n;
  uint8 tmp_i;
  uint8 tmp_j_l;
  uint8 tmp_j_r;//√
  uint8 kuandu_sp_tmp;
  for( tmp_n=(last_n-1) ; tmp_n>=0 ; tmp_n=tmp_n-2 )  //本次n值
  {
    tmp_i   = bj_left[tmp_n].i;                      //上次i值 公共
    tmp_j_l = bj_left[tmp_n].j;                      //本次j值 右侧
    tmp_j_r = f_Xedge_sp_czbjqybj(img_wbx[tmp_i],tmp_j_l);//本次j值 左侧
    kuandu_sp_tmp = tmp_j_r - tmp_j_l + 1;            //本次水平宽度
    
    if( (kuandu_sp_tmp-kuandu_sp_last)>7 )
    {
      sgd_r.i=last_i;
      sgd_r.j=last_j_r;
//      sgd_flg_r=1;
      break;
    }
    
    last_i  = tmp_i;               //保存i值 公共
    last_j_r = tmp_j_r;            //保存j值 左侧
    kuandu_sp_last = kuandu_sp_tmp;//保存赛道宽度
  }
  /*-----------找环形上左拐点----结束-----------*/
  
  
  
  
  
  
  //一旦切换为出环形补线程序 立即暂停更新赛道类型 赛道类型将保持为出环形
  if( enable_gbsdlx==1 )
  {
    enable_gbsdlx=0;
  }
  //如果找到了环形的上右拐点 分一下两种情况：
  if( sgd_flag_l==1 )
  {
    //情况1：上右拐点不够低时 只是粗略地出环形补边界线
    if( (sgd_l.i)<=35 )//0~35
    {
      zb_imgarr_t I_tmp;
      I_tmp.i=bj_left[sgd_l_value_n+8].i;
      I_tmp.j=f_Xedge_sp_czbjqybj(img_wbx[bj_left[sgd_l_value_n+4].i],bj_left[sgd_l_value_n+4].j);
      
      bj_right[0].j=bj_left[0].j+90;
      liang_dian_lian_xian(img_wbx,0,I_tmp,bj_right[0]);
    }
    //情况2：上右拐点足够低时 则认为马上要出环形了 开始计步出环形 开始精确地出环形补边界线
    else//36~79
    {
      if( (sgd_l.i)<=60 )//36~60 比较低
      {
        if( flg_msychx_l==0 )
        {
          flg_msychx_l=1;
        }
      }
      else//61~79 相当低了
      {
        if(flg_msychx_l==1)
        {
          flg_msychx_l=2;
          EncoderCounter_L=0;
          EncoderCounter_R=0;
        }
      }
    }
  }
  if( (flg_msychx_l==1)||(flg_msychx_l==2) )//精确地出环形补边界线
  {
    bj_right[0].j=bj_left[0].j+90;
    liang_dian_lian_xian(img_wbx,0,sgd_r,bj_right[0]);
  }
  //当出环形计步完成后 恢复更新赛道类型
  if(  (flg_msychx_l==2)&&( (EncoderCounter_L+EncoderCounter_R)>20000)  )//对应实际距离1.5m左右
  {
    flg_msychx_l=0;
    enable_gbsdlx=1;
  }
  
  //补出赛道中线
  bxcx_wandao(img_wbx,79,15);
}