/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,山外科技
 *     All rights reserved.
 *     技术讨论：山外论坛 http://www.vcan123.com
 *
 *     除注明出处外，以下所有内容版权均属山外科技所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留山外科技的版权声明。
 *
 * @file       VCAN_OV7725_Eagle.c
 * @brief      鹰眼ov7725驱动代码
 * @author     山外科技
 * @version    v5.0
 * @date       2013-09-07
 */

#include "common.h"
#include "MK60_gpio.h"
#include "MK60_port.h"
#include "MK60_dma.h"
#include "MK60_uart.h"
#include "VCAN_camera.h"



#define OV7725_EAGLE_Delay_ms(time)  DELAY_MS(time)


uint8   *ov7725_eagle_img_buff;

volatile IMG_STATUS_e      ov7725_eagle_img_flag = IMG_FINISH;   //图像状态

//内部函数声明
static void ov7725_eagle_reg_init(void);
static void ov7725_eagle_port_init();


/*!
 *  @brief      鹰眼ov7725初始化
 *  @since      v5.0
 */
uint8 ov7725_eagle_init(uint8 *imgaddr)
{
    ov7725_eagle_img_buff = imgaddr;
    ov7725_eagle_reg_init();//总钻风...
    ov7725_eagle_port_init();
    return 0;
}

/*!
 *  @brief      鹰眼ov7725管脚初始化（内部调用）
 *  @since      v5.0
 */
void ov7725_eagle_port_init()
{
    //DMA通道0初始化，PTA27触发源(默认上升沿)，源地址为PTB_B0_IN，目的地址为：IMG_BUFF，每次传输1Byte
    dma_portx2buff_init(CAMERA_DMA_CH, (void *)&PTB_B0_IN, (void *)ov7725_eagle_img_buff, PTA27, DMA_BYTE1, CAMERA_DMA_NUM, DADDR_KEEPON);

    DMA_DIS(CAMERA_DMA_CH);
    disable_irq(PORTA_IRQn);                        //关闭PTA的中断
    DMA_IRQ_CLEAN(CAMERA_DMA_CH);                   //清除通道传输中断标志位
    DMA_IRQ_EN(CAMERA_DMA_CH);

    port_init(PTA27, ALT1 | DMA_FALLING | PULLDOWN );         //PCLK

    port_init(PTA29, ALT1 | IRQ_RISING  | PULLDOWN | PF);     //场中断，上拉，上降沿触发中断，带滤波

}

/*!
 *  @brief      鹰眼ov7725场中断服务函数
 *  @since      v5.0
 */
void ov7725_eagle_vsync(void)
{

    //场中断需要判断是场结束还是场开始
    if(ov7725_eagle_img_flag == IMG_START)                   //需要开始采集图像
    {
        ov7725_eagle_img_flag = IMG_GATHER;                  //标记图像采集中
        disable_irq(PORTA_IRQn);

#if 1

        PORTA_ISFR = 1 <<  PT27;            //清空PCLK标志位

        DMA_EN(CAMERA_DMA_CH);                  //使能通道CHn 硬件请求
        PORTA_ISFR = 1 <<  PT27;            //清空PCLK标志位
        DMA_DADDR(CAMERA_DMA_CH) = (uint32)ov7725_eagle_img_buff;    //恢复地址

#else
        PORTA_ISFR = 1 <<  PT27;            //清空PCLK标志位
        dma_repeat(CAMERA_DMA_CH, (void *)&PTB_B0_IN, (void *)ov7725_eagle_img_buff,CAMERA_DMA_NUM);
#endif
    }
    else                                        //图像采集错误
    {
        disable_irq(PORTA_IRQn);                        //关闭PTA的中断
        ov7725_eagle_img_flag = IMG_FAIL;                    //标记图像采集失败
    }
}

/*!
 *  @brief      鹰眼ov7725 DMA中断服务函数
 *  @since      v5.0
 */
void ov7725_eagle_dma()
{
    ov7725_eagle_img_flag = IMG_FINISH ;
    DMA_IRQ_CLEAN(CAMERA_DMA_CH);           //清除通道传输中断标志位
}

/*!
 *  @brief      鹰眼ov7725采集图像（采集到的数据存储在 初始化时配置的地址上）
 *  @since      v5.0
 */
void ov7725_eagle_get_img()
{
    ov7725_eagle_img_flag = IMG_START;                   //开始采集图像
    PORTA_ISFR = ~0;                        //写1清中断标志位(必须的，不然回导致一开中断就马上触发中断)
    enable_irq(PORTA_IRQn);                         //允许PTA的中断
    while(ov7725_eagle_img_flag != IMG_FINISH)           //等待图像采集完毕
    {
        if(ov7725_eagle_img_flag == IMG_FAIL)            //假如图像采集错误，则重新开始采集
        {
            ov7725_eagle_img_flag = IMG_START;           //开始采集图像
            PORTA_ISFR = ~0;                //写1清中断标志位(必须的，不然回导致一开中断就马上触发中断)
            enable_irq(PORTA_IRQn);                 //允许PTA的中断
        }
    }
}


/*OV7725初始化配置表*/
reg_s ov7725_eagle_reg[] =
{
    //寄存器，寄存器值次
    {OV7725_COM4         , 0xC1},
    {OV7725_CLKRC        , 0x00},
    {OV7725_COM2         , 0x03},
    {OV7725_COM3         , 0xD0},
    {OV7725_COM7         , 0x40},
    {OV7725_HSTART       , 0x3F},
    {OV7725_HSIZE        , 0x50},
    {OV7725_VSTRT        , 0x03},
    {OV7725_VSIZE        , 0x78},
    {OV7725_HREF         , 0x00},
    {OV7725_SCAL0        , 0x0A},
    {OV7725_AWB_Ctrl0    , 0xE0},
    {OV7725_DSPAuto      , 0xff},
    {OV7725_DSP_Ctrl2    , 0x0C},
    {OV7725_DSP_Ctrl3    , 0x00},
    {OV7725_DSP_Ctrl4    , 0x00},

#if (CAMERA_W == 80)
    {OV7725_HOutSize     , 0x14},
#elif (CAMERA_W == 160)
    {OV7725_HOutSize     , 0x28},
#elif (CAMERA_W == 240)
    {OV7725_HOutSize     , 0x3c},
#elif (CAMERA_W == 320)
    {OV7725_HOutSize     , 0x50},
#else

#endif

#if (CAMERA_H == 60 )
    {OV7725_VOutSize     , 0x1E},
#elif (CAMERA_H == 120 )
    {OV7725_VOutSize     , 0x3c},
#elif (CAMERA_H == 180 )
    {OV7725_VOutSize     , 0x5a},
#elif (CAMERA_H == 240 )
    {OV7725_VOutSize     , 0x78},
#else

#endif

    {OV7725_EXHCH        , 0x00},
    {OV7725_GAM1         , 0x0c},
    {OV7725_GAM2         , 0x16},
    {OV7725_GAM3         , 0x2a},
    {OV7725_GAM4         , 0x4e},
    {OV7725_GAM5         , 0x61},
    {OV7725_GAM6         , 0x6f},
    {OV7725_GAM7         , 0x7b},
    {OV7725_GAM8         , 0x86},
    {OV7725_GAM9         , 0x8e},
    {OV7725_GAM10        , 0x97},
    {OV7725_GAM11        , 0xa4},
    {OV7725_GAM12        , 0xaf},
    {OV7725_GAM13        , 0xc5},
    {OV7725_GAM14        , 0xd7},
    {OV7725_GAM15        , 0xe8},
    {OV7725_SLOP         , 0x20},
    {OV7725_LC_RADI      , 0x00},
    {OV7725_LC_COEF      , 0x13},
    {OV7725_LC_XC        , 0x08},
    {OV7725_LC_COEFB     , 0x14},
    {OV7725_LC_COEFR     , 0x17},
    {OV7725_LC_CTR       , 0x05},
    {OV7725_BDBase       , 0x99},
    {OV7725_BDMStep      , 0x03},
    {OV7725_SDE          , 0x04},
    {OV7725_BRIGHT       , 0x00},
    {OV7725_CNST         , 0xFF},
    {OV7725_SIGN         , 0x06},
    {OV7725_UVADJ0       , 0x11},
    {OV7725_UVADJ1       , 0x02},

};

uint8 ov7725_eagle_cfgnum = ARR_SIZE( ov7725_eagle_reg ) ; /*结构体数组成员数目*/


/*!
 *  @brief      鹰眼ov7725寄存器 初始化
 *  @return     初始化结果（0表示失败，1表示成功）
 *  @since      v5.0
 */
void ov7725_eagle_reg_init(void)
{
  //设置参数 具体请参看使用手册
  uint16 light;
  //摄像头配置数组
  uint8 MT9V032_CFG[8];
  
  
  DELAY_MS(50);           //延时以保证摄像头上面的51完成上电初始化
  uart_init(UART3,115200);//初始换串口 配置摄像头
  
  //帧头
  MT9V032_CFG[0] = 0xFF;//帧头
  
  //命令位
  //具体请参看使用手册
  MT9V032_CFG[1] = 0x00;
  
  //分辨率选择位 分辨率切换后 最好断电一次系统
  //其他分辨率具体看说明书 不过列超过188后K60无法采集 提供的更大的分辨率是给STM32具有摄像头接口的用户
#define ROW 120
  switch(ROW)
  {
  case 480:MT9V032_CFG[2] = 8;break;
  case 240:MT9V032_CFG[2] = 9;break;
  case 120:MT9V032_CFG[2] = 10;break;
  default :MT9V032_CFG[2] = 10;break;
  }
#undef ROW
  
  //设置图像帧率 行数不同可设置的范围也不同 范围限制 120行的时候是1-200 240行的时候是1-132 480行的时候是1-66
  MT9V032_CFG[3] = 50;
  
  //曝光时间越大图像越亮 由于最大曝光时间受到fps与分辨率的共同影响 这里不给出具体范围 可以直接把曝光设置为1000
  //摄像头上的51收到之后会根据分辨率及FPS计算最大曝光时间 然后把曝光时间设置为最大曝光时间
  //并且会返回当前设置的最大曝光时间 这样就可以知道最大可以设置的曝光时间是多少了
  //然后觉得图像过亮 就可以在这个最大曝光值的基础上相应减少
  light = 1000;
  MT9V032_CFG[4] = light>>8;    //曝光时间高八位
  MT9V032_CFG[5] = (uint8)light;//曝光时间低八位
  
  //设置为0表示关闭自动曝光，设置1-63则启用自动曝光，设置的越大图像就越亮  建议开启这个功能，可以有效适应各种场地
  MT9V032_CFG[6] = 0;
  
  //帧尾
  MT9V032_CFG[7] = 0x5A;
  
  //通过串口发送配置参数
  uart_putbuff(UART3,MT9V032_CFG,8);
  DELAY_MS(50);//延时以保障上个配置数据51已经成功写入到摄像头
  
  
  
  
  //以下用于设置摄像头亮度 与上面的曝光时间是不一样的
  MT9V032_CFG[0] = 0xFF;  //帧头
  MT9V032_CFG[1] = 0x02;  //命令位  
  MT9V032_CFG[2] = 0;     //无用，需设为0
  MT9V032_CFG[3] = 0;     //无用，需设为0
  MT9V032_CFG[4] = 0;     //无用，需设为0
  MT9V032_CFG[5] = 32;    //亮度等级选择 亮度等级 1 - 64
  MT9V032_CFG[6] = 0x35;
  MT9V032_CFG[7] = 0x5A;  //帧尾
  uart_putbuff(UART3,MT9V032_CFG,8);
  DELAY_MS(50);//延时以保障上个配置数据51已经成功写入到摄像头
}


