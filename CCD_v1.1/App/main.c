#include "common.h"
#include "include.h"


uint8 Pixel[128];                  //128个像素点相应的AD读取数
uint8 flag=0;                      //CCD前两百次宽度测量
uint8 KuanDu;
uint8 ccd_position;



//函数声明区
void PIT0_ISR();                  //PIT0定时器中断服务函数




/**********************************************************************************************
					主函数Main()											        main函数
**********************************************************************************************/
void main(void)
{
	//CCD初始化部分
	uint8 *pixel_pt;                           //定义指向像素数组名的指针
	pixel_pt = Pixel;                          //指针指向像素名数组
	uart_init (UART0 , 115200);                //初始化UART0，输出脚PTD7，输入脚PTD6，串口频率 115200
	CCD_init() ;                               //CCD传感器初始化
	uint8 i =0 ;
	for(i=0; i<128+10; i++)                    //初始化数组，使数组全部置0
	{
		*pixel_pt++ = 0;
	}


	//ftm_pwm_init(FTM0, FTM_CH0,10*1000,0);

	//中断函数初始化部分
	pit_init_ms(PIT0, 5);                     //初始化PIT0，定时时间为： 5ms
	set_vector_handler(PIT0_VECTORn,PIT0_ISR);//设置PIT0的中断服务函数为PIT0_ISR
	enable_irq (PIT0_IRQn);                   //使能PIT0中断


	while(1)
	{
		SendImageData(Pixel);//发送数据到串口 上位机显示
		printf("%d\t%d\n",KuanDu,ccd_position);
		DELAY_MS(100);
	}
}
/**********************************************************************************************
																  定时器中断函数 1ms
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
	static uint8 T_ccd = 1;
	if(T_ccd==2)
	{
		T_ccd=0;
		//5ms执行一次
		ImageCapture(Pixel);//曝光1次，采集一次CCD数据
//		ErZhiHua(Pixel);
//		if(flag<200)
//		{
//			KuanDu=JiSuanKuanDu(Pixel);//这段代码仅在前十次曝光时运行
//			flag++;
//		}
//		ccd_position = ZhaoZhongJian_From_ErZhiHuaHouDe(Pixel,KuanDu);
//		Pixel[ccd_position]=150;
	}

	static uint8 T_zl=1;
	if(T_zl==2)
	{
		T_zl=0;
		//10ms执行一次
		//直立程序
	}

	T_ccd++;
	T_zl++;
}