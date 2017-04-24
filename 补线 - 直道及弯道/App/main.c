/*----------------------------------2017 东秦光电直立一队软件----------------------------------*/
/*------------------------------------by:YangBing&SuChenHui------------------------------------*/
#include "common.h"
#include "include.h"

uint8 YuZhi;                                  //二值化阈值
uint8 image_gray[IMG_H_YS][IMG_W_YS];         //原始灰度图像         高:120 宽:188
uint8 image_binarized[IMG_H_USED][IMG_W_USED];//抽取及二值化后的图像 高:80  宽:188

uint8 midline_flag;
uint8 midline_value;
float car_speed;

//*************************

//*************************







void main()
{
  //LCD初始化
  LCD_init();
  Site_t imgsite={0,0};
  Size_t imgsize={IMG_W_USED,IMG_H_USED};
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
  //采集一幅图像 求阈值
  if(mt9v032_finish_flag)
  {
    YuZhi=OTSU(image_gray);
    mt9v032_finish_flag = 0;
  }
  //使能直立PIT0中断(10ms)
  enable_irq(PIT0_IRQn);
  /*------------------------------------分--------割--------线-----------------------------------------*/
  while(1)
  {
    if(mt9v032_finish_flag)
    {
      //采图像
      mt9v032_finish_flag = 0;
      //超速保护
      Over_Speed_Protect(450);
      //左上角显示angle
      extern float angle_clhd;//Car_Control.c
      Site_t site;
      site.y=0;
      site.x=0;
      LCD_str(site,itoa((int32)angle_clhd),BLACK,WHITE);
      //图像裁剪 二值化
      ChouQu_ErZhi(image_gray,YuZhi,image_binarized);
      
      
//----------------------------------------------
      pdsdkx(image_binarized);
//----------------------------------------------
      
      /***************提取中线算法开始***************/
      
      //huanxing_ru_bxcx(image_binarized);
      
      /***************提取中线算法结束***************/
      
      #define QianZhan 55
      
      //从补线后的图像中扫描中线值 用于打方向
      f__midline(image_binarized[QianZhan],1,60,&midline_flag,&midline_value);
      
      //画指示线
      Site_t site_czx;
      Site_t site_spx;
      site_czx.x=(uint8)(   ( (float)midline_value )/188.0*128.0   );//画垂直线中线
      for(uint8 i_hx=0;i_hx<128;i_hx++)
      {
        site_czx.y=i_hx;
//        LCD_point(site_czx,GREEN);//画垂直线中线
        
        site_spx.x=i_hx;
        site_spx.y=(uint8)(   ( (float)QianZhan )/80.0*128.0   );//画水平线
        LCD_point(site_spx,GREEN);
      }
      
      
      //画出最中间两列线（93 94）
      Site_t site_zzjczx_l;//最中间垂直线l
      Site_t site_zzjczx_r;//最中间垂直线r
      site_zzjczx_l.x=(uint8)( 93.0/188.0*128.0 );//画垂直线中线l
      site_zzjczx_r.x=(uint8)( 94.0/188.0*128.0 );//画垂直线中线r
      for(uint8 i_hx=0;i_hx<128;i_hx++)
      {
        site_zzjczx_l.y=i_hx;
        LCD_point(site_zzjczx_l,GREEN);//画垂直线中线l
        site_zzjczx_r.y=i_hx;
        LCD_point(site_zzjczx_r,GREEN);//画垂直线中线r
      }
      
      //输出图像
      LCD_Img_gray_Z(imgsite,lcdsize,(uint8*)image_binarized,imgsize);//LCD显示灰度图像
//      vcan_sendimg(image_binarized,IMG_H_USED*IMG_W_USED);//山外上位机发送灰度图像
//      seekfree_sendimg_zzf(image_binarized,IMG_H_USED*IMG_W_USED); //逐飞上位机发送灰度图像
      
      #undef QianZhan
    }
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
  static uint16 flag=0;
  flag++;
  if( flag<=100 )
  {
    car_speed=0;
  }
  else
  {
    car_speed=190*1.2;
  }
  Car_Control(midline_value,95,car_speed);//MeasuredDir TargetDir TargetSpeed
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