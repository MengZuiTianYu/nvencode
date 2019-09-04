#include <iostream>
#include "RtspProcess.h"

/* CUDA 初始化 */
simplelogger::Logger *logger = simplelogger::LoggerFactory::CreateConsoleLogger();
void RtspProcess::m_CudaInit(const int gpuID)
{
    ck(cuInit(0));
    int nGpu = 4;
    ck(cuDeviceGetCount(&nGpu));
    if (gpuID < 0 || gpuID >= nGpu)
    {
        std::ostringstream err;
        err << "Error: GPU ordinal out of range. Should be within [" << 0 << ", " << nGpu - 1 << "]" << std::endl;
        throw std::invalid_argument(err.str());
    }
    CUdevice cuDevice;
    ck(cuDeviceGet(&cuDevice, gpuID));
    char szDeviceName[80];
    ck(cuDeviceGetName(szDeviceName, sizeof(szDeviceName), cuDevice));
    std::cout << "GPU in use: " << szDeviceName << std::endl;
    ck(cuCtxCreate(&g_cuContext, 0, cuDevice));
}
/* 启动解码 */
void RtspProcess::m_Decode(std::string filepath)
{
    demuxer = new FFmpegDemuxer(filepath.c_str());
    dec = new NvDecoder(g_cuContext, demuxer->GetWidth(), demuxer->GetHeight(), true, FFmpeg2NvCodecId(demuxer->GetVideoCodec()));
    int temp = 0;
    int nnWidth = demuxer->GetWidth(), nnHeight = demuxer->GetHeight();
    int nFrameSize = nnWidth * nnHeight * eOutputFormat;
    // img.channels = eOutputFormat;
    // img.height = nnHeight;
    // img.width = nnWidth;

    std::string save_image_dir="/home/zhangl/output/img";
    int quality = 64;
    int channel = 0;
    RectSize rectsize;
    rectsize.height = nnHeight;
    rectsize.width = nnWidth;
    DataBuffer databuffer;
    databuffer.size= nnWidth*nnHeight*3 / 2;
    databuffer.capacity=databuffer.size;
    databuffer.mem_type = MEM_HOST;
    CudaJpegEncode g_cudajpegencode;
    g_cudajpegencode.Init(quality, &rectsize, databuffer.mem_type);
    // cudaMallocManaged<unsigned char>(&img.data, img.width * img.height * img.channels * sizeof(unsigned char));
    databuffer.data = (char*)malloc(databuffer.capacity);//这里和下面num开辟的内存大小是一样的
    // cudaMalloc<char>(&databuffer.data, nWidth*nHeight*3 / 2);
    std::cout << "databuffer.capacity:" << databuffer.capacity << std::endl;
    do
    {
        demuxer->Demux(&pVideo, &nVideoBytes);
        dec->Decode(pVideo, nVideoBytes, &ppFrame, &nFrameReturned);
        for (int i = 0; i < nFrameReturned; i++)
        {
            int num = dec->GetFrameSize() * sizeof(char);
            std::string save_image_path=save_image_dir+std::to_string(temp)+".jpg";
            cudaMemcpy(databuffer.data, ppFrame[i], num, cudaMemcpyDeviceToHost);
            // databuffer.data = (char*)ppFrame[i];
            int64 tStart = cv::getTickCount();
            // jpeg_npp_file(save_image_path.c_str(), quality,  &databuffer, &rectsize,  channel);
            g_cudajpegencode.SetData(&databuffer,&rectsize);
            g_cudajpegencode.EncodeJpeg(IMAGE_DEST_FILE, (char*)save_image_path.c_str(), databuffer.mem_type);
            float time = static_cast<float>((cv::getTickCount() - tStart) * 1000 / cv::getTickFrequency());
            printf("time: %fms\n", time);
            // if (dec->GetOutputFormat() == cudaVideoSurfaceFormat_YUV444)
            //     YUV444ToColor32<BGRA32>((uint8_t *)ppFrame[i], dec->GetWidth(), img.data, eOutputFormat * dec->GetWidth(), dec->GetWidth(), dec->GetHeight());
            // else
            //     Nv12ToColor32<BGRA32>((uint8_t *)ppFrame[i], dec->GetWidth(), img.data, eOutputFormat * dec->GetWidth(), dec->GetWidth(), dec->GetHeight());
            // cv::cuda::GpuMat gpuBGRA(img.height, img.width, CV_8UC4, (uint8_t *)(img.data));
            // gpuBGRA.data = (uint8_t *)(img.data);
            // gpuBGRA.download(c_img);
            // cv::cvtColor(c_img, g_img, CV_BGRA2BGR);
            // cv::imshow("test", c_img);
            // cv::waitKey(0);
            // std::stringstream str;
            // str << "f" << temp << ".jpg";
            // cv::imwrite("/home/zhangl/result/img" + str.str(), g_img);
            std::cout << "正在处理第" << temp << "帧" << std::endl;
            temp++;
        }
        nFrame += nFrameReturned;
    } while (nVideoBytes);
    std::cout << "Total Frame decoded:" << nFrame << std::endl;
    // cuMemFree((CUdeviceptr)(img.data));
    delete demuxer;
    demuxer = nullptr;

    delete dec;
    dec = nullptr;
}
int main()
{
    const int gpuID = 5;
    std::string filepath = "rtsp://admin:admin123@192.168.10.24";
    // std::string filepath = "/nfs-data/wangxy/wangxy/pipeline_data/1.mp4";
    // std::string filepath = "test.mp4";
    RtspProcess rtspProcess;
    rtspProcess.m_CudaInit(gpuID);
    rtspProcess.m_Decode(filepath);
    return 0;
}
