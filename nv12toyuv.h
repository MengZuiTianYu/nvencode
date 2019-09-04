#ifndef _NV12TOYUV_H_
#define _NV12TOYUV_H_
#include <iostream>

void NV12ToYUV(uint8_t* src, uint8_t* des_u, uint8_t* des_v, int width, int height,int pitch);

#endif