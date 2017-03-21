#ifndef __IMAGE_PROCESSING_H__
#define __IMAGE_PROCESSING_H__

#define IMAGE_ARR_HEIGHT 80  //i:0~79
#define IMAGE_ARR_WIDTH  188 //j:0~187

uint8 OTSU(uint8 image_line1[],uint8 image_line2[],uint8 image_line3[],uint8 image_line4[]);
void ChouQu_ErZhi(uint8 gray[][188],uint8 binarized[][188]);
void Scan_Edge_ShuiPing(uint8 Line[],uint8 *flag_l,uint8 *Left,uint8 *flag_r,uint8 *Right);
uint8 Scan_Edge_ChuiZhi(uint8 Line[][188],uint8 start,uint8 final);

int8 Calculate_Angle(uint8 i_1,uint8 j_1,uint8 i_2,uint8 j_2);
void Connect_Line_On_ImageArr(uint8 img_arr[][IMAGE_ARR_WIDTH],uint8 i_1,uint8 j_1,uint8 i_2,uint8 j_2);


#endif