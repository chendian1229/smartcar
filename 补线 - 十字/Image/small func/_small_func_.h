#ifndef __SMALL_FUNC_H__
#define __SMALL_FUNC_H__


/*****************************************图像操作区*****************************************/
/*点间运算 （i_djys.c）*/
uint8 pd_ld_sf_ch_img(zb_imgarr_t I_1,zb_imgarr_t I_2);
zb_imgarr_t zhong_dian_img(zb_imgarr_t I_1,zb_imgarr_t I_2);
/*二值化（i_er_zhi_hua.c）*/
void ChouQu_ErZhi(uint8 gray[][IMG_W_YS],uint8 yz,uint8 binarized[][IMG_W_USED]);
/*图像上找跳变沿（i_f_edge.c）*/
void f_edge_sp_czj(uint8 Line[],uint8 range,uint8 *flag_l,uint8 *Left,uint8 *flag_r,uint8 *Right);
uint8 f_Sedge_sp_cmd(uint8 Line[],uint8 ponit_j,uint8 range,uint8 *posi);
uint8 f_Xedge_sp_cmd(uint8 Line[],uint8 ponit_j,uint8 range,uint8 *posi);
uint8 Scan_Edge_ChuiZhi(uint8 Line[][IMG_W_USED],uint8 start,uint8 final);
uint8 sr_zfxdbzb_pdyw_Sgd(uint8 img[][IMG_W_YS],zb_imgarr_t XiaZuo,uint8 zfx_kd);
void f_S_guaidian(uint8 img[][IMG_W_YS],uint8 bj_l,uint8 bj_r,uint8 *flag_l,zb_imgarr_t *gd_l,uint8 *flag_r,zb_imgarr_t *gd_r);
n_edge_t f_edge_all(uint8 line_bin[],edge_t edge[]);
/*图像上画线（i_hua_xian.c）*/
void liang_dian_lian_xian(uint8 img[][IMG_W_USED],uint8 gray_v,zb_imgarr_t I_1,zb_imgarr_t I_2);
void xiang_shang_hua_yan_shen_xian(uint8 img[][IMG_W_USED],uint8 gray_v,zb_imgarr_t I_1,zb_imgarr_t I_2,uint8 hight_math);
/*阈值（i_yu_zhi.c）*/
uint8 OTSU(uint8 image[][IMG_W_USED]);
/*****************************************数学运算区*****************************************/
/*点间运算 （m_djys.c）*/
uint8 pd_ld_sf_ch_math(zb_math_t M_1,zb_math_t M_2);
zb_math_t zhong_dian_math(zb_math_t M_1,zb_math_t M_2);
/*斜率（m_xie_lv.c）*/
slope_t calcu_slope(zb_math_t M_1,zb_math_t M_2);//★输入的两点不能重合★
/*坐标（m_zuo_biao.c）*/
uint8 x_frm_j(uint8 j);
uint8 y_frm_i(uint8 i);
uint8 j_frm_x(uint8 x);
uint8 i_frm_y(uint8 y);
zb_math_t covert_zb(zb_imgarr_t source);
zb_imgarr_t covert_fan_zb(zb_math_t source);


#endif