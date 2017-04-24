#include "common.h"
#include "_small_func_.h"
#include "include.h"
#include "_panduan_sdlx_.h"

zb_imgarr_t l_end_img,r_end_img;        //定义最后一行坐标与上一帧最后一行
zb_imgarr_t l_end_last_img,r_end_last_img;    

#define Hang_3    50     //        打方向
#define Hang_1    70     //         求斜率
#define Hang_end  79     //        最后一行，判断十字

uint8 f_l_1;//第 1 行
uint8 left_1;
uint8 f_r_1;
uint8 right_1;


uint8 f_l_3;//第 3 行
uint8 left_3;
uint8 f_r_3;
uint8 right_3;


uint8 f_l_end;//最后一行
uint8 left_end;
uint8 f_r_end;
uint8 right_end;

/*-----------------赛道类型判断函数---------------*/
SaiDao_type_m  pdsdkx(uint8 img_bin[][IMG_W_USED])
{
  
  SaiDao_type_m sdlx_return;    //定义返回的赛道类型
  
  zb_imgarr_t l_1_img,r_1_img;            //定义图像坐标
  zb_imgarr_t l_3_img,r_3_img;
  
  zb_math_t l_1_math,r_1_math;            //定义数学坐标
  zb_math_t l_3_math,r_3_math;
  
  slope_t K_l,K_r;                        //定义左右斜率
  
  uint8 kuandu_last;                      //定义上一次最后一行宽度数据
  uint8 kuandu;                           //定义最后一行宽度数据
  
  static uint8 leixing_flag=1;               //定义判断类型标志位

  
  f__edge_sp_czj(img_bin[Hang_1],93,&f_l_1,&left_1,&f_r_1,&right_1);//水平扫第一行
  f__edge_sp_czj(img_bin[Hang_3],93,&f_l_3,&left_3,&f_r_3,&right_3);//水平扫第三行
  f__edge_sp_czj(img_bin[Hang_end],93,&f_l_end,&left_end,&f_r_end,&right_end);//水平扫最后一行
     
      

      
  l_1_img.i=Hang_1;     //  左1图像坐标赋值            //给图像坐标赋值 
  l_1_img.j=left_1;
  r_1_img.i=Hang_1;     //  右1图像坐标赋值
  r_1_img.j=right_1;
  
  l_3_img.i=Hang_3;     //  左3图像坐标赋值
  l_3_img.j=left_3;
  r_3_img.i=Hang_3;     //  右3图像坐标赋值
  r_3_img.j=right_3;


  //两个斜率图像画线
  liang_dian_lian_xian(img_bin,125,l_1_img,l_3_img);  
  liang_dian_lian_xian(img_bin,125,r_1_img,r_3_img);
  liang_dian_lian_xian(img_bin,125,l_1_img,r_1_img);
  liang_dian_lian_xian(img_bin,125,l_3_img,r_3_img);
      
  l_end_img.i=Hang_end;   //最后一行图像坐标赋值
  l_end_img.j=left_end;
  r_end_img.i=Hang_end;
  r_end_img.j=right_end;      
      
  l_1_math=covert_zb(l_1_img);      //坐标转换用于计算斜率
  r_1_math=covert_zb(r_1_img);
  l_3_math=covert_zb(l_3_img);
  r_3_math=covert_zb(r_3_img);
      

  K_l=calcu_slope(l_1_math,l_3_math);    //计算左斜率
  K_r=calcu_slope(r_1_math,r_3_math);    //计算右斜率
  
//  Site_t P_xielv_l= {0,0};             //图像显示斜率
//  Site_t P_xielv_r= {104,0};
//  LCD_str(P_xielv_l,itoa((int32)K_l.value),BLACK,WHITE);
//  LCD_str(P_xielv_r,itoa((int32)K_r.value),BLACK,WHITE);
  
  gpio_init (PTD15,GPO,1);      //指示赛道类型的灯      D15为直道和弯道
  gpio_init (PTE26,GPO,1);      //                      E26为十字
  
  printf("n0.val=%d",(int8)(K_l.value*10));
  UART_End();
  printf("n1.val=%d",(int8)(K_r.value*10));
  UART_End();
  //printf("hello");
  
  if( (K_l.exist==1)&&(K_r.exist==1) )
  {
    if(leixing_flag==1)
    {
      //判断直道和弯道 
      if( (K_l.value>0)&&(K_r.value<0))
      {
        gpio_set(PTD15, 0);
        sdlx_return=ZhiDao;

      }
      if(((K_l.value>0)&&(K_r.value>0))||((K_l.value<0)&&(K_r.value<0)))
      {
        gpio_set(PTD15, 0);
        sdlx_return=WanDao;          
      }
    }
    if( (leixing_flag==1)||(leixing_flag==2) )
    {
      //判断十字
      if(((K_l.value<0)&&(K_r.value>0))||(leixing_flag==2))
      {
        leixing_flag=2;
        gpio_set(PTE26,0); 
        
        //十字补线程序

        kuandu_last=r_end_last_img.j-l_end_last_img.j;
        kuandu=r_end_img.j-l_end_img.j;
        if((kuandu_last>100)&&(kuandu<100))
        {
          leixing_flag=1;
        }
        
      }
    }
    
  }

      

  l_end_last_img=l_end_img;     //保留最后一行上一帧的跳变沿坐标
  r_end_last_img=r_end_img;

  return sdlx_return;
}