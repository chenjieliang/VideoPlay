//
// Created by chenjieliang on 2019/8/29.
//
extern "C" {
#include <libavcodec/avcodec.h>
}
#include "FFNetVideo.h"

void FFNetVideo::init(int width, int height)
{
    AVCodecParameters *avPara = avcodec_parameters_alloc();
    avPara->codec_type = AVMEDIA_TYPE_VIDEO;
    avPara->codec_id = AV_CODEC_ID_H264;
    avPara->format = AV_PIX_FMT_YUV420P;
    avPara->width = width;
    avPara->height = height;
    avPara->sample_aspect_ratio.num = 4;
    avPara->sample_aspect_ratio.den = 3;

    param.para = avPara;
    AVRational time_base;
    time_base.num = 1;
    time_base.den = 12800;
    param.time_base = &time_base;
}

XParameter FFNetVideo :: getParameter()
{
    return param;
}
void FFNetVideo::receive(int type, int camType, int width, int height, int fps, long pts,
                         unsigned char *dataArray, long size)
{
    XData d;
    AVPacket *pkt = av_packet_alloc();
    //av_init_packet(pkt);
    pkt->data = dataArray;
    pkt->size = size;
    pkt->pts = pts;
    d.size = size;
    d.data = (unsigned char*)pkt;
    d.format = AV_PIX_FMT_YUV420P;
    d.pts = (int)pkt->pts;
    d.isAudio = false;
    send(d);
}
