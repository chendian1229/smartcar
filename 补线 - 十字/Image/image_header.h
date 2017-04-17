#ifndef __IMAGE_HEADER__
#define __IMAGE_HEADER__


#define IMAGE_ARR_HEIGHT 80  //i:0~79
#define IMAGE_ARR_WIDTH  188 //j:0~187


int8 Calculate_Angle(uint8 i_1,uint8 j_1,uint8 i_2,uint8 j_2);
void ChouQu_ErZhi(uint8 gray[][188],uint8 binarized[][188]);
uint8 OTSU(uint8 image[][188]);
void Scan_Edge_ShuiPing(uint8 Line[],uint8 *flag_l,uint8 *Left,uint8 *flag_r,uint8 *Right);
uint8 Scan_Edge_ChuiZhi(uint8 Line[][188],uint8 start,uint8 final);
zb_math_t covert_zb(zb_imgarr_t source);
zb_imgarr_t covert_fan_zb(zb_math_t source);
void lian_xian(uint8 img[][IMAGE_ARR_WIDTH],uint8 color,zb_imgarr p_a,zb_imgarr p_b);
zb_imgarr zhong_jian_dian(zb_imgarr p1,zb_imgarr p2);
edge_n_t f_edge_all(uint8 line_bin[],edge_t edge[]);

#endif