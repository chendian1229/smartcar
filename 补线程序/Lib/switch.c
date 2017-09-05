#include "common.h"
#include "MK60_port.h"
#include "MK60_gpio.h"
#include "switch.h"

uint8 bmkgflg_ctrspeed;   //控速开关
uint8 bmkgflg_lcd_image;  //LCD显示图像开关
uint8 bmkgflg_lcd_message;//LCD显示信息开关
uint8 bmkgflg_uart_message;//LCD显示信息开关

uint8 bmkgflg_hxzy;
uint8 bmkgflg_csxz;

void button_init()
{
  gpio_init(PTB16,GPI,1); //KEY1 确认
  gpio_init(PTB17,GPI,1); //KEY2 上
  gpio_init(PTB18,GPI,1); //KEY3 返回
  gpio_init(PTB19,GPI,1); //KEY4 左
  gpio_init(PTB20,GPI,1); //KEY5 下
  gpio_init(PTB21,GPI,1); //KEY6 右
}
//拨码开关相关GPIO初始化
void bmkg_init()
{
  gpio_init(PTE4,GPI,1); //上下 1 是否关闭控速
  gpio_init(PTE5,GPI,1); //上下 2 是否关闭显示图像
  gpio_init(PTE6,GPI,1); //上下 3 是否关闭显示参数
  gpio_init(PTE7,GPI,1); //上下 4
  gpio_init(PTE8,GPI,1); //左右
  gpio_init(PTE9,GPI,1); //左右
  gpio_init(PTE10,GPI,1);//左右
  gpio_init(PTE11,GPI,1);//左右
}
//拨码开关读取IO电平
void bmkg_read_IO()
{
  bmkgflg_ctrspeed    = gpio_get(PTE4);
  bmkgflg_lcd_image   = gpio_get(PTE5);
  bmkgflg_lcd_message = gpio_get(PTE6);
  bmkgflg_uart_message= gpio_get(PTE7);
  
  bmkgflg_hxzy=gpio_get(PTE8);//左0 右1
  
  
  
  bmkgflg_csxz=gpio_get(PTE9);
}