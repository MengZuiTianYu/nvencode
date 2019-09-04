#include <cuda.h>
#include "nv12toyuv.h"
__global__ void kernel_yuvconv(uint8_t* src, uint8_t* dst1, uint8_t* dst2)
{
	int i = threadIdx.x;
	dst1[i] = src[2 * i];
	dst2[i] = src[2 * i + 1];
}

void NV12ToYUV(uint8_t* src, uint8_t* des_u, uint8_t* des_v, int width, int height,int pitch)
{
    //pitch = 1920     width = 2048
	uint8_t *src_uv, *mid_uv;
	int width_2 = width / 2;//width_2 = 1024
	int width_4 = width / 4;//width_4 = 512
	src_uv = src + pitch * height;
	cudaMalloc((void**)&mid_uv, width);
	for (int i = 0; i < height / 2; i ++)
	{
		cudaMemcpy(mid_uv,  src_uv,  pitch, cudaMemcpyDeviceToDevice);//pitch = 1920

		if (width_2 >= 512)
		{
			kernel_yuvconv<<<1, width_4>>>(mid_uv, des_u, des_v);//一个网格，512个线程块，width_4这里最好设置为32的倍数
			kernel_yuvconv<<<1, width_4>>>(mid_uv + width_2, des_u + width_4, des_v + width_4);
		}
		des_u	+= width_2;
		des_v	+= width_2;
		src_uv  += pitch;
	}
	cudaFree(mid_uv);
}
