#include "common.h"
#include "include.h"

uint8 image_gray[CAMERA_H][CAMERA_W];         //灰度图像 高:120 宽:188
uint8 image_binarized[CAMERA_H/2][CAMERA_W/2];//抽取及二值化后的图像
uint8 image_jzh[CAMERA_H/2][CAMERA_W/2];

uint8 f_l_1;
uint8 left_1;
uint8 f_r_1;
uint8 right_1;

uint8 f_l_2;
uint8 left_2;
uint8 f_r_2;
uint8 right_2;

uint8 f_l_pao;
uint8 left_pao;
uint8 f_r_pao;
uint8 right_pao;


uint8 flag_chuizhi;
uint8 hight_chuizhi;

uint8 position;

uint8 i,j;//用于各种循环

//速度相关
float speed=0;



//函数声明
void PORTA_IRQHandler();
void DMA0_IRQHandler();
void PIT0_ISR();




void main()
{
  //LCD初始化
  LCD_init();
  Site_t imgsite={0,0};
  Site_t site;
  Size_t imgsize={CAMERA_W/2,CAMERA_H/2};
  Size_t lcdsize;
  lcdsize.H = LCD_H;//实测:128
  lcdsize.W = LCD_W;//实测:128
  
  //摄像头初始化
  camera_init( (uint8*)image_gray );//一维数组首地址<--二维数组首地址(强制转化)
  
  //直立控制初始化
  Car_Control_Init();
  
  //摄像头中断服务函数配置
  set_vector_handler(PORTA_VECTORn,PORTA_IRQHandler);//设置 PORTA 的中断服务函数为 PORTA_IRQHandler
  set_vector_handler(DMA0_VECTORn,DMA0_IRQHandler);  //设置 DMA0 的中断服务函数为 PORTA_IRQHandler
  
  //定时器中断函数配置
  pit_init_ms(PIT0,10);                     //初始化PIT0 定时时间10ms
  set_vector_handler(PIT0_VECTORn,PIT0_ISR);//设置PIT0的中断服务函数为PIT0_ISR
  
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
  
  //使能PIT0中断(10ms)
  enable_irq(PIT0_IRQn);
  
  while(1)
  {

    //测大循环时间开始
    pit_time_start(PIT2);
    
    
    
    
    
    //采集图像
    camera_get_img();
    //图像抽取及二值化
    ChouQu_ErZhi(image_gray,image_binarized);
    //桶形失真矫正
    //JiaoZheng(image_binarized,image_jzh);
    //显示
    LCD_Img_gray_Z(imgsite,lcdsize,(uint8*)image_binarized,imgsize);
    //图像处理
    #define Hang_1 58
    #define Hang_pao 44
    #define Hang_2 38
    #define Hang_3 18
    Scan_Edge_ShuiPing(image_binarized[Hang_1],&f_l_1,&left_1,&f_r_1,&right_1);
    Scan_Edge_ShuiPing(image_binarized[Hang_2],&f_l_2,&left_2,&f_r_2,&right_2);
    Scan_Edge_ShuiPing(image_binarized[Hang_pao],&f_l_pao,&left_pao,&f_r_pao,&right_pao);
    
    if(  (right_1-left_1)>=(right_2-left_2)  )//正常赛道 上行宽度<下行宽度
    {
//        uint8 f_l_2;
//        uint8 left_2;
//        uint8 f_r_2;
//        uint8 right_2;
//        #define kuandu_2 30.0
      if( (f_l_pao==1)&&(f_r_pao==1) )
      {
        position=(right_pao+left_pao)/2;
      }
      
      if( (f_l_pao==1)&&(f_r_pao==0) )
      {
        position=left_pao+22;
      }
      
      if( (f_l_pao==0)&&(f_r_pao==1) )
      {
        position=right_pao-22;
      }
      
    }
    else
    {
      Scan_Edge_ChuiZhi(image_binarized,&flag_chuizhi,&hight_chuizhi);
      if( flag_chuizhi==0 )
      {//十字
        position=49;//等于Dir_PID目标值 即方向控制输出为0
      }
      else
      {//环形
        position=right_pao-22;
      }
    }
    
    
    
    //画位置水平线
    for(i=0;i<128;i++)
    {
      site.x=i;
      
      site.y=(uint8)(   ( (float)Hang_1 )/60.0*128.0   );
      LCD_point(site,BLUE);
      
      site.y=(uint8)(   ( (float)Hang_2 )/60.0*128.0   );
      LCD_point(site,BLUE);
      
      site.y=(uint8)(   ( (float)Hang_3 )/60.0*128.0   );
      LCD_point(site,BLUE);
      
      site.y=(uint8)(   ( (float)Hang_pao )/60.0*128.0   );
      LCD_point(site,RED);
    }

    
//      //打印相关信息
//      printf("S_n   %d   :%d\t%d\t%d\t",S_n,S_arr[0],S_arr[1],S_arr[2]);
//      printf("X_n   %d   :%d\t%d\t%d\t",X_n,X_arr[0],X_arr[1],X_arr[2]);
//
//      printf("posi:%d\t",position);
//      extern int16 dir_pid_result;//control_car.c 方向环pid整体输出
//      extern float dir_d;//pid.c 方向环pid微分项输出
//      printf("d:%s\t",ftoa(dir_d,1));
//      printf("all:%d\t",dir_pid_result);
    
    
    
    
    
    printf("%d\t%d\t%d\t%d\t",left_pao,right_pao,right_pao-left_pao,position);
    
    
    
    //测大循环时间结束
    printf("loopT:\t%dus\n",pit_time_get_us(PIT2));

  }
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
  Car_Control(0);//直立控制
}