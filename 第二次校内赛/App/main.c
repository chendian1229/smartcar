/*----------------------------------2017 东秦光电直立一队软件----------------------------------*/
/*------------------------------------by:YangBing&SuChenHui------------------------------------*/
#include "common.h"
#include "include.h"

uint8 YuZhi;                                  //二值化阈值
uint8 image_gray[IMG_H_YS][IMG_W_YS];         //原始灰度图像         高:120 宽:188
uint8 image_binarized[IMG_H_USED][IMG_W_USED];//抽取及二值化后的图像 高:80  宽:188

uint8 enable_findbmx=0;

uint8         enable_gbsdlx = 1;          //改变赛道类型允许位 默认为1 即允许
SaiDao_type_m SaiDaoLeiXing_Last = ZhiDao;//上一帧图像赛道类型
SaiDao_type_m SaiDaoLeiXing;              //本帧图像赛道类型
extern uint8  flg_msycsz;                 //马上要出十字标志位

uint8 QianZhan_base;
uint8 QianZhan_used;                      //提取赛道中线的所用的前瞻
uint8 midline_value = IMG_TARGET_POSITION;//默认值给为IMG_TARGET_POSITION 方向环PID首次输出为0

float K_midline_yjdt=0.8;
float mid_vlu_yjdt_f32_used;
float mid_vlu_yjdt_f32_last=IMG_TARGET_POSITION;


float car_speed_set;
float car_speed=0;//目标车速



/*****其他c文件的全局变量*****/
extern float angle_clhd;//Car_Control.c
extern float angle_clhd;//Car_Control.c
extern int16 pwm_common;//Car_Control.c
extern float speed_average;//Car_Control.c
extern float speed_left;//Car_Control.c
extern float speed_right;//Car_Control.c
extern int16 pwm_speed_now;//Car_Control.c
extern int16 pwm_dir;//Car_Control.c

extern uint8 bmkgflg_ctrspeed;    //控速开关
extern uint8 bmkgflg_lcd_image;   //LCD显示图像开关
extern uint8 bmkgflg_lcd_message; //LCD显示信息开关
extern uint8 bmkgflg_uart_message;//UART发送消息
extern uint8 bmkgflg_hxzy;        //环型左右选择
extern uint8 bmkgflg_csxz;        //参数选择 左侧低速挡 右侧高速挡

extern float Kp_an;//直立PD控制
extern float Ki_an;
extern float Kd_an;
extern float Kp_sp;//速度PI控制 可以用的参数
extern float Ki_sp;
extern float Kd_sp;
extern float Kp_dir;//方向PD控制
extern float Ki_dir;
extern float Kd_dir;
/*****************************/

void main()
{
  //拨码开关初始化
  bmkg_init();
  bmkg_read_IO();
  button_init();
  //LCD初始化
  LCD_init();
  LCD_init();
  Site_t lcd_display_site;
  Site_t imgsite={0,0};
  Size_t imgsize={IMG_W_USED,IMG_H_USED};
  Size_t lcdsize;
  lcdsize.H = LCD_H;//实测:128
  lcdsize.W = LCD_W;//实测:128
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
  //使能直立PIT0中断(10ms)
  enable_irq(PIT0_IRQn);
  //车躺着状态下 不断求角度 1秒后进行自动校正 然后LED指示校正完毕
  DELAY_MS(1000);
  extern float cali_angle;
  extern float angle_clhd;
  if( fabs(32.0-angle_clhd)>0.2 )
  {
    cali_angle=32.0-angle_clhd;
  }
  gpio_init(PTE26,GPO,0);gpio_init(PTD15,GPO,0);DELAY_MS(100);
  gpio_turn(PTE26);gpio_turn(PTD15);DELAY_MS(100);
  gpio_turn(PTE26);gpio_turn(PTD15);DELAY_MS(100);
  gpio_turn(PTE26);gpio_turn(PTD15);DELAY_MS(100);
  gpio_turn(PTE26);gpio_turn(PTD15);DELAY_MS(100);
  gpio_turn(PTE26);gpio_turn(PTD15);
  //LED指示完成后 LCD显示当前选择的高低速挡位
  if(bmkgflg_csxz==0)//拨左侧 低速挡 低低低低低低低低低低低低低低低低低低低低
  {
    car_speed_set=260;
    K_midline_yjdt=0.8;
    QianZhan_base=50;
    
    Kp_an=90;//直立PD控制
    Ki_an=0;
    Kd_an=220;
    Kp_sp=9;//速度PI控制 可以用的参数
    Ki_sp=0.5;
    Kd_sp=0;
    Kp_dir=7;//方向PD控制
    Ki_dir=0;
    Kd_dir=50;
    
    lcd_display_site.x=64;
    lcd_display_site.y=64;
    LCD_str(lcd_display_site,"L O W",FCOLOUR,BCOLOUR);
  }
  else// 1 右侧 高速挡 高高高高高高高高高高高高高高高高高高高高高高高
  {
    car_speed_set=280;
    K_midline_yjdt=0.7;
    QianZhan_base=50;
    
    Kp_an=90;//直立PD控制
    Ki_an=0;
    Kd_an=220;
    Kp_sp=10;//速度PI控制 可以用的参数
    Ki_sp=0.5;
    Kd_sp=0;
    Kp_dir=6.5;//方向PD控制
    Ki_dir=0;
    Kd_dir=50;
    
    lcd_display_site.x=64;
    lcd_display_site.y=64;
    LCD_str(lcd_display_site,"H I G H",FCOLOUR,BCOLOUR);
  }
  //手扶着车起来 使杆保持垂直 此时按下确定键 手动求一次阈值 LED指示求阈值完毕
  while(1)
  {
    if( gpio_get(PTB16)==0 ) break;//等待确认键按下
  }
  mt9v032_get_img();                                //采集一幅图像先裁剪后求阈值
  CaiJian( image_gray,image_binarized );            //原始尺寸灰度图像   -> 裁剪后尺寸灰度图像
  YuZhi=OTSU_FRM_USEDSIEZ_GRAY( image_binarized );  //裁剪后尺寸灰度图像 -> 阈值
  gpio_init(PTE26,GPO,0);gpio_init(PTD15,GPO,0);DELAY_MS(100);
  gpio_turn(PTE26);gpio_turn(PTD15);DELAY_MS(100);
  gpio_turn(PTE26);gpio_turn(PTD15);DELAY_MS(100);
  gpio_turn(PTE26);gpio_turn(PTD15);DELAY_MS(100);
  gpio_turn(PTE26);gpio_turn(PTD15);DELAY_MS(100);
  gpio_turn(PTE26);gpio_turn(PTD15);
  //求阈值指示完毕后 直立环 速度还 方向环全开
  extern uint8 en_AngPID_output;
  extern uint8 en_SpdPID_output;
  extern uint8 en_DirPID_output;
  en_AngPID_output=1;
  en_SpdPID_output=1;
  en_DirPID_output=1;
  extern float bias_integration_sp;//清速度环积分项
  bias_integration_sp=0;
  car_speed=car_speed_set;//给速度 车2s后开始逐渐加速
  
  /*------------------------------------分--------割--------线-----------------------------------------*/
  while(1)
  {
    //测大循环时间开始
    pit_time_start(PIT1);
    
    //读取拨码开关IO电平
    bmkg_read_IO();
    
    //超速保护
    Over_Speed_Protect( (uint16)(car_speed*2.0) );
    
    //采图像
    mt9v032_get_img();
    
    //图像裁剪 二值化
    CaiJian_ErZhi(image_gray,YuZhi,image_binarized);
    
    if( enable_gbsdlx==1 ) SaiDaoLeiXing = pdsdlx(image_binarized);
    if( (SaiDaoLeiXing==ShiZi)&&(SaiDaoLeiXing!=SaiDaoLeiXing_Last) ) flg_msycsz=0;
    if( bmkgflg_hxzy==1 )//看环走左侧还是右侧
    {
      switch( SaiDaoLeiXing )
      {
        case BanMaXian   : bxcx_banmaxian(image_binarized);break;
        case ChuHuanXing : bxcx_huanxing_chu_r(image_binarized);break;
        case RuHuanXing  : bxcx_huanxing_ru_r(image_binarized);break;
        case ShiZi       : bxcx_shizi(image_binarized);break;
        case WanDao      : bxcx_wandao(image_binarized,79,10);break;
        case ZhiDao      : bxcx_zhidao(image_binarized);break;
      }
    }
    else
    {
      switch( SaiDaoLeiXing )
      {
        case BanMaXian   : bxcx_banmaxian(image_binarized);break;
        case ChuHuanXing : bxcx_huanxing_chu_l(image_binarized);break;
        case RuHuanXing  : bxcx_huanxing_ru_l(image_binarized);break;
        case ShiZi       : bxcx_shizi(image_binarized);break;
        case WanDao      : bxcx_wandao(image_binarized,79,10);break;
        case ZhiDao      : bxcx_zhidao(image_binarized);break;
      }
    }
    SaiDaoLeiXing_Last = SaiDaoLeiXing;
    
//    bxcx_banmaxian(image_binarized);
//    bxcx_huanxing_chu_r(image_binarized);
//    bxcx_huanxing_ru_r(image_binarized);
//    bxcx_huanxing_chu_l(image_binarized);
//    bxcx_huanxing_ru_l(image_binarized);
//    bxcx_shizi(image_binarized);
//    bxcx_wandao(image_binarized,79,10);
//    bxcx_zhidao(image_binarized);
    

    extern float speed_left;          //左轮速度
    extern float speed_right;         //右轮速度
    extern float speed_average;       //左右轮平均速度
    
    float QianZhan_used_f32;
    
//    QianZhan_used_f32=52.0-(speed_average-280.0)*0.07;
    
    static float speed_chazhi_max=0;
    float speed_chazhi;
    speed_chazhi=fabs( (speed_left-speed_right) );
    if(speed_chazhi>speed_chazhi_max) speed_chazhi_max=speed_chazhi;
    
    QianZhan_used_f32=(float)QianZhan_base+speed_chazhi*0.085;//180*0.08=14
    
    if(QianZhan_used_f32>=65) QianZhan_used_f32=65;//低
    if(QianZhan_used_f32<=45) QianZhan_used_f32=45;//高
    QianZhan_used=(uint8)QianZhan_used_f32;
    
//    QianZhan_used=55;
    
    
    //从补线后的图像中扫描中线值 用于打方向
    // f__midline_frm_line(image_binarized[QianZhan_used],1,&midline_value);
    midline_value = f__midline_frm_img_plus(image_binarized,QianZhan_used,1);
    
    mid_vlu_yjdt_f32_used=(float)midline_value;//float <-- uint8
    mid_vlu_yjdt_f32_used=K_midline_yjdt*mid_vlu_yjdt_f32_used+(1-K_midline_yjdt)*mid_vlu_yjdt_f32_last;
    
    midline_value=(uint8)mid_vlu_yjdt_f32_used;
    
    //输出图像
    if( bmkgflg_lcd_image==1 )
    {
      LCD_Img_gray_Z(imgsite,lcdsize,(uint8*)image_binarized,imgsize);//LCD显示灰度图像
    }
    //Size_t imgsize_ys={IMG_W_YS,IMG_H_YS};
    //LCD_Img_gray_Z(imgsite,lcdsize,(uint8*)image_gray,imgsize_ys);  //LCD显示灰度图像
    //vcan_sendimg(image_binarized,IMG_H_USED*IMG_W_USED);            //山外上位机发送灰度图像
    //seekfree_sendimg_zzf(image_binarized,IMG_H_USED*IMG_W_USED);    //逐飞上位机发送灰度图像
    
    //画 前瞻指示线 中线指示线
    Site_t site_czx;
    Site_t site_spx;
    site_czx.x=(uint8)(  ( (float)midline_value )/188.0*128.0  );//垂直线
    site_spx.y=(uint8)(  ( (float)QianZhan_used      )/80.0 *128.0  );//水平线
    for(uint8 i_hx=0;i_hx<128;i_hx++)
    {
      site_czx.y=i_hx;
//      LCD_point(site_czx,BLUE);//垂直线
      site_spx.x=i_hx;
      LCD_point(site_spx,BLUE);//水平线
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
      lcd_display_site.y=0+0;//angle_clhd
      lcd_display_site.x=0;
      LCD_str(lcd_display_site,ftoa(angle_clhd,1),BLACK,WHITE);
      lcd_display_site.y=0+15;//直立pwm
      lcd_display_site.x=0;
      LCD_str(lcd_display_site,itoa((int32)pwm_common),BLACK,WHITE);
      lcd_display_site.y=35+0;//编码器测得速度
      lcd_display_site.x=0;
      LCD_str(lcd_display_site,ftoa(speed_average,1),BLACK,WHITE);
      lcd_display_site.y=35+15;//速度pwm
      lcd_display_site.x=0;
      LCD_str(lcd_display_site,itoa((int32)pwm_speed_now),BLACK,WHITE);
      lcd_display_site.y=70+0;//中线值
      lcd_display_site.x=0;
      LCD_str(lcd_display_site,itoa((int32)midline_value),BLACK,WHITE);
      lcd_display_site.y=70+15;//方向pwm
      lcd_display_site.x=0;
      LCD_str(lcd_display_site,itoa((int32)pwm_dir),BLACK,WHITE);
      lcd_display_site.y=105+0;//阈值
      lcd_display_site.x=0;
      LCD_str(lcd_display_site,itoa((int32)YuZhi),BLACK,WHITE);
      
      lcd_display_site.y=105;//阈值
      lcd_display_site.x=80;
      LCD_str(lcd_display_site,ftoa(speed_chazhi_max,1),BLACK,WHITE);
    }
    
    //串口发送相关信息
    if( bmkgflg_uart_message==1 )
    {
      //中线值
      printf("ps:%s\t",itoa((int32)midline_value));
//      extern int16 pwm_dir;
//      printf("dir:%s\t",itoa((int32)pwm_dir));
      extern float dir_pid_p_out;
      printf("p:%s\t",ftoa(dir_pid_p_out,1));
      extern float dir_pid_d_out;
      printf("d:%s\t",ftoa(dir_pid_d_out,1));
      
      printf("qz:%s\n",itoa( (int32)QianZhan_used ) );
//      extern float speed_average;
//      printf("sp:%s\n",ftoa(speed_average,1));
    }
    
    //测大循环时间结束
    Site_t site_lcd_looptime;
    site_lcd_looptime.y=0+0;//主循环时间
    site_lcd_looptime.x=112;
    LCD_str(site_lcd_looptime,itoa((int32)(pit_time_get_us(PIT1)/1000)),BLACK,WHITE); 
    
    
    mid_vlu_yjdt_f32_last=mid_vlu_yjdt_f32_used;
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
  static uint16 TimeCount_fc=0;//用于逐步加速起步 uint16型 0~65535 足够比赛用了 不用担心溢出
  if( car_speed<10 )
  {
    tmp_speed=car_speed;
  }
  else
  {
    TimeCount_fc++;
    if( TimeCount_fc<=100 )  //0~100  0~1s保持控速为0 保持原地站立
    {
      tmp_speed=0;
    }
    else
    {
      if( TimeCount_fc<=200 )//101~200 1~2s逐渐加速起步
      {
        tmp_speed=car_speed*(TimeCount_fc-100)/100;
      }
      else                   //201~65535 2s~ 始终保持为目标速度
      {
        tmp_speed=car_speed;
        
        //发车3s后允许找起跑线
        if(TimeCount_fc>=300)
        {
          //if( 不允许 ) 则允许
          if(enable_findbmx==0) enable_findbmx=1;
        }
        //发车3s后允许找起跑线
      }
    }
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