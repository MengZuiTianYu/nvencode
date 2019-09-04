#ifndef RTSPPROCESS_H_
#define RTSPPROCESS_H_

#include <opencv2/core/core.hpp>
// #include <opencv2/imgproc/imgproc.hpp>
// #include <opencv2/imgproc/types_c.h>
// #include <opencv2/highgui/highgui.hpp>
// #include <opencv2/core/cuda.hpp>
// #include <opencv2/cudaimgproc.hpp>
#include "NvDecoder.h"
#include "FFmpegDemuxer.h"
#include "ColorSpace.h"
#include "gh_jpegnpp.h"
#include <time.h>

typedef struct
{
    unsigned char *data = nullptr;
    int height = 0;
    int width = 0;
    int channels = 0;
} Gpumat;

class RtspProcess
{
public:
    void m_CudaInit(const int gpuID);
    void m_Decode(std::string filepath);
private:
    CUcontext g_cuContext = nullptr;
    int eOutputFormat = 4;
    int nFrame = 0;
    int nVideoBytes = 0, nFrameReturned = 0;
    uint8_t *pVideo = nullptr;
    uint8_t **ppFrame;
    // cv::Mat c_img;
    // cv::Mat g_img;
    Gpumat img;
    FFmpegDemuxer *demuxer = nullptr;
    NvDecoder *dec = nullptr;
};
#endif