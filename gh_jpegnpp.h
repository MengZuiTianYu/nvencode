#ifndef __GH_JPEG_NPP_H__
#define __GH_JPEG_NPP_H__

#include <npp.h>
#include <cuda_runtime.h>
#include "Exceptions.h"
#include "global.h"
#include "Endianess.h"
#include <math.h>
#include <cmath>
#include <string.h>
#include <fstream>
#include <iostream>

#include "helper_string.h"
#include "helper_cuda.h"

#define IMAGE_MAX_WIDTH  1920
#define IMAGE_MAX_HEIGHT 1080

//每个线程使用一个静态实例。
#define USE_STATIC_INSTANCE	1

class CudaJpegEncode
{
public:
    CudaJpegEncode();
    ~CudaJpegEncode();
public:
    void Init(int quality, const RectSize* pImageSize, MEM_TYPE mem_type);
    void SetQuality(unsigned char* pTable, const unsigned char* pTable50, int quality);
    void Release();
public:
    int  EncodeJpeg(const IMAGE_DEST dest, char* pDest, MEM_TYPE mem_type);
    void SetData(const DataBuffer* pImageBuffer, const RectSize* pImageSize);
public:
    NppiDCTState*        pDCTState;
    FrameHeader          oFrameHeader;
    QuantizationTable    aQuantizationTables[4];
    Npp8u*               pdQuantizationTables;

    HuffmanTable         aHuffmanTables[4];
    //HuffmanTable*        pHuffmanDCTables = aHuffmanTables;
    //HuffmanTable*        pHuffmanACTables = &aHuffmanTables[2];
    HuffmanTable*        pHuffmanDCTables;
    HuffmanTable*        pHuffmanACTables;
    ScanHeader           oScanHeader;

    NppiSize aSrcSize[3];
    Npp16s  *apdDCT[3];
    Npp32s   aDCTStep[3];

    Npp8u   *apSrcImage[3];
    Npp32s   aSrcImageStep[3];
    size_t   aSrcPitch[3];

    NppiEncodeHuffmanSpec *apHuffmanDCTable[3];
    NppiEncodeHuffmanSpec *apHuffmanACTable[3];

    int nMCUBlocksH;
    int nMCUBlocksV;

    Npp8u *pdScan;
    Npp32s nScanSize;

    Npp8u *pJpegEncoderTemp;
    size_t nTempSize;

    unsigned char *pDstJpeg;
public:
    uint8_t*    mY;
    uint8_t*    mU;
    uint8_t*    mV;
    int         mWidth;
    int         mHeight;
};



// int jpeg_npp_dest(const IMAGE_DEST dest, char *pDest, const int quality,
//         const DataBuffer* pImageBuffer, const RectSize* pImageSize, const int channel);

// int jpeg_npp_file(     const char *szOutputFile,      const int quality, 
//         const DataBuffer* pImageBuffer, const RectSize* pImageSize, const int channel);

// int jpeg_npp_mem(      DataBuffer *pJpegBuffer, const int quality,
//         const DataBuffer* pImageBuffer, const RectSize* pImageSize, const int channel);



#endif //__JPEG_NPP_H__
