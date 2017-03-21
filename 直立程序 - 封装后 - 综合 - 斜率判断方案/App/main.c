#include "common.h"
#include "include.h"

#include <math.h>
#include "ctr_motor.h"


uint8 image_gray[CAMERA_H][CAMERA_W];         //灰度图像 高:120 宽:188
uint8 image_binarized[IMAGE_ARR_HEIGHT][IMAGE_ARR_WIDTH];//抽取及二值化后的图像



#define Hang_6 10 //往上扫的极限
#define Hang_5 35 //障碍判断
#define Hang_4 40 //
#define Hang_3 50 //3 2 打方向
#define Hang_2 55 //
#define Hang_1 70 //1 3 斜率判断


uint8 f_l_1;//第 1 行
uint8 left_1;
uint8 f_r_1;
uint8 right_1;
uint16 color_1=BLUE;

uint8 f_l_2;//第 2 行
uint8 left_2;
uint8 f_r_2;
uint8 right_2;
uint16 color_2=BLUE;

uint8 f_l_3;//第 3 行
uint8 left_3;
uint8 f_r_3;
uint8 right_3;
uint16 color_3=BLUE;

uint8 f_l_4;//第 4 行
uint8 left_4;
uint8 f_r_4;
uint8 right_4;
uint16 color_4=BLUE;

uint8 f_l_5;//第 5 行
uint8 left_5;
uint8 f_r_5;
uint8 right_5;
uint16 color_5=BLUE;

uint8 middle_line;//赛道中线位置
uint16 color_mlp=BLUE;

uint8 flag_jbp=0;//单边记步跑标志位 默认不启用

uint8 FLAG=0;//0 未过起跑线 1 已过起跑线 比赛开始 2再次过起跑线 比赛结束

uint8 i,j;//用于各种循环


typedef enum
{
  ZhiDao,       //直道
  ZhangAi_zuo,  //左侧障碍 需要明确的是 障碍只会出现在直道上
  ZhangAi_you,  //右侧障碍
  WanDao,       //弯道
  ShiZi,        //十字
  PoDaoXian,    //坡道
  HuanXing,     //环形
}SaiDao_type_structer;
SaiDao_type_structer SaiDaoLeiXing;


//函数声明
void PORTA_IRQHandler();
void DMA0_IRQHandler();
void PIT0_ISR();




//调试开关... 
#define SWITCH_LCD_TUXIANG
//#define SWITCH_UART_MESSAGE
#define SWITCH_LCD_HUAXIAN
#define SWITCH_UART_LOOP_TIME

void main()
{
  //LCD初始化
  LCD_init();
  Site_t imgsite={0,0};
  Size_t imgsize={IMAGE_ARR_WIDTH,IMAGE_ARR_HEIGHT};
  Size_t lcdsize;
  lcdsize.H = LCD_H;//实测:128
  lcdsize.W = LCD_W;//实测:128
  
  Site_t site;
  Site_t site_czx;
  Site_t site_spx;
  
  
  //摄像头初始化
  camera_init( (uint8*)image_gray );//一维数组首地址<--二维数组首地址(强制转化)
  
  //直立控制初始化
  Car_Control_Init();
  
  //摄像头中断服务函数配置
  set_vector_handler(PORTA_VECTORn,PORTA_IRQHandler);//设置 PORTA 的中断服务函数为 PORTA_IRQHandler
  set_vector_handler(DMA0_VECTORn,DMA0_IRQHandler);  //设置 DMA0 的中断服务函数为 PORTA_IRQHandler
  
  //定时器中断函数配置
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
  
  
  //采集图像
  camera_get_img();

  //计算阈值
  extern uint8 ThresholdValue;//image_processing.c
  ThresholdValue = OTSU(image_gray[15],image_gray[30],image_gray[45],image_gray[60]);
  
  
  //使能直立PIT0中断(10ms)
  enable_irq(PIT0_IRQn);
  /*------------------------------------分--------割--------线-----------------------------------------*/
  while(1)
  {
    //测大循环时间开始
    #ifdef SWITCH_UART_LOOP_TIME
    pit_time_start(PIT2);
    #endif
    
    //车速过快保护
    extern float speed_average;
    if( (speed_average>30)||(speed_average<-30) )
    {
      disable_irq(PIT0_IRQn);
      Ctr_Motor_L(0);
      Ctr_Motor_R(0);
    }
    
    //采集图像
    camera_get_img();
    
    //图像抽取及二值化
    ChouQu_ErZhi(image_gray,image_binarized);
    
    //桶形失真矫正
    //JiaoZheng(image_binarized,image_jzh);
    
    //显示图像
    #ifdef SWITCH_LCD_TUXIANG
    LCD_Img_gray_Z(imgsite,lcdsize,(uint8*)image_binarized,imgsize);
    #endif
    
    //图像处理
    Scan_Edge_ShuiPing(image_binarized[Hang_5],&f_l_5,&left_5,&f_r_5,&right_5);//Hang_5
    Scan_Edge_ShuiPing(image_binarized[Hang_4],&f_l_4,&left_4,&f_r_4,&right_4);//Hang_4
    Scan_Edge_ShuiPing(image_binarized[Hang_3],&f_l_3,&left_3,&f_r_3,&right_3);//Hang_3
    Scan_Edge_ShuiPing(image_binarized[Hang_2],&f_l_2,&left_2,&f_r_2,&right_2);//Hang_2
    Scan_Edge_ShuiPing(image_binarized[Hang_1],&f_l_1,&left_1,&f_r_1,&right_1);//Hang_1
    
    
    
    int8 K_l = Calculate_Angle(Hang_1,left_1,Hang_3,left_3);
    int8 K_r = Calculate_Angle(Hang_1,right_1,Hang_3,right_3);
    
    
    if(K_l==90)
    {
      K_l=-90;
    }
//    if( (-90<K_l)&&(K_l<90))
//    {
//    }
//    else
//    {
//      K_l=0;
//    }
//    if( (-90<K_r)&&(K_r<90))
//    {
//    }
//    else
//    {
//      K_l=0;
//    }
    
    
    site.x=0;
    
    site.y=0;
    extern float Speed_PID_Value;//Car_Control.c
    LCD_str(site,ftoa(Speed_PID_Value,2), BLACK,WHITE);
    
    site.x=80;
    extern float angle_clhd;//Car_Control.c
    LCD_str(site,itoa((int32)angle_clhd), BLACK,WHITE);
    
    site.x=0;
    site.y=(uint16)(   ( (float)Hang_2 )/80.0*128.0   );
    LCD_str(site,itoa(right_2-left_2), BLACK,WHITE);
    
    site.y=(uint16)(   ( (float)Hang_3 )/80.0*128.0   );
    LCD_str(site,itoa(right_3-left_3), BLACK,WHITE);
    
    site.y=(uint16)(   ( (float)Hang_1 )/80.0*128.0   );;
    LCD_str(site,itoa(K_l), BLACK,WHITE);
    
    site.x=100;
    LCD_str(site,itoa(K_r), BLACK,WHITE);
    
    
    
//    //串口输出各种信息
//    #ifdef SWITCH_UART_MESSAGE
    printf("l_4:%d\tr_4:%d\tL_4:%d\t",left_4,right_4,(right_4-left_4));
    printf("l_5:%d\tr_5:%d\tL_5:%d\t",left_5,right_5,(right_5-left_5));
//    printf("yuzhi:%d\t",ThresholdValue);
//    #endif
//    
    
    
    
    //******************判断赛道类型******************
    extern int64 EncoderCounter_L;//Car_Control.c
    extern int64 EncoderCounter_R;
    if( flag_jbp==0 )
    {
      //左正右负 直道
      if( (K_l>0)&&(K_r<0) )
      {
        SaiDaoLeiXing=ZhiDao;
        if( (right_3-left_3)<20 )//说明遇到了起跑线/结束线
        {
          EncoderCounter_L=0;
          EncoderCounter_R=0;
          flag_jbp=1;
        }
        else
        {
          if( (((right_4-left_4)-(right_5-left_5))>10) && ((right_4-left_4)>25) && ((right_5-left_5)>25) )
          {
            //有障碍 但不知道在哪侧
            if( (((right_5+left_5)/2)-((right_4+left_4)/2))>0 )
            {
              //左侧障碍
              SaiDaoLeiXing=ZhangAi_zuo;
              
              EncoderCounter_L=0;
              EncoderCounter_R=0;
              flag_jbp=2;
            }
            else
            {
              //右侧障碍
              SaiDaoLeiXing=ZhangAi_you;

              EncoderCounter_L=0;
              EncoderCounter_R=0;
              flag_jbp=2;
            }
          }
        }
        
        
      }
      //同符号 弯道
      if( ((K_l>0)&&(K_r>0))||((K_l<0)&&(K_r<0)) )
      {
        SaiDaoLeiXing=WanDao;
      }
      //左负右正
      if( (K_l<0)&&(K_r>0) )
      {
        //十字赛道
        if( Scan_Edge_ChuiZhi(image_binarized,40,10)==0 )
        {
          SaiDaoLeiXing = ShiZi;
        }
        //环形赛道
        else
        {
          SaiDaoLeiXing = HuanXing;
        }
      }
    }
    if(flag_jbp==1)//编码器记步跑法 未达到规定距离前不会更新赛道类型
    {
      if( (EncoderCounter_L+EncoderCounter_R)>5000 )
      {
        flag_jbp=0;
        FLAG++;
      }
    }
    if(flag_jbp==2)//编码器记步跑法 未达到规定距离前不会更新赛道类型
    {
      if( (EncoderCounter_L+EncoderCounter_R)>8000 )
      {
        flag_jbp=0;
      }
    }
    
    //******************依据判断赛道打方向******************
    //FLAG==0 说明未过起跑线    发车中...
    //FLAG==1 说明已过起跑线    比赛中...
    //FLAG==2 说明再次过起跑线  比赛结束...
    if( (FLAG==1)||(FLAG==0) )
    {
    //直道怎么跑
    if( SaiDaoLeiXing == ZhiDao )
    {
      if( (right_3-left_3)<=(right_2-left_2) )//谁小用谁
      {
        if( (f_l_3==1)&&(f_r_3==1) )
        {
          middle_line=(right_3+left_3)/2;
        }
        if( (f_l_3==1)&&(f_r_3==0) )
        {
          middle_line=left_3+32;
        }
        if( (f_l_3==0)&&(f_r_3==1) )
        {
          middle_line=right_3-32;
        }
        
        
        color_mlp=GBLUE;//指示线条换色
        color_3=RED;
        color_2=BLUE;
        color_1=BLUE;
        
      }
      else//谁小用谁
      {
        if( (f_l_2==1)&&(f_r_2==1) )
        {
          middle_line=(right_2+left_2)/2;
        }
        if( (f_l_2==1)&&(f_r_2==0) )
        {
          middle_line=left_2+36;
        }
        if( (f_l_2==0)&&(f_r_2==1) )
        {
          middle_line=right_2-36;
        }
        
        
        color_mlp=GBLUE;//指示线条换色
        color_3=BLUE;
        color_2=RED;
        color_1=BLUE;
      }
    }
    //弯道怎么跑
    if( SaiDaoLeiXing == WanDao )
    {
      if( (right_3-left_3)<=(right_2-left_2) )//谁小用谁
      {
        if( (f_l_3==1)&&(f_r_3==1) )
        {
          middle_line=(right_3+left_3)/2;
        }
        if( (f_l_3==1)&&(f_r_3==0) )
        {
          middle_line=left_3+35;
        }
        if( (f_l_3==0)&&(f_r_3==1) )
        {
          middle_line=right_3-35;
        }
        
        
        color_mlp=YELLOW;//指示线条换色
        color_3=RED;
        color_2=BLUE;
        color_1=BLUE;
      }
      else//谁小用谁
      {
        if( (f_l_2==1)&&(f_r_2==1) )
        {
          middle_line=(right_2+left_2)/2;
        }
        if( (f_l_2==1)&&(f_r_2==0) )
        {
          middle_line=left_2+36;
        }
        if( (f_l_2==0)&&(f_r_2==1) )
        {
          middle_line=right_2-36;
        }
        
        
        color_mlp=YELLOW;//指示线条换色
        color_3=BLUE;
        color_2=RED;
        color_1=BLUE;
      }
    }
    //十字怎么跑
    if(  SaiDaoLeiXing == ShiZi  )
    {
      middle_line=95;
      
      
      color_mlp=RED;//指示线条换色
      color_3=BLUE;
      color_2=BLUE;
      color_1=BLUE;
    }
    //环形怎么跑
    if(  SaiDaoLeiXing == HuanXing  )
    {
      middle_line=right_2-39;//单边跑
      
      
      color_mlp=GREEN;//指示线条换色
      color_3=BLUE;
      color_2=BLUE;
      color_1=BLUE;
    }
    //左侧障碍怎么跑
    if(  SaiDaoLeiXing == ZhangAi_zuo  )
    {
      middle_line=right_2-17;
      
      color_mlp=BLACK;//指示线条换色
      color_3=BLUE;
      color_2=BLUE;
      color_1=BLUE;
    }
    //右侧障碍怎么跑
    if(  SaiDaoLeiXing == ZhangAi_you  )
    {
      middle_line=left_2+22;
      
      color_mlp=BLACK;//指示线条换色
      color_3=BLUE;
      color_2=BLUE;
      color_1=BLUE;
    }
    
    
    }
    else
    {
      disable_irq(PIT0_IRQn);
      Ctr_Motor_L(0);
      Ctr_Motor_R(0);
    }
    
    
    
    //画指示线
    #ifdef SWITCH_LCD_HUAXIAN
    site_czx.x=(uint8)(   ( (float)middle_line )/188.0*128.0   );//画垂直线中线
    for(i=0;i<128;i++)
    {
      site_czx.y=i;
      LCD_point(site_czx,color_mlp);                             //画垂直线中线
      
      site_spx.x=i;
      site_spx.y=(uint8)(   ( (float)Hang_1 )/80.0*128.0   );    //画水平线1
      LCD_point(site_spx,color_1);
      site_spx.y=(uint8)(   ( (float)Hang_2 )/80.0*128.0   );    //画水平线2
      LCD_point(site_spx,color_2);
      site_spx.y=(uint8)(   ( (float)Hang_3 )/80.0*128.0   );    //画水平线3
      LCD_point(site_spx,color_3);
      site_spx.y=(uint8)(   ( (float)Hang_4 )/80.0*128.0   );    //画水平线4 用于判断障碍
      LCD_point(site_spx,BLUE);
      site_spx.y=(uint8)(   ( (float)Hang_5 )/80.0*128.0   );    //画水平线5 用于判断障碍
      LCD_point(site_spx,BLUE);
      site_spx.y=(uint8)(   ( (float)Hang_6 )/80.0*128.0   );    //画水平线6 指示往上扫的极限
      LCD_point(site_spx,BLUE);
    }
    #endif
    

    //测大循环时间结束
    #ifdef SWITCH_UART_LOOP_TIME
    printf("loopT:\t%dus\n",pit_time_get_us(PIT2));
    #endif
  }
}
/**********************************************************************************************
                                   定时器中断函数(10ms)
**********************************************************************************************/
void PIT0_ISR()
{
  /*
  清中断标志位
  */
  PIT_Flag_Clear(PIT0);
  
  
  /*
  功能代码部分
  */
  Car_Control(12);//直立控制
}
/**********************************************************************************************
                                     PORTA中断服务函数
**********************************************************************************************/
void PORTA_IRQHandler()
{
    uint8  n;    //引脚号
    uint32 flag;

    while(!PORTA_ISFR);
    flag = PORTA_ISFR;
    PORTA_ISFR  = ~0;                                   //清中断标志位

    n = 29;                                             //场中断
    if(flag & (1 << n))                                 //PTA29触发中断
    {
        camera_vsync();
    }
#if ( CAMERA_USE_HREF == 1 )                            //使用行中断
    n = 28;
    if(flag & (1 << n))                                 //PTA28触发中断
    {
        camera_href();
    }
#endif

}
/**********************************************************************************************
                                     DMA0中断服务函数
**********************************************************************************************/
void DMA0_IRQHandler()
{
    camera_dma();
}