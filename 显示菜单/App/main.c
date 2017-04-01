#include "common.h"
#include "include.h"

//函数声明
void PORTA_IRQHandler();
void DMA0_IRQHandler();
void PIT0_ISR();


extern void main_menu_task(void);

void pit_hander(void)
{
  if(PIT_TFLG(PIT2)==1)
  {
    PIT_Flag_Clear(PIT2);
    
    key_IRQHandler();
  }
}

void main()
{
  key_init(KEY_MAX);
  
  pit_init_ms(PIT2,10);
  set_vector_handler(PIT2_VECTORn,pit_hander);
  
  //使能PIT2中断(10ms)
  enable_irq(PIT2_IRQn);
  
  
  //LCD初始化
  LCD_init();

  while(1)
  {
    main_menu_task();
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
