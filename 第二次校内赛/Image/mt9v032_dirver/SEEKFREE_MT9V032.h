#ifndef _MT9V032_h
#define _MT9V032_h


void mt9v032_camera_init();
void VSYNC();
void row_finished();
void mt9v032_get_img();
void seekfree_sendimg_zzf(void *imgaddr, uint32_t imgsize);


#endif