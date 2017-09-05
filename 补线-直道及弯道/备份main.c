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

Site_t P_xielv_l= {0,0};
Site_t P_xielv_r= {104,0};

zb_imgarr_t l_end_img,r_end_img;        //定义最后一行坐标与上一帧最后一行
zb_imgarr_t l_end_last_img,r_end_last_img;    

/*-------------------------定义判断赛道类型的行数线---------------------------------*/

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

uint8 Flag=0;       //起跑线结束线标志位：0 未过起跑线    1 已过起跑线 比赛开始 2再次过起跑线 比赛结束
uint8 flag_jbp=0;   //单边记步跑标志位：  0（默认）       目前有：障碍 和 起跑线

uint8 middle_line;//赛道中线位置
uint16 color_ml=BLUE;
float car_speed;



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
      //LCD_str(site,itoa((int32)angle_clhd),BLACK,WHITE);
      //图像裁剪 二值化
      ChouQu_ErZhi(image_gray,YuZhi,image_binarized);
      

      
      
      
//uint8 f_l_1;//第 1 行
//uint8 left_1;
//uint8 f_r_1;
//uint8 right_1;
//
//
//uint8 f_l_3;//第 3 行
//uint8 left_3;
//uint8 f_r_3;
//uint8 right_3;
      
      f__edge_sp_czj(image_binarized[Hang_1],93,&f_l_1,&left_1,&f_r_1,&right_1);//水平扫第一行
      f__edge_sp_czj(image_binarized[Hang_3],93,&f_l_3,&left_3,&f_r_3,&right_3);//水平扫第三行
      f__edge_sp_czj(image_binarized[Hang_end],93,&f_l_end,&left_end,&f_r_end,&right_end);//水平扫最后一行
      
      
      zb_imgarr_t l_1_img,r_1_img;            //定义图像坐标
      zb_imgarr_t l_3_img,r_3_img;
  
      
      zb_math_t l_1_math,r_1_math;            //定义数学坐标
      zb_math_t l_3_math,r_3_math;
      
      l_1_img.i=Hang_1;                       //给图像坐标赋值 
      l_1_img.j=left_1;
      r_1_img.i=Hang_1;
      r_1_img.j=right_1;
      
      l_3_img.i=Hang_3;
      l_3_img.j=left_3;
      r_3_img.i=Hang_3;
      r_3_img.j=right_3;


      
      liang_dian_lian_xian(image_binarized,125,l_1_img,l_3_img);
      liang_dian_lian_xian(image_binarized,125,r_1_img,r_3_img);
      liang_dian_lian_xian(image_binarized,125,l_1_img,r_1_img);
      liang_dian_lian_xian(image_binarized,125,l_3_img,r_3_img);
      
      l_end_img.i=Hang_end;
      l_end_img.j=left_end;
      r_end_img.i=Hang_end;
      r_end_img.j=right_end;      
      
      l_1_math=covert_zb(l_1_img);      //坐标转换用于计算斜率
      r_1_math=covert_zb(r_1_img);
      l_3_math=covert_zb(l_3_img);
      r_3_math=covert_zb(r_3_img);
      
      slope_t K_l,K_r;                          //定义左右斜率

      K_l=calcu_slope(l_1_math,l_3_math);    //计算左斜率
      K_r=calcu_slope(r_1_math,r_3_math);        //计算右斜率
      
      LCD_str(P_xielv_l,itoa((int32)K_l.value),BLACK,WHITE);
      LCD_str(P_xielv_r,itoa((int32)K_r.value),BLACK,WHITE);
      gpio_init (PTD15,GPO,1);
      gpio_init (PTE26,GPO,1);
      
      static uint8 ajflkasjf=1; 
      
      if(ajflkasjf==1)
      {
        //判断直道和弯道 
        if( ((K_l.value>0)&&(K_r.value<0))||((K_l.value>0)&&(K_r.value>0))||((K_l.value<0)&&(K_r.value<0)))
        {
          gpio_set(PTD15, 0);
        }
      }
      
      if( (ajflkasjf==1)||(ajflkasjf==2) )
      {
        //判断十字
        if(((K_l.value<0)&&(K_r.value>0))||(ajflkasjf==2))
        {
         ajflkasjf=2;
         gpio_set(PTE26,0); 
          
            
            uint8 kuandu_last;
            uint8 kuandu;
            kuandu_last=r_end_last_img.j-l_end_last_img.j;
            kuandu=r_end_img.j-l_end_img.j;
            if((kuandu_last>100)&&(kuandu<100))
            
            {
              ajflkasjf=1;
            }
          
        }
      }
      
      

      l_end_last_img=l_end_img;
      r_end_last_img=r_end_img;


      LCD_Img_gray_Z(imgsite,lcdsize,(uint8*)image_binarized,imgsize);//LCD显示灰度图像


//      vcan_sendimg(image_binarized,IMG_H_USED*IMG_W_USED);//山外上位机发送灰度图像
//     seekfree_sendimg_zzf(image_binarized,IMG_H_USED*IMG_W_USED); //逐飞上位机发送灰度图像
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
    car_speed=180;
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