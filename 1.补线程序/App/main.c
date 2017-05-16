/*----------------------------------2017 东秦光电直立一队软件----------------------------------*/
/*------------------------------------by:YangBing&SuChenHui------------------------------------*/
#include "common.h"
#include "include.h"

uint8 YuZhi;                                  //二值化阈值
uint8 image_gray[IMG_H_YS][IMG_W_YS];         //原始灰度图像         高:120 宽:188
uint8 image_binarized[IMG_H_USED][IMG_W_USED];//抽取及二值化后的图像 高:80  宽:188

uint8         enable_gbsdlx = 1;          //改变赛道类型允许位 默认为1 即允许
SaiDao_type_m SaiDaoLeiXing_Last = ZhiDao;//上一帧图像赛道类型
SaiDao_type_m SaiDaoLeiXing;              //本帧图像赛道类型
extern uint8  flg_msycsz;                 //马上要出十字标志位

uint8 QianZhan = 50;                      //提取赛道中线的所用的前瞻
uint8 midline_value = IMG_TARGET_POSITION;//默认值给为IMG_TARGET_POSITION 方向环PID首次输出为0
uint8 midline_value_last = IMG_TARGET_POSITION;

float car_speed=280;//目标车速




/*****其他c文件的全局变量*****/
extern float angle_clhd;//Car_Control.c
extern float angle_clhd;//Car_Control.c
extern int16 pwm_common;//Car_Control.c
extern float speed_average;//Car_Control.c
extern float speed_left;//Car_Control.c
extern float speed_right;//Car_Control.c
extern int16 pwm_speed_now;//Car_Control.c
extern int16 pwm_dir;//Car_Control.c

extern uint8 bmkgflg_ctrspeed;   //控速开关
extern uint8 bmkgflg_lcd_image;  //LCD显示图像开关
extern uint8 bmkgflg_lcd_message;//LCD显示信息开关
extern uint8 bmkgflg_uart_message;
/*****************************/

void main()
{
  //拨码开关初始化
  bmkg_init();
  bmkg_readIO();
  //LCD初始化
  LCD_init();
  Site_t imgsite={0,0};
  Size_t imgsize={IMG_W_USED,IMG_H_USED};
  //Size_t imgsize_ys={IMG_W_YS,IMG_H_YS};
  Size_t lcdsize;
  lcdsize.H = LCD_H;//实测:128
  lcdsize.W = LCD_W;//实测:128
  //摄像头初始化
  mt9v032_camera_init();
  //摄像头中断服务函数配置
  void PORTA_IRQHandler();
  void DMA0_IRQHandler();
  set_vector_handler(PORTA_VECTORn,PORTA_IRQHandler);//设置 PORTA 的中断服务函数为 PORTA_IRQHandler
  set_vector_handler(DMA0_VECTORn,DMA0_IRQHandler);  //设置 DMA0 的中断服务函数为 PORTA_IRQHandler
  //直立控制初始化
  Car_Control_Init();
  //直立定时器中断函数配置
  void PIT0_ISR();
  pit_init_ms(PIT0,10);                     //初始化 PIT0 定时时间 10ms
  set_vector_handler(PIT0_VECTORn,PIT0_ISR);//设置 PIT0 的中断服务函数为 PIT0_ISR
  //中断嵌套、优先级配置
  #define   NVIC_PriorityGroup_0    ((uint32)0x7)
  #define   NVIC_PriorityGroup_1    ((uint32)0x6)
  #define   NVIC_PriorityGroup_2    ((uint32)0x5)
  #define   NVIC_PriorityGroup_3    ((uint32)0x4)
  #define   NVIC_PriorityGroup_4    ((uint32)0x3)
  NVIC_SetPriorityGrouping( NVIC_PriorityGroup_1 );//1bit抢占优先级 3bit响应优先级
  NVIC_SetPriority( PORTA_IRQn , 0);//场中断------>抢占优先级：0  响应优先级：0
  NVIC_SetPriorityGrouping( NVIC_PriorityGroup_1 );//1bit抢占优先级 3bit响应优先级
  NVIC_SetPriority( DMA0_IRQn  , 0);//DMA中断----->抢占优先级：0  响应优先级：0
  NVIC_SetPriorityGrouping( NVIC_PriorityGroup_1 );//1bit抢占优先级 3bit响应优先级
  NVIC_SetPriority( PIT0_IRQn  , 8);//定时器中断-->抢占优先级：1  响应优先级：0
  #undef    NVIC_PriorityGroup_0
  #undef    NVIC_PriorityGroup_1
  #undef    NVIC_PriorityGroup_2
  #undef    NVIC_PriorityGroup_3
  #undef    NVIC_PriorityGroup_4
  //采集一幅图像先裁剪后求阈值
  mt9v032_get_img();
  CaiJian( image_gray,image_binarized );            //原始尺寸灰度图像   -> 裁剪后尺寸灰度图像
  YuZhi = OTSU_FRM_USEDSIEZ_GRAY( image_binarized );//裁剪后尺寸灰度图像 -> 阈值
  //使能直立PIT0中断(10ms)
  enable_irq(PIT0_IRQn);
  /*------------------------------------分--------割--------线-----------------------------------------*/
  while(1)
  {
    //测大循环时间开始
    pit_time_start(PIT1);
    
    //读取拨码开关IO电平
    //bmkg_readIO();
    
    //超速保护
    Over_Speed_Protect( (uint16)(car_speed*2.0) );
    
    //采图像
    mt9v032_get_img();
    
    //图像裁剪 二值化
    CaiJian_ErZhi(image_gray,YuZhi,image_binarized);
    
    /***************提取中线算法开始***************/
    if( enable_gbsdlx==1 ) SaiDaoLeiXing = pdsdlx(image_binarized);
    if( (SaiDaoLeiXing==ShiZi)&&(SaiDaoLeiXing!=SaiDaoLeiXing_Last) ) flg_msycsz=0;
    switch( SaiDaoLeiXing )
    {
      case ZhiDao      : wandao_bxcx(image_binarized,79,10);break;
      case WanDao      : wandao_bxcx(image_binarized,79,10);break;
      case ShiZi       : shizi_bxcx(image_binarized);break;
      case RuHuanXing  : huanxing_r_ru_bxcx(image_binarized);break;//入环形补线程序
      case ChuHuanXing : huanxing_r_chu_bxcx(image_binarized);break;//入环形补线程序
      default          : wandao_bxcx(image_binarized,79,10);
    }
    SaiDaoLeiXing_Last = SaiDaoLeiXing;
    /***************提取中线算法结束***************/
    
//    zhidao_bxcx(image_binarized,79,10);
//    wandao_bxcx(image_binarized,79,10);
//    shizi_bxcx(image_binarized);
//    huanxing_r_ru_bxcx(image_binarized);
//    huanxing_r_chu_bxcx(image_binarized);
    
    //从补线后的图像中扫描中线值 用于打方向
    // f__midline_frm_line(image_binarized[QianZhan],1,&midline_value);
    uint8 midline_tmp_ps2;
    uint8 midline_tmp_ps1;
    uint8 midline_tmp;
    uint8 midline_tmp_px1;
    uint8 midline_tmp_px2;
    midline_tmp_ps2 = f__midline_frm_img(image_binarized,QianZhan-5,1);
    midline_tmp_ps1 = f__midline_frm_img(image_binarized,QianZhan-3,1);
    midline_tmp     = f__midline_frm_img(image_binarized,QianZhan  ,1);
    midline_tmp_px1 = f__midline_frm_img(image_binarized,QianZhan+3,1);
    midline_tmp_px2 = f__midline_frm_img(image_binarized,QianZhan+5,1);
    midline_value = (midline_tmp_ps2+midline_tmp_ps1+midline_tmp+midline_tmp_px1+midline_tmp_px2)/5;    
    
    //输出图像
    if( bmkgflg_lcd_image==1 )
    {
      LCD_Img_gray_Z(imgsite,lcdsize,(uint8*)image_binarized,imgsize);//LCD显示灰度图像
    }
    //LCD_Img_gray_Z(imgsite,lcdsize,(uint8*)image_gray,imgsize_ys);  //LCD显示灰度图像
    //vcan_sendimg(image_binarized,IMG_H_USED*IMG_W_USED);            //山外上位机发送灰度图像
    //seekfree_sendimg_zzf(image_binarized,IMG_H_USED*IMG_W_USED);    //逐飞上位机发送灰度图像
    
    //画 前瞻指示线 中线指示线
    Site_t site_czx;
    Site_t site_spx;
    site_czx.x=(uint8)(  ( (float)midline_value )/188.0*128.0  );//垂直线
    site_spx.y=(uint8)(  ( (float)QianZhan      )/80.0 *128.0  );//水平线
    for(uint8 i_hx=0;i_hx<128;i_hx++)
    {
      site_czx.y=i_hx;
      LCD_point(site_czx,RED);//垂直线
      site_spx.x=i_hx;
      LCD_point(site_spx,RED);//水平线
    }
    
//    //画出最中间两列线（93 94）
//    Site_t site_zzjczx_l;//最中间垂直线l 93
//    Site_t site_zzjczx_r;//最中间垂直线r 94
//    site_zzjczx_l.x=(uint8)( 93.0/188.0*128.0 );//画垂直线中线l 93
//    site_zzjczx_r.x=(uint8)( 94.0/188.0*128.0 );//画垂直线中线r 94
//    for(uint8 i_hx=0;i_hx<128;i_hx++)
//    {
//      site_zzjczx_l.y=i_hx;
//      LCD_point(site_zzjczx_l,GREEN);//画垂直线中线l 93
//      site_zzjczx_r.y=i_hx;
//      LCD_point(site_zzjczx_r,GREEN);//画垂直线中线r 94
//    }
    
    //LCD显示相关数据
    if( bmkgflg_lcd_message==1 )
    {
      Site_t site_lcd_display;
      site_lcd_display.y=0+0;//angle_clhd
      site_lcd_display.x=0;
      LCD_str(site_lcd_display,itoa((int32)angle_clhd),BLACK,WHITE);
      site_lcd_display.y=0+15;//直立pwm
      site_lcd_display.x=0;
      LCD_str(site_lcd_display,itoa((int32)pwm_common),BLACK,WHITE);
      site_lcd_display.y=35+0;//编码器测得速度
      site_lcd_display.x=0;
      LCD_str(site_lcd_display,ftoa(speed_average,1),BLACK,WHITE);
      site_lcd_display.y=35+15;//速度pwm
      site_lcd_display.x=0;
      LCD_str(site_lcd_display,itoa((int32)pwm_speed_now),BLACK,WHITE);
      site_lcd_display.y=70+0;//中线值
      site_lcd_display.x=0;
      LCD_str(site_lcd_display,itoa((int32)midline_value),BLACK,WHITE);
      site_lcd_display.y=70+15;//方向pwm
      site_lcd_display.x=0;
      LCD_str(site_lcd_display,itoa((int32)pwm_dir),BLACK,WHITE);
      site_lcd_display.y=105+0;//阈值
      site_lcd_display.x=0;
      LCD_str(site_lcd_display,itoa((int32)YuZhi),BLACK,WHITE);
    }
    
    //串口发送相关信息
    if( bmkgflg_uart_message==1 )
    {
//      printf("ps:%s\t",itoa((int32)midline_value));
//      printf("dir:%s\t",itoa((int32)pwm_dir));
//      extern float dir_pid_d_out;
//      extern float dir_pid_p_out;
//      printf("p:%s\t",ftoa(dir_pid_p_out,1));
//      printf("d:%s\n",ftoa(dir_pid_d_out,1));
      
      
      
    }
    printf("en:%d\t",enable_gbsdlx);
      printf("lx:%d\n",SaiDaoLeiXing);
    //测大循环时间结束
    Site_t site_lcd_looptime;
    site_lcd_looptime.y=0+0;//主循环时间
    site_lcd_looptime.x=112;
    LCD_str(site_lcd_looptime,itoa((int32)(pit_time_get_us(PIT1)/1000)),BLACK,WHITE);
  }
}
/**********************************************************************************************
                                   定时器中断函数(10ms)
**********************************************************************************************/
void PIT0_ISR()
{
  //清中断标志位
  PIT_Flag_Clear(PIT0);
  
  //功能代码部分
  static float tmp_speed;
  static uint16 TimeCount_ysfc=0;//用于延时发车 uint16型 0~65535 足够比赛用了 不用担心溢出
  TimeCount_ysfc++;
  if( TimeCount_ysfc<=50 )
  {
    tmp_speed=0;
  }
  else
  {
    tmp_speed=car_speed;
  }
  Car_Control(midline_value,IMG_TARGET_POSITION,tmp_speed);//MeasuredDir TargetDir TargetSpeed
}
/**********************************************************************************************
                    摄像头 PORTA场中断服务函数 及 DMA0完成中断服务函数
**********************************************************************************************/
void PORTA_IRQHandler()//PTA29 场中断
{
  uint8  n;//引脚号
  uint32 flag;
  
  while(!PORTA_ISFR);
  flag = PORTA_ISFR;
  PORTA_ISFR  = ~0;                                   //清中断标志位
  
  n = 29;                                             //场中断
  if(flag & (1 << n))                                 //PTA29触发中断
  {
    VSYNC();
  }
}
void DMA0_IRQHandler()//PTA27像素中断
{
  DMA_IRQ_CLEAN(DMA_CH0);
  row_finished();
}