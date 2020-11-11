
extern "C"
{
#include <libswresample/swresample.h>
}

#include "IResample.h"
#include "../Xlog.h"
#include "FFResample.h"
#include <libavcodec/avcodec.h>

void FFResample::Close()
{
    mux.lock();
    if(actx)
    {
        swr_free(&actx);
    }
    mux.unlock();
}
bool FFResample::Open(XParameter in, XParameter out)
{
    Close();
    mux.lock();

    //音频重采样
    actx = swr_alloc();
    actx = swr_alloc_set_opts(actx,
                              av_get_default_channel_layout(out.channels),
                              AV_SAMPLE_FMT_S16,out.sample_rate,
                              av_get_default_channel_layout(in.para->channels),
                              (AVSampleFormat)in.para->format,in.para->sample_rate,0,0);

    int re = swr_init(actx);
    if(re != 0)
    {
        mux.unlock();
        XLOGE("swr_init failed!");
        return false;
    }else
    {
        XLOGI("swr_init success");
    }

    outChannels = out.channels;
    outFormat = AV_SAMPLE_FMT_S16;
    mux.unlock();
    return true;
}

XData FFResample::Resample(XData indata)
{
    if(indata.size <= 0 || !indata.data)
    {
        return XData();
    }

    mux.lock();

    if(!actx)
    {
        mux.unlock();
        return XData();
    }

    AVFrame *frame = (AVFrame *)indata.data;

    //输出空间的分配
    XData out;
    int outsize = outChannels * frame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)outFormat);
    if(outsize <= 0)
    {
        mux.unlock();
        return XData();
    }

    out.Alloc(outsize);
    //uint8_t *outArr[2] = {0};
    //outArr[0] = out.data;
    uint8_t *buffer = out.data;
    //返回每个通道输出的样本数,立体声就是2个通道
    int len = swr_convert(actx, &buffer, frame->nb_samples,(const uint8_t **)frame->data,frame->nb_samples);
    XLOGD("len %d",len);
    if(len <= 0)
    {
        XLOGD("len %d",len);
        mux.unlock();
        out.Drop();
        return XData();
    }
    //outsize = len * outChannels * av_get_bytes_per_sample((AVSampleFormat)outFormat);

    //out.channels = av_frame_get_channels(frame);
    out.bytes_per_sample = av_get_bytes_per_sample((AVSampleFormat)outFormat);
    out.samplesLen = len;
    //out.sampleRate = frame->sample_rate;
    out.size = outsize;
    out.pts = indata.pts;
    XLOGD(" ********************* out.pts %d %d",indata.pts,out.pts);
    mux.unlock();
    return out;
}
