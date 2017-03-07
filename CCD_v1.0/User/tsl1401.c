/*
 * 包含头文件
 */
#include "common.h"
#include "MK60_port.h"
#include "MK60_gpio.h"
#include "MK60_adc.h"
#include "MK60_uart.h"
#include "tsl1401.h"




#define SI_SetVal()   PTE4_OUT = 1
#define SI_ClrVal()   PTE4_OUT = 0
#define CLK_ClrVal()  PTE5_OUT = 0
#define CLK_SetVal()  PTE5_OUT = 1




/*************************************************************************
*                           蓝宙电子工作室
*
*  函数名称：SamplingDelay
*  功能说明：CCD延时程序 200ns
*  参数说明：
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*************************************************************************/
void SamplingDelay(void)
{
	volatile uint8 i;
	for(i=0;i<1;i++)
	{
		asm("nop");
		asm("nop");
	}
}
/*************************************************************************
                 函数名称：CCD_init
                 功能说明：CCD初始化
*************************************************************************/
void CCD_init(void)
{
  gpio_init (PTE4, GPO,HIGH);
  gpio_init (PTE5, GPO,HIGH);
  adc_init(ADC0_SE8) ;//PTB0
}
/*************************************************************************
              函数名称：StartIntegration
              功能说明：CCD启动程序
*************************************************************************/
void StartIntegration(void)
{

    unsigned char i;

    SI_SetVal();            /* SI  = 1 */
    SamplingDelay();
    CLK_SetVal();           /* CLK = 1 */
    SamplingDelay();
    SI_ClrVal();            /* SI  = 0 */
    SamplingDelay();
    CLK_ClrVal();           /* CLK = 0 */

    for(i=0; i<127; i++)
    {
        SamplingDelay();
        SamplingDelay();
        CLK_SetVal();       /* CLK = 1 */
        SamplingDelay();
        SamplingDelay();
        CLK_ClrVal();       /* CLK = 0 */
    }
    SamplingDelay();
    SamplingDelay();
    CLK_SetVal();           /* CLK = 1 */
    SamplingDelay();
    SamplingDelay();
    CLK_ClrVal();           /* CLK = 0 */
}
/*************************************************************************
              *  函数名称：ImageCapture
              *  功能说明：CCD采样程序
              *  参数说明：* ImageData   采样数组
              *ImageData =  ad_once(ADC1, AD6a, ADC_8bit);
*************************************************************************/
void ImageCapture(unsigned char * ImageData)
{

    unsigned char i;
    extern uint8 AtemP ;

    SI_SetVal();            /* SI  = 1 */
    SamplingDelay();
    CLK_SetVal();           /* CLK = 1 */
    SamplingDelay();
    SI_ClrVal();            /* SI  = 0 */
    SamplingDelay();

    //Delay 10us for sample the first pixel
    /**/
    for(i = 0; i < 200; i++)
    {                    //更改250，让CCD的图像看上去比较平滑，
      SamplingDelay() ;  //200ns                  //把该值改大或者改小达到自己满意的结果。
    }

    //Sampling Pixel 1

    *ImageData =  adc_once(ADC0_SE8,ADC_8bit);
    ImageData ++ ;
    CLK_ClrVal();           /* CLK = 0 */

    for(i=0; i<127; i++)
    {
        SamplingDelay();
        SamplingDelay();
        CLK_SetVal();       /* CLK = 1 */
        SamplingDelay();
        SamplingDelay();
        //Sampling Pixel 2~128

       *ImageData =  adc_once(ADC0_SE8,ADC_8bit);
        ImageData ++ ;
        CLK_ClrVal();       /* CLK = 0 */
    }
    SamplingDelay();
    SamplingDelay();
    CLK_SetVal();           /* CLK = 1 */
    SamplingDelay();
    SamplingDelay();
    CLK_ClrVal();           /* CLK = 0 */
}
/*************************************************************************
*                           蓝宙电子工作室
*
*  函数名称：SendHex
*  功能说明：采集发数程序
*  参数说明：
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*************************************************************************/
void SendHex(unsigned char hex)
{
  unsigned char temp;
  temp = hex >> 4;
  if(temp < 10)        uart_putchar(UART0,temp + '0');
  else                 uart_putchar(UART0,temp - 10 + 'A');

  temp = hex & 0x0F;

  if(temp < 10)     uart_putchar(UART0,temp + '0');
  else              uart_putchar(UART0,temp - 10 + 'A');

}
/*************************************************************************
*                           蓝宙电子工作室
*
*  函数名称：SendImageData
*  功能说明：
*  参数说明：
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*************************************************************************/
void SendImageData(unsigned char * ImageData)
{

    unsigned char i;
    unsigned char crc = 0;

    /* Send Data */
    uart_putchar(UART0,'*');
    uart_putchar(UART0,'L');
    uart_putchar(UART0,'D');

    SendHex(0);
    SendHex(0);
    SendHex(0);
    SendHex(0);

    for(i=0; i<128; i++)
    {
      SendHex(*ImageData++);
    }

    SendHex(crc);
    uart_putchar(UART0,'#');
}
/*************************************************************************
*                         第十二届东秦光电直立一队
*  函数名称：二值化程序
*  功能说明：
*  参数说明：
*  函数返回：
*  修改时间：
*  备    注：
*************************************************************************/
void ErZhiHua(uint8 *ImageData)
{
	uint8 i=0;
	for(i=0;i<128;i++)
	{
		if(ImageData[i]>240)
		{
			ImageData[i]=255;
		}
		else
		{
			ImageData[i]=150;
		}
	}
}
/*************************************************************************
*                         第十二届东秦光电直立一队
*  函数名称：从二值化后的数据中计算宽度 仅在初始化时执行一次
*  功能说明：
*  参数说明：
*  函数返回：
*  修改时间：
*  备    注：
*************************************************************************/
uint8 JiSuanKuanDu(uint8 *ImageData)
{
	uint8 KuanDu;

	uint8 position_low;
	uint8 position_high;

	uint8 i;
	for(i=0;i<128;i++)
	{
		if( ImageData[i]==255 )
		{
			position_low=i;
			break;//找到position_low后 结束整个for循环
		}
	}

	for(i=position_low;i<128;i++)
	{
		if( ImageData[i]==150 )
		{
			position_high=i;
			break;//找到position_high后 结束整个for循环
		}
	}

	KuanDu=position_high-position_low;
	return KuanDu;
}
/*************************************************************************
*                         第十二届东秦光电直立一队
*  函数名称：从二值化后的数据中 找 中间位置
*  功能说明：
*  参数说明：
*  函数返回：
*  修改时间：
*  备    注：
*************************************************************************/
uint8 ZhaoZhongJian_From_ErZhiHuaHouDe(uint8 *ImageData,uint8 KuanDu)
{
	uint8 position;//函数返回值
	uint8 position_low;
	uint8 position_high;

	uint8 i;
	for(i=0;i<128;i++)
	{
		if( ImageData[i]==255 )
		{
			position_low=i;
			break;//找到position_low后 结束整个for循环
		}
	}

	for(i=position_low;i<128;i++)
	{
		if( ImageData[i]==150 )
		{
			position_high=i;
			break;//找到position_high后 结束整个for循环
		}
	}

	if(position_high>120)
	{
		position=position_low+KuanDu/2;
	}
	else
	{
		if(position_low<7)
		{
			position=position_high-KuanDu/2;
		}
		else
		{
			position=(position_high+position_low)/2;
		}
	}

	return position;
}