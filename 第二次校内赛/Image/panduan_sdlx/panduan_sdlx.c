#include "common.h"
#include "_small_func_.h"
#include  "include.h"
#include "panduan_sdlx.h"
#include "lcd_draw_line.h"
#include "math.h"


void UART_End();                                                //声明串口结束函数    
n_edge_t Start_End(uint8 line[],uint8 start,uint8 final,edge_t edge[]);                //声明起跑线扫描函数
uint8  f__edge_cz_ry(uint8 line[][IMG_W_USED],uint8 row,uint8 start_i,uint8 final_i); //声明垂直扫函数
uint8 huanxing_shizi(uint8 img_bin[][IMG_W_USED],uint8 bx_start_i,uint8 bx_range);     //声明环形弯道识别程序
uint8 zhidao_wandao(uint8 img_bin[][IMG_W_USED],uint8 bx_start_i,uint8 bx_range);       //声明直道弯道识别程序

zb_imgarr_t l_end_img,r_end_img;        //定义最后一行坐标与上一帧最后一行
zb_imgarr_t l_end_last_img,r_end_last_img;    

#define Hang_2    43     //        打方向
#define Hang_startline 53    //        起跑线
#define Hang_1    75     //        求斜率
#define Hang_end  79     //        最后一行，判断十字
#define Hang_6    32     //用于判断起跑线
#define Hang_5    35     //前障碍线
#define Hang_4    40     //后障碍线


#define wandao          ((K_l_12.value>=0)&&(K_r_12.value>=0))||((K_l_12.value<0)&&(K_r_12.value<0))
#define zhidao          (K_l_12.value>0)&&(K_r_12.value<0)

extern int64 EncoderCounter_L;
extern int64 EncoderCounter_R;

uint8 f_l_end;//底部一行
uint8 left_end;
uint8 f_r_end;
uint8 right_end;


uint8 f_l_1;//第 1 行
uint8 left_1;
uint8 f_r_1;
uint8 right_1;

uint8 f_l_startline;//起跑线
uint8 left_startline;
uint8 f_r_startline;
uint8 right_startline;

uint8 f_l_2;//第 2 行
uint8 left_2;
uint8 f_r_2;
uint8 right_2;

uint8 f_l_4;//第 4 行
uint8 left_4;
uint8 f_r_4;
uint8 right_4;

uint8 f_l_5;//第 5 行
uint8 left_5;
uint8 f_r_5;
uint8 right_5;

uint8 f_l_6;//第 6 行
uint8 left_6;
uint8 f_r_6;
uint8 right_6;

/*-----------------赛道类型判断函数---------------*/
SaiDao_type_m  pdsdlx(uint8 img_bin[][IMG_W_USED])
{
  
  SaiDao_type_m sdlx_return;    //定义返回的赛道类型
  
  zb_imgarr_t l_1_img,r_1_img;            //定义第一行，第二行左右跳变沿图像坐标
  zb_imgarr_t l_2_img,r_2_img;
  zb_imgarr_t l_startline_img,r_startline_img;  // 定义起跑线扫描线图像坐标(50行) 
  zb_imgarr_t l_4_img,r_4_img;          //障碍
  zb_imgarr_t l_5_img,r_5_img;          //障碍判断线
  zb_imgarr_t l_6_img,r_6_img;          //起跑线

  
  
  zb_math_t l_1_math,r_1_math;            //定义数学坐标
  zb_math_t l_2_math,r_2_math;

  slope_t K_l_12,K_r_12;                        //定义1、2行左右斜率
  
  uint8 kuandu_last;                      //定义上一次最后一行宽度数据
  uint8 kuandu;                           //定义最后一行宽度数据
  uint8 startline_kuandu;                     //定义起跑线宽度
  float zhangai_kuandushang;                  //障碍上行宽度
  float zhangai_kuanduxia;                    //障碍下行宽度
  float zhangai_kuandu;                       //障碍宽度比
  int8 left_cha;                              //障碍前后判断
  int8 right_cha;
  
  
  
  static uint8 dibu_last=0;                       //定义底部是否丢边数据
  static uint8 dibu=0;                            //上一帧丢边而这一帧不丢边则切换为出环形程序
  
  static uint8 leixing_flag=0;                  //定义判断类型标志位
         uint8 shizi_huanxing_flag=0;           //定义十字环形标志位，非静态变量
         uint8 zhidaowandao_flag=0;
  
         


  f__edge_sp_czj_plus(img_bin[Hang_1],93,&f_l_1,&left_1,&f_r_1,&right_1);//水平扫第一行
  f__edge_sp_czj_plus(img_bin[Hang_2],93-47,&f_l_2,&left_2,&f_r_2,&right_2);//水平扫第二行
  f__edge_sp_czj(img_bin[Hang_end],93-20,&f_l_end,&left_end,&f_r_end,&right_end);//水平扫最后一行
  f__edge_sp_czj(img_bin[Hang_startline],93-30,&f_l_startline,&left_startline,&f_r_startline,&right_startline);
  //水平扫描起跑线，第50行
  f__edge_sp_czj(img_bin[Hang_4],93-30,&f_l_4,&left_4,&f_r_4,&right_4);   //水平扫第四行
  f__edge_sp_czj(img_bin[Hang_5],93-30,&f_l_5,&left_5,&f_r_5,&right_5);   //水平扫第五行
  f__edge_sp_czj(img_bin[Hang_6],93-30,&f_l_6,&left_6,&f_r_6,&right_6);   //水平扫第六行
  
  
  l_1_img.i=Hang_1;     //  左1图像坐标赋值            //给图像坐标赋值 
  l_1_img.j=left_1;
  r_1_img.i=Hang_1;     //  右1图像坐标赋值
  r_1_img.j=right_1;
   
  l_2_img.i=Hang_2;     //  左2图像坐标赋值
  l_2_img.j=left_2;
  r_2_img.i=Hang_2;     //  右2图像坐标赋值
  r_2_img.j=right_2;
  


  l_startline_img.i=Hang_startline;     //  左起跑线(50行)图像坐标赋值
  l_startline_img.j=left_startline;
  r_startline_img.i=Hang_startline;     //  右起跑线(50行)图像坐标赋值
  r_startline_img.j=right_startline;

  l_end_img.i=Hang_end;   //最后一行图像坐标赋值
  l_end_img.j=left_end;
  r_end_img.i=Hang_end;
  r_end_img.j=right_end;      
  
  l_4_img.i=Hang_4;     //  左4图像坐标赋值            //给图像坐标赋值 
  l_4_img.j=left_4;
  r_4_img.i=Hang_4;     //  右4图像坐标赋值
  r_4_img.j=right_4;
  
  l_5_img.i=Hang_5;     //  左5图像坐标赋值            //给图像坐标赋值 
  l_5_img.j=left_5;
  r_5_img.i=Hang_5;     //  右5图像坐标赋值
  r_5_img.j=right_5;
      
  l_6_img.i=Hang_6;     //  左6图像坐标赋值            //给图像坐标赋值 
  l_6_img.j=left_6;
  r_6_img.i=Hang_6;     //  右6图像坐标赋值
  r_6_img.j=right_6;
  
  l_1_math=covert_zb(l_1_img);      //第一行，第二行左右跳变沿坐标转换用于计算斜率
  r_1_math=covert_zb(r_1_img);          
  l_2_math=covert_zb(l_2_img);
  r_2_math=covert_zb(r_2_img);


  K_l_12=calcu_slope(l_1_math,l_2_math);    //计算1_2左斜率
  K_r_12=calcu_slope(r_1_math,r_2_math);    //计算1_2右斜率
  

  
  
  gpio_init (PTD15,GPO,1);      //指示赛道类型的灯      D15为直道和弯道
  gpio_init (PTE26,GPO,1);      //                      E26为十字，环形

  
/*--------------------连线函数-----------------------*/
  //两个斜率图像连线
//  liang_dian_lian_xian(img_bin,125,l_1_img,l_2_img);  
//  liang_dian_lian_xian(img_bin,125,r_1_img,r_2_img);
//  liang_dian_lian_xian(img_bin,125,l_4_img,r_4_img);
//  liang_dian_lian_xian(img_bin,125,l_5_img,r_5_img);
//  liang_dian_lian_xian(img_bin,125,l_startline_img,r_startline_img);
///*---------------------------------------------------------  */
//  
///*-------------------串口屏显示------------------------*/
//  printf("n2.val=%d",f_l_end);     //左斜率
//  UART_End();
//  
//  printf("n3.val=%d",f_l_end);     //右斜率
//  UART_End();
//  
//  printf("n2.val=%d",f_l_2);
//  UART_End();
//  
//  printf("n3.val=%d",f_r_2);
//  UART_End();
////  //printf("hello");
/*--------------------计算底行宽度判断跳出条件------------------------------*/  

  kuandu_last=r_end_last_img.j-l_end_last_img.j;               //计算上一帧最后一行宽度
  kuandu=r_end_img.j-l_end_img.j;                              //和现在这一帧最后一行宽度之差
  dibu=(f_l_end)&&(f_r_end);                                   //保存这一帧图像左边是否丢边
  startline_kuandu=r_startline_img.j-l_startline_img.j;        //保存扫描这一行的宽度
  zhangai_kuandushang=r_5_img.j-l_5_img.j;                     //障碍上一行宽度
  zhangai_kuanduxia=r_4_img.j-l_4_img.j;                       //障碍下一行宽度 
  zhangai_kuandu=(float)(zhangai_kuanduxia/zhangai_kuandushang);   //障碍宽度比
/*---------------------------判断赛道类型-----------------------------------*/  
    
      //起跑线
//    if((startline_kuandu<=20)&&(leixing_flag==0))
//    {
//        sdlx_return=BanMaXian;
//    }

     zhidaowandao_flag=zhidao_wandao(img_bin,79,10);          //直道弯道判断标志位
     shizi_huanxing_flag=huanxing_shizi(img_bin,79,15);       //十字环形判断标志位置位
     
    //直道弯道
      if(((f_l_2==1)||(f_r_2==1))&&(leixing_flag==0))
      {

          //判断为直道 
          if((zhidao)&&(zhidaowandao_flag==1))   
          {
            sdlx_return=WanDao;
            if(zhangai_kuandu>1.5)          //识别为障碍或者起跑线
            {
              if((r_6_img.j-l_6_img.j)<10)     //识别为起跑线
              {
                
                  gpio_set(PTD15, 0);
                  sdlx_return=WanDao;
                  
                  extern uint8 enable_findbmx;
                  if(enable_findbmx==1) Stop_Stand();
              }
              else
              {
                left_cha=l_5_img.j-l_4_img.j;
                right_cha=r_4_img.j-r_5_img.j;
                if((abs(left_cha-right_cha))>10)
                {
                  gpio_set(PTE26,0);              //识别为障碍
                  sdlx_return=ZhiDao;
                }
                else
                {
                  sdlx_return=WanDao;
                }
              }
            }
            
          }
          
          //判断为弯道 
          else
          {
            sdlx_return=WanDao;  
          }

      }
      //判断为十字
      if(((f_l_2==0)&&(f_r_2==0)&&(shizi_huanxing_flag==0)&&(leixing_flag!=2))||(leixing_flag==1))
      {
              leixing_flag=1;  				//总类型置1可保持十字
              //进入十字条件且能保持
              sdlx_return=ShiZi;
                  
              //判断出十字
              if((kuandu_last>105)&&(kuandu<105))
              {
                leixing_flag=0;        
              }  

      }
      
              
      
      //进入环形赛道
      if(((shizi_huanxing_flag==1)&&(leixing_flag!=1))||(leixing_flag==2))
      {
          leixing_flag=2;              //赛道类型标志位置2，保持环形
          sdlx_return=RuHuanXing; 
          
          //判断入环形是否结束               
          if((dibu==1)&&(dibu_last==0))
          {

                  leixing_flag=0;
                  sdlx_return=ChuHuanXing;			//满足入环形结束程序后，切换出环形程序
          }
          
      }

  l_end_last_img=l_end_img;     //保留最后一行上一帧的跳变沿坐标
  r_end_last_img=r_end_img;
  dibu_last=dibu;            //保留最后底部左边是否丢边情况
  return sdlx_return;
  
}

/**********************************************************************************************
名    称：             

功能描述：        从底部向上垂直,start_i到final_i是否为全为黑色，
入口参数： 
出口参数： 

备    注： 全为黑色则flag置1，否则flag置0
**********************************************************************************************/
uint8 f__edge_cz_ry(uint8 line[][IMG_W_USED],uint8 row,uint8 start_i,uint8 final_i)
{
  //从中间底部向上
  uint8 flag=1;
  for( uint8 Ci=start_i ; Ci>=final_i ; Ci-- )
  {
    //从第 93(94也可以) 列处 由下往上 垂直扫描跳变沿
    if( line[Ci][row]==255)
    {
         flag=0;
         break;
    }

  }
  return flag;
}
/**********************************************************************************************
名    称：                           环形十字识别程序

功能描述： 
入口参数： 
出口参数： 

备    注： 无特殊要求时给形参bx_start_i传入79即可
**********************************************************************************************/
uint8 huanxing_shizi(uint8 img_bin[][IMG_W_USED],uint8 bx_start_i,uint8 bx_range)
{
  int16 Ci;
  
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
  uint8 shihuan_flag=0;
  

  
  
  //先从最下面5行找边界 并求取平均值
  start_bj_j_left=0;
  start_bj_j_right=0;
  for( Ci=bx_start_i ; Ci>=(bx_start_i-4); Ci-- )//假如bx_start_i==79 则是79 78 77 76 75 共计5行
  {
    //需要注意 程序默认最底部5行都是可以找到边界点的
    f__edge_sp_czj(img_bin[Ci],93,&nomeaning_l,&(bj_left[n_bj_l].j),&nomeaning_r,&(bj_right[n_bj_r].j));
    bj_left[n_bj_l].i  = Ci;
    bj_right[n_bj_r].i = Ci;
    start_bj_j_left   += bj_left[n_bj_l].j;
    start_bj_j_right  += bj_right[n_bj_r].j;
    
    n_bj_l++;
    n_bj_r++;
  }
  start_bj_j_left /=5;//将平均值作为以后的起始边界j值
  start_bj_j_right/=5;//同上
  
  //前5行扫完后 往后每行在上一行的左右边界附近查找本行边界 以节省时间 同时找的更远
  left_last  = start_bj_j_left;
  right_last = start_bj_j_right;
  for( Ci=(bx_start_i-5) ; Ci>=0 ; Ci-- )
  {
    if(over_flag_l==0)
    {
      if( f_Sedge_sp_cmd(img_bin[Ci],left_last,bx_range,&left_now)==1 )
      {
        bj_left[n_bj_l].i=Ci;
        bj_left[n_bj_l].j=left_now;
        
        left_last=left_now;
        n_bj_l++;
      }
      else
      {
        over_flag_l=1;
      }
    }
    
    if(over_flag_r==0)
    {
      if( f_Xedge_sp_cmd(img_bin[Ci],right_last,bx_range,&right_now)==1 )
      {
        bj_right[n_bj_r].i=Ci;
        bj_right[n_bj_r].j=right_now;
        
        right_last=right_now;
        n_bj_r++;
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
  
  //自拟代码，判断环形和十字左右两边点的个数
  #define n_shang (79-25)
  #define n_zhong (79-35)
  #define n_xia   (79-45)


  int8 left_shang=bj_left[n_shang].j-bj_left[n_zhong].j;
  int8 left_xia=bj_left[n_zhong].j-bj_left[n_xia].j;
  int8 right_shang=bj_right[n_shang].j-bj_right[n_zhong].j;
  int8 right_xia=bj_right[n_zhong].j-bj_right[n_xia].j;
  if( MIN(n_bj_l,n_bj_r)>60 )
  {
      if((left_shang<0)&&(left_xia>0)&&(right_shang>0)&&(right_xia<0))
      {
          shihuan_flag=1;
      }
  }
  
  return shihuan_flag;
}
/**************************************************************
 *
 * 功       能：LCD结束指令
 *
 **************************************************************/
void UART_End()
{
    uart_putchar (UART4,0xff);
    pit_delay(PIT2, 100);
    uart_putchar (UART4,0xff);
    pit_delay(PIT2, 100);
    uart_putchar (UART4,0xff);
    pit_delay(PIT2, 100);

}

/**********************************************************************************************
名    称：                           直道弯道识别程序

功能描述： 
入口参数： 
出口参数： 

备    注： 无特殊要求时给形参bx_start_i传入79即可
**********************************************************************************************/
uint8 zhidao_wandao(uint8 img_bin[][IMG_W_USED],uint8 bx_start_i,uint8 bx_range)
{
  int16 Ci;
  
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
  
  uint8 zhidaowandao_flag=0;      //定义标志位
  uint8 dqz_l=0,dqz_r=0;
  
  //先从最下面5行找边界 并求取平均值
  start_bj_j_left=0;
  start_bj_j_right=0;
  for( Ci=bx_start_i ; Ci>=(bx_start_i-4); Ci-- )//假如bx_start_i==79 则是79 78 77 76 75 共计5行
  {
    //需要注意 程序默认最底部5行都是可以找到边界点的
    f__edge_sp_czj(img_bin[Ci],93,&nomeaning_l,&(bj_left[n_bj_l].j),&nomeaning_r,&(bj_right[n_bj_r].j));
    bj_left[n_bj_l].i  = Ci;
    bj_right[n_bj_r].i = Ci;
    start_bj_j_left   += bj_left[n_bj_l].j;
    start_bj_j_right  += bj_right[n_bj_r].j;
    
    n_bj_l++;
    n_bj_r++;
  }
  start_bj_j_left /=5;//将平均值作为以后的起始边界j值
  start_bj_j_right/=5;//同上
  
  //前5行扫完后 往后每行在上一行的左右边界附近查找本行边界 以节省时间 同时找的更远
  left_last  = start_bj_j_left;
  right_last = start_bj_j_right;
  for( Ci=(bx_start_i-5) ; Ci>=0 ; Ci-- )
  {
    if(over_flag_l==0)
    {
      if( f_Sedge_sp_cmd(img_bin[Ci],left_last,bx_range,&left_now)==1 )
      {
        bj_left[n_bj_l].i=Ci;
        bj_left[n_bj_l].j=left_now;
        
        left_last=left_now;
        n_bj_l++;
      }
      else
      {
        over_flag_l=1;
      }
    }
    
    if(over_flag_r==0)
    {
      if( f_Xedge_sp_cmd(img_bin[Ci],right_last,bx_range,&right_now)==1 )
      {
        bj_right[n_bj_r].i=Ci;
        bj_right[n_bj_r].j=right_now;
        
        right_last=right_now;
        n_bj_r++;
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
  //自拟代码
  if((n_bj_l>=60)&&(n_bj_r>=60))
  {
    for(dqz_l=10;dqz_l<60;)
    {
      if(bj_left[dqz_l].j>bj_left[dqz_l+5].j)
      {
        break;
      }
      dqz_l=dqz_l+5;
    }
    
    for(dqz_r=10;dqz_r<60;)
    {
      if(bj_right[dqz_r].j<bj_right[dqz_r+5].j)
      {
        break;
      }
      dqz_r=dqz_r+5;
    }
    if((dqz_l>=60)&&(dqz_r>=60))
    {
      zhidaowandao_flag=1;
    }
    
  }
  
  
  return zhidaowandao_flag;
}

