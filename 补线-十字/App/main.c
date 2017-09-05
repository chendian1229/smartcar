#include "common.h"
#include "include.h"

uint8 YuZhi;
uint8 image_gray[IMG_H_YS][IMG_W_YS];         //原始灰度图像         高:120 宽:188
uint8 image_binarized[IMG_H_USED][IMG_W_USED];//抽取及二值化后的图像 高:120 宽:188

uint8 i,j;

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
  camera_init( (uint8*)image_gray );//一维数组首地址<--二维数组首地址(强制转化)
  //摄像头中断服务函数配置
  void PORTA_IRQHandler();
  void DMA0_IRQHandler();
  set_vector_handler(PORTA_VECTORn,PORTA_IRQHandler);//设置 PORTA 的中断服务函数为 PORTA_IRQHandler
  set_vector_handler(DMA0_VECTORn,DMA0_IRQHandler);  //设置 DMA0 的中断服务函数为 PORTA_IRQHandler
  //采集一幅图像 求阈值
  camera_get_img();
  YuZhi = OTSU(image_gray);
  while(1)
  {
    camera_get_img();
    ChouQu_ErZhi(image_gray,YuZhi,image_binarized);
    
    uint16 bj_j_start_left; //起始左边界点 j 值（前5行平均值）
    uint16 bj_j_start_right;//起始右边界点 j 值（前5行平均值）
    
    uint8 n_bj_l=0;//搜索到的左边点界个数
    uint8 n_bj_r=0;//搜索到的右边点界个数
    zb_imgarr_t bj_left[IMG_H_USED]; //左边界点数组
    zb_imgarr_t bj_right[IMG_H_USED];//有边界点数组
    
    uint8 nomeaning_l;
    uint8 nomeaning_r;
    uint8 over_flag_l=0;
    uint8 over_flag_r=0;
    uint8 left_now,right_now;//本次扫描所使用左右边界点的j值
    uint8 left_last,right_last;//下次扫描所使用左右边界点的j值（即本次结果）
    
    zb_imgarr_t mid_point[10];//分9等份 共计10个 中间点
    
    //先从最下面5行找边界 并求取平均值
    bj_j_start_left=0;
    bj_j_start_right=0;
    
    for(i=79;i>=75;i--)//79 78 77 76 75 共计5行
    {
      //需要注意 程序默认最底部5行都是可以找到边界点的
      f_edge_sp_czj( image_binarized[i],93,&nomeaning_l,&(bj_left[n_bj_l].j),&nomeaning_r,&(bj_right[n_bj_r].j) );
      bj_left[n_bj_l].i =i;
      bj_right[n_bj_r].i=i;
      bj_j_start_left  += bj_left[n_bj_l].j;
      bj_j_start_right += bj_right[n_bj_r].j;
      
      n_bj_l++;
      n_bj_r++;
    }
    bj_j_start_left /=5;//将平均值作为以后的起始边界j值
    bj_j_start_right/=5;//
    
    //最5行 指示线
//liang_dian_lian_xian(image_binarized,128,bj_left[n_bj_l-1],bj_right[n_bj_r-1]);
    
    //前5行扫完后 往后每行在上一行的左右边界附近查找本行边界 以节省时间 同时找的更远
    left_last =bj_j_start_left;
    right_last=bj_j_start_right;
    for(i=74;;i--)
    {
      if(i==255) break;//（uint8型）0继续减下去会变成255
      if(over_flag_l==0)
      {
        if( f_Sedge_sp_cmd(image_binarized[i],left_last,15,&left_now)==1 )
        {
          bj_left[n_bj_l].i=i;
          bj_left[n_bj_l].j=left_now;
          
          left_last=left_now;
          n_bj_l++;
        }
        else
        {
          over_flag_l=1;
        }
      }
      
      if(over_flag_r==0)
      {
        if( f_Xedge_sp_cmd(image_binarized[i],right_last,15,&right_now)==1 )
        {
          bj_right[n_bj_r].i=i;
          bj_right[n_bj_r].j=right_now;
          
          right_last=right_now;
          n_bj_r++;
        }
        else
        {
          over_flag_r=1;
        }
      }
      
      if( (over_flag_l==1)&&(over_flag_r==1) )
      {
        break;
      }
      
    }
    //取10个均匀的中间点
    mid_point[0]  = zhong_dian_img( bj_left[ 0*n_bj_l/9 ],bj_right[ 0*n_bj_r/9 ] );
    mid_point[1]  = zhong_dian_img( bj_left[ 1*n_bj_l/9 -1],bj_right[ 1*n_bj_r/9 -1] );
    mid_point[2]  = zhong_dian_img( bj_left[ 2*n_bj_l/9 -1],bj_right[ 2*n_bj_r/9 -1] );
    mid_point[3]  = zhong_dian_img( bj_left[ 3*n_bj_l/9 -1],bj_right[ 3*n_bj_r/9 -1] );
    mid_point[4]  = zhong_dian_img( bj_left[ 4*n_bj_l/9 -1],bj_right[ 4*n_bj_r/9 -1] );
    mid_point[5]  = zhong_dian_img( bj_left[ 5*n_bj_l/9 -1],bj_right[ 5*n_bj_r/9 -1] );
    mid_point[6]  = zhong_dian_img( bj_left[ 6*n_bj_l/9 -1],bj_right[ 6*n_bj_r/9 -1] );
    mid_point[7]  = zhong_dian_img( bj_left[ 7*n_bj_l/9 -1],bj_right[ 7*n_bj_r/9 -1] );
    mid_point[8]  = zhong_dian_img( bj_left[ 8*n_bj_l/9 -1],bj_right[ 8*n_bj_r/9 -1] );
    mid_point[9]  = zhong_dian_img( bj_left[ 9*n_bj_l/9 -1],bj_right[ 9*n_bj_r/9 -1] );
    //依次连接
//    liang_dian_lian_xian(image_binarized,128,mid_point[0],mid_point[1]);
//    liang_dian_lian_xian(image_binarized,128,mid_point[1],mid_point[2]);
//    liang_dian_lian_xian(image_binarized,128,mid_point[2],mid_point[3]);
//    liang_dian_lian_xian(image_binarized,128,mid_point[3],mid_point[4]);
//    liang_dian_lian_xian(image_binarized,128,mid_point[4],mid_point[5]);
//    liang_dian_lian_xian(image_binarized,128,mid_point[5],mid_point[6]);
//    liang_dian_lian_xian(image_binarized,128,mid_point[6],mid_point[7]);
//    liang_dian_lian_xian(image_binarized,128,mid_point[7],mid_point[8]);
//    liang_dian_lian_xian(image_binarized,128,mid_point[8],mid_point[9]);
    //顶行指示线
    //liang_dian_lian_xian(image_binarized,128,bj_left[n_bj_l-1],bj_right[n_bj_r-1]);
    //十字延伸线
    uint8 gd_flag_l;                    //十字左拐点
    uint8 gd_flag_r;                    //十字右拐点
    zb_imgarr_t gd_l,gd_r;              //
    uint8 lx;
    uint8 rx;
    lx=bj_left[0].j; //最小34
    rx=bj_right[0].j;//最大154
    
    f_S_guaidian(image_binarized,38,150,&gd_flag_l,&gd_l,&gd_flag_r,&gd_r);
    
    liang_dian_lian_xian(image_binarized,128,bj_left[0],gd_l);
    liang_dian_lian_xian(image_binarized,128,bj_right[0],gd_r);
//    xiang_shang_hua_yan_shen_xian(image_binarized,128,bj_left[0],bj_left[n_bj_l-3],79);
//    xiang_shang_hua_yan_shen_xian(image_binarized,128,bj_right[0],bj_right[n_bj_r-3],79);
    
    LCD_Img_gray_Z(imgsite,lcdsize,(uint8*)image_binarized,imgsize);
    vcan_sendimg(image_binarized,IMG_W_USED*IMG_H_USED);
  }
}
/**********************************************************************************************
                         摄像头 PORTA中断服务函数 及 DMA0中断服务函数
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
void DMA0_IRQHandler()
{
    camera_dma();
}