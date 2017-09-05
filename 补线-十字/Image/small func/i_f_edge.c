#include "common.h"
#include "_small_func_.h"




/**********************************************************************************************
名    称：                    水平从中间同时往两边“找跳变沿”函数

功能描述：水平从中间同时往两边找跳变沿
入口参数：一维数组首地址Line[]、范围（最大输入93即188个像素全行扫描）
出口参数：跳变沿信息
备    注：这个函数只是找跳变沿 并不会判断是上升沿还是下降沿
**********************************************************************************************/
void f_edge_sp_czj(uint8 Line[],uint8 range,uint8 *flag_l,uint8 *Left,uint8 *flag_r,uint8 *Right)
{
  uint8 i;
  *flag_l=0;
  *flag_r=0;
  //最中间的两个像素点 93 94（下标从0开始）
  //注：忽略了93和94之间的跳变关系
  for(i=1;i<=range;i++)//若range==186/2==93 即全部扫描
  {
    //往左边扫
    if( *flag_l==0 )
    {
      if( Line[93-i]==Line[93-i+1] )
      {
      }
      else
      {
        *Left=(93-i);
        *flag_l=1;
      }
    }
    //往右边扫
    if( *flag_r==0 )
    {
      if( Line[94+i-1]==Line[94+i] )
      {
      }
      else
      {
        *Right=(94+i);
        *flag_r=1;
      }
    }
    //判断是否还需要再执行循环
    if( (*flag_l==1)&&(*flag_r==1) )
    {
      break;
    }
  }
  if(*flag_l==0)
    *Left =93-range;
  if(*flag_r==0)
    *Right=94+range;
}
/**********************************************************************************************
名    称：                水平从某点开始同时往两边“找上升沿”函数

功能描述：水平从某点开始往两边找上升沿
入口参数：一维数组、起始的点、范围
出口参数：是否存在标志位、位置
备    注：仅当返回的标志位为1时 位置才会返回 不然*posi指向的变量不会被更新
**********************************************************************************************/
uint8 f_Sedge_sp_cmd(uint8 Line[],uint8 start_p,uint8 range,uint8 *posi)
{
  uint8 i;
  uint8 flag_l=0;//往左找到上升沿标志位
  uint8 flag_r=0;//往右找到上升沿标志位
  uint8 tmp_l;//被比较的左侧像素下标
  uint8 tmp_r;//被比较的右侧像素下标
  
  for(i=1;i<=range;i++)
  {
    //往左边扫
    if( flag_l==0 )
    {
      //限幅 防止不在0~187范围内
      tmp_l=start_p-i;
      tmp_r=start_p-i+1;
      if( (tmp_l<=0)||(tmp_l>230) )
        tmp_l=0;
      if(tmp_r>187)
        tmp_r=187;
      
      //找上升沿
      if( Line[tmp_l]==Line[tmp_r] )
      {
      }
      else
      {
        if( Line[tmp_l]<Line[tmp_r] )
        {
          flag_l=1;
          *posi=tmp_l;
        }
        else
        {
          //说明是下降沿 我们要找的是上升沿 故不要
        }
      }
    }
    //往右边扫
    if( flag_r==0 )
    {
      //限幅 防止不在0~187范围内
      tmp_l=start_p+i-1;
      tmp_r=start_p+i;
      if( (tmp_l<=0)||(tmp_l>230) )
        tmp_l=0;
      if(tmp_r>187)
        tmp_r=187;
      
      //找上升沿
      if( Line[tmp_l]==Line[tmp_r] )
      {
      }
      else
      {
        if( Line[tmp_l]<Line[tmp_r] )
        {
          flag_r=1;
          *posi=tmp_l;
        }
        else
        {
          //说明是下降沿 我们要找的是上升沿 故不要
        }
      }
    }
    //判断是否还需要再执行循环
    if( (flag_l==1)||(flag_r==1) )
    {
      break;
    }
  }
  
  //返回是否找到上升沿标志位
  if( (flag_l==1)||(flag_r==1) ) 
    return 1;
  else
    return 0;
}
/**********************************************************************************************
名    称：                    水平从某点同时往两边“找下降沿”函数

功能描述：水平从某点往两边找下降沿
入口参数：一维数组、开始的点、范围
出口参数：是否存在标志位、位置
备    注：仅当返回的标志位为1时 位置才会更新
**********************************************************************************************/
uint8 f_Xedge_sp_cmd(uint8 Line[],uint8 start_p,uint8 range,uint8 *posi)
{
  uint8 i;
  uint8 flag_l=0;//往左找到上升沿标志位
  uint8 flag_r=0;//往右找到上升沿标志位
  uint8 tmp_l;//被比较的左侧像素下标
  uint8 tmp_r;//被比较的右侧像素下标
  
  for(i=1;i<=range;i++)
  {
    //往左边扫
    if( flag_l==0 )
    {
      //限幅 防止不在0~187范围内
      tmp_l=start_p-i;
      tmp_r=start_p-i+1;
      if( (tmp_l<=0)||(tmp_l>230) )
        tmp_l=0;
      if(tmp_r>187)
        tmp_r=187;
      
      //找上升沿
      if( Line[tmp_l]==Line[tmp_r] )
      {
      }
      else
      {
        if( Line[tmp_l]>Line[tmp_r] )
        {
          flag_l=1;
          *posi=tmp_r;
        }
        else
        {
          //说明是上升沿 我们要找的是下降沿 故不要
        }
      }
    }
    //往右边扫
    if( flag_r==0 )
    {
      //限幅 防止不在0~187范围内
      tmp_l=start_p+i-1;
      tmp_r=start_p+i;
      if( (tmp_l<=0)||(tmp_l>230) )
        tmp_l=0;
      if(tmp_r>187)
        tmp_r=187;
      
      //找上升沿
      if( Line[tmp_l]==Line[tmp_r] )
      {
      }
      else
      {
        if( Line[tmp_l]>Line[tmp_r] )
        {
          flag_r=1;
          *posi=tmp_r;
        }
        else
        {
          //说明是上升沿 我们要找的是下降沿 故不要
        }
      }
    }
    //判断是否还需要再执行循环
    if( (flag_l==1)||(flag_r==1) )
    {
      break;
    }
  }
  
  //返回是否找到上升沿标志位
  if( (flag_l==1)||(flag_r==1) ) 
    return 1;
  else
    return 0;
}
/**********************************************************************************************
                                从最中间底部向上垂直扫描一列
**********************************************************************************************/
uint8 Scan_Edge_ChuiZhi(uint8 Line[][IMG_W_USED],uint8 start,uint8 final)
{
  uint8 flag=0;
  uint8 i;
  for(i=start;i>=final;i--)
  {
    //从第 93(94也可以) 列处 由下往上 垂直扫描跳变沿
    if( Line[i][93]==Line[i+1][93] )
    {
      //没扫到跳变沿
    }
    else
    {
      //扫到了跳变沿
      flag=1;
      break;//终止整个for循环
    }
  }
  return flag;
}
/**********************************************************************************************
                            遍历整个一行 返回所有跳变沿 下降沿
**********************************************************************************************/
n_edge_t f_edge_all(uint8 line_bin[],edge_t edge[])
{
  n_edge_t n_edge;//函数返回值
  
  n_edge.all=0;//所有跳变沿个数清零
  n_edge.s_n=0;//上升沿个数清零
  n_edge.x_n=0;//下降沿个数清零
  
  for(uint8 j=0 ; j<(IMG_W_USED-1) ; j++ )
  {
    if( line_bin[j]!=line_bin[j+1] )
    {
      n_edge.all++;
      if( line_bin[j]<line_bin[j+1] )//黑0 < 白255 上升沿
      {
        edge[n_edge.all].sx = 1;
        edge[n_edge.all].wz = j;
        
        n_edge.s_n++;
      }
      else                           //白255 > 黑0 下降沿
      {
        edge[n_edge.all].sx = -1;
        edge[n_edge.all].wz = j+1;
        
        n_edge.x_n++;
      }
    }
  }
  
  return n_edge;
}
/**********************************************************************************************
名    称：  输入正方形底部左侧坐标及正方形宽度 判断其内是否存在“上左拐点”或“上右拐点”

功能描述：
入口参数：
出口参数：
备    注：0不存在拐点 1存在左上拐点 2存在右上拐点
**********************************************************************************************/
uint8 sr_zfxdbzb_pdyw_Sgd(uint8 img[][IMG_W_YS],zb_imgarr_t XiaZuo,uint8 zfx_kd)
{
  uint8 exist_flag=0;
  zb_imgarr_t XiaYou;           //正方形下右点坐标
  zb_imgarr_t ShangZuo,ShangYou;//正方形上左点坐标 正方形上右点坐标
  zb_imgarr_t Mid;              //正方形中心点
  
  //计算正方形下右侧坐标
  XiaYou.i=XiaZuo.i;
  XiaYou.j=XiaZuo.j+zfx_kd-1;
  
  //计算正方形上左侧坐标
  ShangZuo.j=XiaZuo.j;
  ShangZuo.i=XiaZuo.i-zfx_kd+1;
  
  //计算正方形上右侧坐标
  ShangYou.i=ShangZuo.i;
  ShangYou.j=XiaYou.j;
  
  //计算正方形中心点
  Mid.i=(XiaZuo.i+ShangZuo.i)/2;
  Mid.j=(XiaZuo.j+XiaYou.j)/2;
  
  //进行首次判断
  if( (img[XiaZuo.i][XiaZuo.j]==255)&&(img[XiaYou.i][XiaYou.j]==255) )
  {
    if( (img[ShangZuo.i][ShangZuo.j]==0)&&(img[ShangYou.i][ShangYou.j]==255) )
    {
      exist_flag=1;//存在“上左拐点”? 
    }
    if( (img[ShangZuo.i][ShangZuo.j]==255)&&(img[ShangYou.i][ShangYou.j]==0) )
    {
      exist_flag=2;//存在“上右拐点”? 
    }
  }

  //再次判断 防止误判
  if( (exist_flag==1)||(exist_flag==2) )
  {
    exist_flag=0;//清0 即不存在 再次判断
    
    //根据计算得出的正方形中心点 计算2倍原边长的新正方形
    XiaZuo.i=Mid.i+(zfx_kd/2);
    XiaZuo.j=Mid.j-(zfx_kd/2);
    
    XiaYou.i=Mid.i+(zfx_kd/2);
    XiaYou.j=Mid.j+(zfx_kd/2);
    
    ShangYou.i=Mid.i-(zfx_kd/2);
    ShangYou.j=Mid.j+(zfx_kd/2);
    
    ShangZuo.i=Mid.i-(zfx_kd/2);
    ShangZuo.j=Mid.j-(zfx_kd/2);
    if( (img[ShangZuo.i][ShangZuo.j]==0)&&(img[ShangYou.i][ShangYou.j]==255) )
    {
      exist_flag=1;//确实存在“上左拐点”
    }
    if( (img[ShangZuo.i][ShangZuo.j]==255)&&(img[ShangYou.i][ShangYou.j]==0) )
    {
      exist_flag=2;//确实存在“上右拐点”
    }
  }
  
//  extern uint8 image_binarized[IMG_H_USED][IMG_W_USED];
//  liang_dian_lian_xian(image_binarized,200,XiaZuo,XiaYou);
//  liang_dian_lian_xian(image_binarized,200,XiaYou,ShangYou);
//  liang_dian_lian_xian(image_binarized,200,ShangYou,ShangZuo);
//  liang_dian_lian_xian(image_binarized,200,ShangZuo,XiaZuo);
  
  return exist_flag;
}
/**********************************************************************************************
名    称：                         查找上左右拐点函数

功能描述：输入最底部左右边界j值 输出上左右拐点存在性及存在时的位置
入口参数：最底部左右边界j值
出口参数：上左右拐点存在性及存在时的位置
备    注：
**********************************************************************************************/
void f_S_guaidian(uint8 img[][IMG_W_YS],uint8 bj_l,uint8 bj_r,uint8 *flag_l,zb_imgarr_t *gd_l,uint8 *flag_r,zb_imgarr_t *gd_r)
{
  //bj_l      bj_r       最底部左右边界的j值
  //*flag_l   *flag_r    上左右拐点是否查找到标志位 0未找到 1找到
  //*gp_l     *gd_r      拐点的位置
  
  *flag_l=0;
  *flag_r=0;
  
  uint8 bj_kd; //最底部左右边界宽度
  uint8 zfx_kd;//正方形宽度
  
  bj_kd = bj_r - bj_l + 1;                              //求最底部边界宽度
  if( (bj_kd%12)==0 ){zfx_kd=bj_kd/12;}                 //求正方形宽度
  else               {zfx_kd=( bj_kd-(bj_kd%12) )/12+1;}//这样求不论zfx_kd为多少都是12等分
  
  uint8 zfx_db_i=79;       //正方形底部i值 第一次=79 下一次=上一次-zfx_kd
  zb_imgarr_t I_zfx_zx[12];//正方形左下角坐标值 12等份
  
  uint8 ct_zfx;//计数正方形个数 12等份
 
  for(ct_zfx=0;ct_zfx<12;ct_zfx++)
  {
    I_zfx_zx[ct_zfx].j=bj_l+zfx_kd*ct_zfx;
  }
  
  uint8 do_while_over_flag=0;
  
  do
  {
    for(ct_zfx=1;ct_zfx<=12;ct_zfx++)//每一行共判断12个正方形
    {
      I_zfx_zx[ct_zfx-1].i=zfx_db_i;
      if( sr_zfxdbzb_pdyw_Sgd(img,I_zfx_zx[ct_zfx-1],zfx_kd)==1 )//上左拐点？
      {
        *flag_l=1;
        (*gd_l).i=I_zfx_zx[ct_zfx-1].i-zfx_kd+1;//返回正方形左上顶点
        (*gd_l).j=I_zfx_zx[ct_zfx-1].j;
      }
      if( sr_zfxdbzb_pdyw_Sgd(img,I_zfx_zx[ct_zfx-1],zfx_kd)==2 )//上右拐点？
      {
        *flag_r=1;
        (*gd_r).i=I_zfx_zx[ct_zfx-1].i-zfx_kd+1;//返回正方形右上顶点
        (*gd_r).j=I_zfx_zx[ct_zfx-1].j+zfx_kd-1;
      }
      if( (*flag_l==1)&&(*flag_r==1) )
      {
        do_while_over_flag=1;
        break;//提前终止内层for循环
      }
    }
    
    if(do_while_over_flag==1) break;//提前终止外层do-while循环 即整个循环
    zfx_db_i=zfx_db_i-zfx_kd;
  }
  while(zfx_db_i>10);
}