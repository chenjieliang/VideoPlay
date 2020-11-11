//
// Created by chenjieliang on 2019/8/29.
//

extern "C" {
#include <libavcodec/avcodec.h>
}
#include "FFNetAudio.h"

void FFNetAudio :: init()
{
    AVCodecParameters *avPara = avcodec_parameters_alloc();
    avPara->codec_type = AVMEDIA_TYPE_AUDIO;
    avPara->codec_id = AV_CODEC_ID_PCM_ALAW; //G711A
    avPara->format = AV_SAMPLE_FMT_S16;
    avPara->channels = 1;
    avPara->sample_rate = 8000;
    //avPara->channel_layout = 1;

    param.para = avPara;
    param.channels = 1;
    param.sample_rate = 8000;
}

XParameter FFNetAudio :: getParameter()
{
    return param;
}

void FFNetAudio :: receive(
        int type,             //I、P、A
        int channels,         //声道数
        int sps,              //采样率
        int bps,              //
        long pts,             //时间戳
        unsigned char * dataArray,     //unint_8 、unsigned char
        long size)
{
    XData d;
    AVPacket *pkt = av_packet_alloc();
    //av_init_packet(pkt);
    pkt->data = dataArray;
    pkt->size = size;
    pkt->pts = pts;

    d.size = size;
    d.data = (unsigned char*)pkt;
    d.bytes_per_sample = bps;
    d.pts = (int)pkt->pts;
    d.isAudio = true;

    send(d);
}