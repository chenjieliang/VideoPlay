//
// Created by chenjieliang on 2019/1/27.
//

#ifndef VIDEOPLAY_XDATA_H
#define VIDEOPLAY_XDATA_H

enum
{
    AVPACKET_TYPE = 0,
    AVFRAME_TYPE = 1,
    UCHAR_TYPE = 2
};

struct XData {
    int type = 0;
    int pts = 0;

    unsigned char *data = 0;
    unsigned  char *datas[8] = {0};
    int size = 0;
    bool isAudio = false;
    int width = 0;
    int height = 0;
    int format = 0;
    //number of samples output per channel
    int samplesLen = 0;
    //每次采样的字节数
    int bytes_per_sample;
    //int channels;
    //int sampleRate;
    bool Alloc(int size, const char *data = 0);
    void Drop();
};
#endif //VIDEOPLAY_XDATA_H
