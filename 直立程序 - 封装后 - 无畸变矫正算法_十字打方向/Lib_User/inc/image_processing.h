#ifndef __IMAGE_PROCESSING_H__
#define __IMAGE_PROCESSING_H__


void ChouQu_ErZhi(uint8 gray[][188],uint8 binarized[][94]);
void Scan_Edge_ShuiPing(uint8 Line[],uint8 *flag_l,uint8 *Left,uint8 *flag_r,uint8 *Right);
void Scan_Edge_ChuiZhi(uint8 Line[][94],uint8 *flag,uint8 *hight);
void JiaoZheng(uint8 yuantuxiang[][94],uint8 jiaozhenghou[][94]);


#endif