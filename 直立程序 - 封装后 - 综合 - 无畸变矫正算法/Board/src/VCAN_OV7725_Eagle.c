/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,ɽ��Ƽ�
 *     All rights reserved.
 *     �������ۣ�ɽ����̳ http://www.vcan123.com
 *
 *     ��ע�������⣬�����������ݰ�Ȩ����ɽ��Ƽ����У�δ����������������ҵ��;��
 *     �޸�����ʱ���뱣��ɽ��Ƽ��İ�Ȩ������
 *
 * @file       VCAN_OV7725_Eagle.c
 * @brief      ӥ��ov7725��������
 * @author     ɽ��Ƽ�
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

volatile IMG_STATUS_e      ov7725_eagle_img_flag = IMG_FINISH;   //ͼ��״̬

//�ڲ���������
static void ov7725_eagle_reg_init(void);
static void ov7725_eagle_port_init();


/*!
 *  @brief      ӥ��ov7725��ʼ��
 *  @since      v5.0
 */
uint8 ov7725_eagle_init(uint8 *imgaddr)
{
    ov7725_eagle_img_buff = imgaddr;
    ov7725_eagle_reg_init();//�����...
    ov7725_eagle_port_init();
    return 0;
}

/*!
 *  @brief      ӥ��ov7725�ܽų�ʼ�����ڲ����ã�
 *  @since      v5.0
 */
void ov7725_eagle_port_init()
{
    //DMAͨ��0��ʼ����PTA27����Դ(Ĭ��������)��Դ��ַΪPTB_B0_IN��Ŀ�ĵ�ַΪ��IMG_BUFF��ÿ�δ���1Byte
    dma_portx2buff_init(CAMERA_DMA_CH, (void *)&PTB_B0_IN, (void *)ov7725_eagle_img_buff, PTA27, DMA_BYTE1, CAMERA_DMA_NUM, DADDR_KEEPON);

    DMA_DIS(CAMERA_DMA_CH);
    disable_irq(PORTA_IRQn);                        //�ر�PTA���ж�
    DMA_IRQ_CLEAN(CAMERA_DMA_CH);                   //���ͨ�������жϱ�־λ
    DMA_IRQ_EN(CAMERA_DMA_CH);

    port_init(PTA27, ALT1 | DMA_FALLING | PULLDOWN );         //PCLK

    port_init(PTA29, ALT1 | IRQ_RISING  | PULLDOWN | PF);     //���жϣ��������Ͻ��ش����жϣ����˲�

}

/*!
 *  @brief      ӥ��ov7725���жϷ�����
 *  @since      v5.0
 */
void ov7725_eagle_vsync(void)
{

    //���ж���Ҫ�ж��ǳ��������ǳ���ʼ
    if(ov7725_eagle_img_flag == IMG_START)                   //��Ҫ��ʼ�ɼ�ͼ��
    {
        ov7725_eagle_img_flag = IMG_GATHER;                  //���ͼ��ɼ���
        disable_irq(PORTA_IRQn);

#if 1

        PORTA_ISFR = 1 <<  PT27;            //���PCLK��־λ

        DMA_EN(CAMERA_DMA_CH);                  //ʹ��ͨ��CHn Ӳ������
        PORTA_ISFR = 1 <<  PT27;            //���PCLK��־λ
        DMA_DADDR(CAMERA_DMA_CH) = (uint32)ov7725_eagle_img_buff;    //�ָ���ַ

#else
        PORTA_ISFR = 1 <<  PT27;            //���PCLK��־λ
        dma_repeat(CAMERA_DMA_CH, (void *)&PTB_B0_IN, (void *)ov7725_eagle_img_buff,CAMERA_DMA_NUM);
#endif
    }
    else                                        //ͼ��ɼ�����
    {
        disable_irq(PORTA_IRQn);                        //�ر�PTA���ж�
        ov7725_eagle_img_flag = IMG_FAIL;                    //���ͼ��ɼ�ʧ��
    }
}

/*!
 *  @brief      ӥ��ov7725 DMA�жϷ�����
 *  @since      v5.0
 */
void ov7725_eagle_dma()
{
    ov7725_eagle_img_flag = IMG_FINISH ;
    DMA_IRQ_CLEAN(CAMERA_DMA_CH);           //���ͨ�������жϱ�־λ
}

/*!
 *  @brief      ӥ��ov7725�ɼ�ͼ�񣨲ɼ��������ݴ洢�� ��ʼ��ʱ���õĵ�ַ�ϣ�
 *  @since      v5.0
 */
void ov7725_eagle_get_img()
{
    ov7725_eagle_img_flag = IMG_START;                   //��ʼ�ɼ�ͼ��
    PORTA_ISFR = ~0;                        //д1���жϱ�־λ(����ģ���Ȼ�ص���һ���жϾ����ϴ����ж�)
    enable_irq(PORTA_IRQn);                         //����PTA���ж�
    while(ov7725_eagle_img_flag != IMG_FINISH)           //�ȴ�ͼ��ɼ����
    {
        if(ov7725_eagle_img_flag == IMG_FAIL)            //����ͼ��ɼ����������¿�ʼ�ɼ�
        {
            ov7725_eagle_img_flag = IMG_START;           //��ʼ�ɼ�ͼ��
            PORTA_ISFR = ~0;                //д1���жϱ�־λ(����ģ���Ȼ�ص���һ���жϾ����ϴ����ж�)
            enable_irq(PORTA_IRQn);                 //����PTA���ж�
        }
    }
}


/*OV7725��ʼ�����ñ�*/
reg_s ov7725_eagle_reg[] =
{
    //�Ĵ������Ĵ���ֵ��
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

uint8 ov7725_eagle_cfgnum = ARR_SIZE( ov7725_eagle_reg ) ; /*�ṹ�������Ա��Ŀ*/


/*!
 *  @brief      ӥ��ov7725�Ĵ��� ��ʼ��
 *  @return     ��ʼ�������0��ʾʧ�ܣ�1��ʾ�ɹ���
 *  @since      v5.0
 */
void ov7725_eagle_reg_init(void)
{
  //���ò��� ������ο�ʹ���ֲ�
  uint16 light;
  //����ͷ��������
  uint8 MT9V032_CFG[8];
  
  
  DELAY_MS(50);           //��ʱ�Ա�֤����ͷ�����51����ϵ��ʼ��
  uart_init(UART3,115200);//��ʼ������ ��������ͷ
  
  //֡ͷ
  MT9V032_CFG[0] = 0xFF;//֡ͷ
  
  //����λ
  //������ο�ʹ���ֲ�
  MT9V032_CFG[1] = 0x00;
  
  //�ֱ���ѡ��λ �ֱ����л��� ��öϵ�һ��ϵͳ
  //�����ֱ��ʾ��忴˵���� �����г���188��K60�޷��ɼ� �ṩ�ĸ���ķֱ����Ǹ�STM32��������ͷ�ӿڵ��û�
#define ROW 120
  switch(ROW)
  {
  case 480:MT9V032_CFG[2] = 8;break;
  case 240:MT9V032_CFG[2] = 9;break;
  case 120:MT9V032_CFG[2] = 10;break;
  default :MT9V032_CFG[2] = 10;break;
  }
#undef ROW
  
  //����ͼ��֡�� ������ͬ�����õķ�ΧҲ��ͬ ��Χ���� 120�е�ʱ����1-200 240�е�ʱ����1-132 480�е�ʱ����1-66
  MT9V032_CFG[3] = 50;
  
  //�ع�ʱ��Խ��ͼ��Խ�� ��������ع�ʱ���ܵ�fps��ֱ��ʵĹ�ͬӰ�� ���ﲻ�������巶Χ ����ֱ�Ӱ��ع�����Ϊ1000
  //����ͷ�ϵ�51�յ�֮�����ݷֱ��ʼ�FPS��������ع�ʱ�� Ȼ����ع�ʱ������Ϊ����ع�ʱ��
  //���һ᷵�ص�ǰ���õ�����ع�ʱ�� �����Ϳ���֪�����������õ��ع�ʱ���Ƕ�����
  //Ȼ�����ͼ����� �Ϳ������������ع�ֵ�Ļ�������Ӧ����
  light = 1000;
  MT9V032_CFG[4] = light>>8;    //�ع�ʱ��߰�λ
  MT9V032_CFG[5] = (uint8)light;//�ع�ʱ��Ͱ�λ
  
  //����Ϊ0��ʾ�ر��Զ��ع⣬����1-63�������Զ��ع⣬���õ�Խ��ͼ���Խ��  ���鿪��������ܣ�������Ч��Ӧ���ֳ���
  MT9V032_CFG[6] = 0;
  
  //֡β
  MT9V032_CFG[7] = 0x5A;
  
  //ͨ�����ڷ������ò���
  uart_putbuff(UART3,MT9V032_CFG,8);
  DELAY_MS(50);//��ʱ�Ա����ϸ���������51�Ѿ��ɹ�д�뵽����ͷ
  
  
  
  
  //����������������ͷ���� ��������ع�ʱ���ǲ�һ����
  MT9V032_CFG[0] = 0xFF;  //֡ͷ
  MT9V032_CFG[1] = 0x02;  //����λ  
  MT9V032_CFG[2] = 0;     //���ã�����Ϊ0
  MT9V032_CFG[3] = 0;     //���ã�����Ϊ0
  MT9V032_CFG[4] = 0;     //���ã�����Ϊ0
  MT9V032_CFG[5] = 32;    //���ȵȼ�ѡ�� ���ȵȼ� 1 - 64
  MT9V032_CFG[6] = 0x35;
  MT9V032_CFG[7] = 0x5A;  //֡β
  uart_putbuff(UART3,MT9V032_CFG,8);
  DELAY_MS(50);//��ʱ�Ա����ϸ���������51�Ѿ��ɹ�д�뵽����ͷ
}


