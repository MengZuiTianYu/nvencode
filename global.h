#ifndef _GLOBAL_H_
#define _GLOBAL_H_
#include <iostream>

//量化表数据
typedef struct 
{
    unsigned char nPrecisionAndIdentifier;
    unsigned char aTable[64];
} QuantizationTable;

//图片信息
typedef struct 
{
    unsigned char  nSamplePrecision;
    unsigned short nHeight;
    unsigned short nWidth;
    unsigned char  nComponents;
    unsigned char  aComponentIdentifier[3];
    unsigned char  aSamplingFactors[3];
    unsigned char  aQuantizationTableSelector[3];
} FrameHeader;

//扫描头
typedef struct 
{
    unsigned char nComponents;
    unsigned char aComponentSelector[3];
    unsigned char aHuffmanTablesSelector[3];
    unsigned char nSs;
    unsigned char nSe;
    unsigned char nA;
} ScanHeader;

//霍夫曼编码表数据，一般是固定的
typedef struct
{
    unsigned char nClassAndIdentifier;
    unsigned char aCodes[16];
    unsigned char aTable[256];
} HuffmanTable;

//typedef可以严格限定值的范围。
typedef enum
{
    IMAGE_DEST_FILE,
    IMAGE_DEST_MEM
} IMAGE_DEST;

typedef enum
{
    MEM_HOST,
    MEM_CUDA
} MEM_TYPE;

typedef struct
{
    int width;
    int height;
} RectSize;

//缓冲区的通用定义。
typedef struct
{
    //数据存储区。如果发现容量不够，需要先free再malloc
    char* data;
    //数据缓冲区的容量
    int   capacity;
    //数据实际长度，memcpy。<=capacity
    int   size;

    MEM_TYPE mem_type;
    //如果多个线程使用，需要使用这个来决定释放
    //int ref_count;
} DataBuffer;

#endif