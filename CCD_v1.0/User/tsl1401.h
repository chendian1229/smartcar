#ifndef __TSL1401_H__
#define __TSL1401_H_


void SamplingDelay(void);
void CCD_init(void);
void StartIntegration(void);
void ImageCapture(unsigned char * ImageData);
void SendHex(unsigned char hex);
void SendImageData(unsigned char * ImageData);
void ErZhiHua(uint8 *ImageData);
uint8 JiSuanKuanDu(uint8 *ImageData);
uint8 ZhaoZhongJian_From_ErZhiHuaHouDe(uint8 *ImageData,uint8 KuanDu);


#endif