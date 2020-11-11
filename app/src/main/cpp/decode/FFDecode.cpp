
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavcodec/jni.h>

#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
}

#include "FFDecode.h"
#include "../Xlog.h"

//分数转为浮点数
static double r2d(AVRational rational)
{
    return rational.num == 0 || rational.den == 0 ? 0.:(double)rational.num/ (double)rational.den;
}

static int avcodec_parameters_to_context2(AVCodecContext *codec,
                                         const AVCodecParameters *par)
{
    codec->codec_type = par->codec_type;//AVMEDIA_TYPE_VIDEO
    codec->codec_id   = par->codec_id;//AV_CODEC_ID_H264
    codec->codec_tag  = par->codec_tag;//0

    codec->bit_rate              = par->bit_rate;//0
    codec->bits_per_coded_sample = par->bits_per_coded_sample;//0
    codec->bits_per_raw_sample   = par->bits_per_raw_sample;//8
    codec->profile               = par->profile;//66
    codec->level                 = par->level;//42

    switch (par->codec_type) {
        case AVMEDIA_TYPE_VIDEO:
            codec->pix_fmt                = (AVPixelFormat)(par->format);//12
            codec->width                  = par->width;//1920
            codec->height                 = par->height;//1080
            codec->field_order            = par->field_order;//AV_FIELD_PROGRESSIVE
            codec->color_range            = par->color_range;//AVCOL_RANGE_JPEG
            codec->color_primaries        = par->color_primaries;//AVCOL_PRI_BT709
            codec->color_trc              = par->color_trc;//AVCOL_TRC_BT709
            codec->colorspace             = par->color_space;//AVCOL_SPC_BT709
            codec->chroma_sample_location = par->chroma_location;//AVCHROMA_LOC_LEFT
            codec->sample_aspect_ratio    = par->sample_aspect_ratio;//num=0,den=1
            codec->has_b_frames           = par->video_delay;//0
            break;
        case AVMEDIA_TYPE_AUDIO:
            codec->sample_fmt       = (AVSampleFormat)(par->format);
            codec->channel_layout   = par->channel_layout;
            codec->channels         = par->channels;
            codec->sample_rate      = par->sample_rate;
            codec->block_align      = par->block_align;
            codec->frame_size       = par->frame_size;
            //codec->delay            =
            codec->initial_padding  = par->initial_padding;
            codec->trailing_padding = par->trailing_padding;
            codec->seek_preroll     = par->seek_preroll;
            break;
        case AVMEDIA_TYPE_SUBTITLE:
            codec->width  = par->width;
            codec->height = par->height;
            break;
    }

    //extradata保存SPS/PPS信息,重新拷贝一份到AVCodecContext中，用于解码
    if (par->extradata) {
        av_freep(&codec->extradata);
        codec->extradata = (uint8_t *)(av_mallocz(par->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE));
        if (!codec->extradata)
            return AVERROR(ENOMEM);
        memcpy(codec->extradata, par->extradata, par->extradata_size);
        codec->extradata_size = par->extradata_size;
    }

    return 0;
}



void FFDecode::InitHard(void *vm)
{
    av_jni_set_java_vm(vm,0);
}

void FFDecode::Clear()
{
    IDecode::Clear();
    mux.lock();
    if(codec)
        avcodec_flush_buffers(codec);
    mux.unlock();
}

void FFDecode::Close()
{
    IDecode::Clear();
    mux.lock();
    pts = 0;
    if(frame)
    {
        av_frame_free(&frame);
    }

    if(codec)
    {
        avcodec_close(codec);
        avcodec_free_context(&codec);
    }
    mux.unlock();
}


bool FFDecode::Open(XParameter para, bool isHard)
{
    Close();
    if(!para.para){
        return false;
    }
    AVCodecParameters *p = para.para;

    //1查找解码器
    AVCodec *cd = avcodec_find_decoder(p->codec_id);

    if(!cd)
    {
        cd = avcodec_find_decoder_by_name("h264_mediacodec");
    }

    if(!cd){
        XLOGE("avcodec_find_decoder %d failed", p->codec_id);
        return false;
    }

    XLOGI("avcodec_find_decoder success!");

    mux.lock();
    //2 创建解码上下文 并复制参数
    codec = avcodec_alloc_context3(cd);
    avcodec_parameters_to_context2(codec, p);

    codec->thread_count = 8;

    //3 打开解码器
    int re = avcodec_open2(codec, 0, 0);
    if(re != 0){
        mux.unlock();
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf)-1);
        XLOGE("%s", buf);
        return false;
    }
    if(codec->codec_type == AVMEDIA_TYPE_VIDEO){
        this->isAudio = false;
        //initFilter2(para);
    }else{
        this->isAudio = true;
       // initFilter(para);
    }

    mux.unlock();

    XLOGI("avcodec_open2 success!");
    return true;
}

bool FFDecode::SendPacket(XData pkt)
{
    if(pkt.size <= 0 || !pkt.data)
    {
        return false;
    }
    mux.lock();

    if(!codec)
    {
        return false;
    }
    //XLOGD("SendPacket AVPacket->pts %d : ",(int) ((AVPacket *)pkt.data)->pts);
    int re = avcodec_send_packet(codec, (AVPacket *)pkt.data);
    mux.unlock();
    if(re != 0){
        return false;
    }

    return true;
}

//从线程中获取解码结果
XData FFDecode::RecvFrame()
{
    mux.lock();

    if(!codec)
    {
        mux.unlock();
        return XData();
    }

    if(!frame){
        frame = av_frame_alloc();
    }

    int re = avcodec_receive_frame(codec,frame);
    if(re != 0)
    {
        mux.unlock();
        return XData();
    }
    XData d;

    if(codec->codec_type == AVMEDIA_TYPE_VIDEO){
        d.size = (frame->linesize[0] + frame->linesize[1] + frame->linesize[2])*frame->height;
        d.width = frame->width;
        d.height = frame->height;
        //filterFrame(frame);
    }else
    {
        //样本字节数 * 单通道样本数 * 通道数
        d.size = av_get_bytes_per_sample((AVSampleFormat)frame->format)*frame->nb_samples * frame->channels ;
    }
    if (frame)
    {
    d.data = (unsigned char *)frame;
    d.format = frame->format;
    memcpy(d.datas, frame->data, sizeof(d.datas));
    d.pts = frame->pts;
    pts =  d.pts;
    } else {
      XLOGD("frame null");
    }
    mux.unlock();
    return d;
}

int FFDecode::initFilter(XParameter parameter){
    //1 注册滤波器组件
    avfilter_register_all();

    //2 初始化图状结构
    filter_graph = avfilter_graph_alloc();

    //3.1 获取输入filter
    AVFilter *buffersrc  = avfilter_get_by_name("abuffer"); /* 输入buffer filter */
    buffersrc_ctx = avfilter_graph_alloc_filter(filter_graph,buffersrc,"src");
    char args[512];
    snprintf(args, sizeof(args),
             "sample_rate=%d:sample_fmt=%s:channel_layout=0x%" PRIx64,
             codec->sample_rate, av_get_sample_fmt_name(codec->sample_fmt), codec->channel_layout);
    int ret = avfilter_init_str(buffersrc_ctx,args);
    if (ret < 0) {
        XLOGD("errot init abuffer filter");
        return ret;
    }
    //3.2 获取用于倍速播放的filter
    AVFilter *atempo_filter = avfilter_get_by_name("atempo");
    AVFilterContext *atempo_ctx = avfilter_graph_alloc_filter(filter_graph,atempo_filter,"atempo");

    AVDictionary *dic = NULL;
    ret = av_dict_set(&dic,"tempo","2.0",0);
    if (ret < 0) {
        XLOGD("av_dict_set fail error info = %s" ,av_err2str(ret));
        return ret;
    }
    ret = avfilter_init_dict(atempo_ctx,&dic);
    if (ret < 0) {
        XLOGD("avfilter_init_dict fail error info = %s" ,av_err2str(ret));
        return ret;
    }
    //3.3 获取输出filter
    AVFilter *buffersink = avfilter_get_by_name("abuffersink"); /* 输出buffer filter */
    buffersink_ctx = avfilter_graph_alloc_filter(filter_graph,buffersink,"sink");
    ret = avfilter_init_str(buffersink_ctx,NULL);
    if (ret < 0) {
        XLOGD("errot init buffersink");
        return ret;
    }

    //3.4 链接
    if (avfilter_link(buffersrc_ctx,0,atempo_ctx,0)!=0)
    {
        XLOGD("errot link to atmpo filter");
        return ret;
    }
    if (avfilter_link(atempo_ctx,0,buffersink_ctx,0)!=0)
    {
        XLOGD("errot link to sink filter");
        return ret;
    }

    XLOGD("avfilter_graph_create_filter success");

    //4 配置构建的图
    ret = avfilter_graph_config(filter_graph, NULL);
    if (ret < 0) {//检查当前所构造的FilterGraph的完整性与可用性
        XLOGE("Cannot avfilter_graph_config eror = %s",av_err2str(ret));
        return ret;
    }
    XLOGI("avfilter_graph_config suc");

    return ret;
}

int FFDecode::initFilter2(XParameter parameter){
    //1 注册滤波器组件
    avfilter_register_all();

    AVBufferSinkParams *buffersink_params;
    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };

    //2 初始化图状结构
    filter_graph = avfilter_graph_alloc();

    //3.1 获取filter
    AVFilter *buffersrc  = avfilter_get_by_name("buffer"); /* 输入buffer filter */

    int ret;
    char args[512];
    AVRational time_base = *(parameter.time_base);/* 时间基数 */
    /*snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d" ,
             codec->width, codec->height, codec->pix_fmt,time_base.num, time_base.den, codec->sample_aspect_ratio.num,
             codec->sample_aspect_ratio.den);*/
    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d" ,
             codec->width, codec->height, codec->pix_fmt,time_base.num, time_base.den, codec->sample_aspect_ratio.num,
             codec->sample_aspect_ratio.den);

    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in", args, NULL, filter_graph);
    if (ret < 0) {
        XLOGE("Cannot create buffer source === %d\n",ret);
        return ret;
    }

    buffersink_params = av_buffersink_params_alloc();
    buffersink_params->pixel_fmts = pix_fmts;

    //3.2 获取filter
    AVFilter *buffersink = avfilter_get_by_name("buffersink"); /* 输出buffer filter */

    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out", NULL, buffersink_params, filter_graph);
    av_free(buffersink_params);
    if (ret < 0) {
        XLOGE("Cannot create buffer sink\n");
        return ret;
    }

    XLOGI("avfilter_graph_create_filter suc");

    ret = av_opt_set_int_list(buffersink_ctx, "pix_fmts", pix_fmts,
                              AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        XLOGI(NULL, AV_LOG_ERROR, "Cannot set output pixel format");
    }

    //4.1 标识Filter在哪个节点后面
    AVFilterInOut *outputs = avfilter_inout_alloc();
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    AVFilterInOut *inputs  = avfilter_inout_alloc();
    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    char *filters_descr;

    //改变颜色水印
    char *filters_color = "lutyuv='u=128:v=128'";

    //文字水印，暂不支持可能编译时没有添加文字选项
    char *filters_text = "drawtext=fontsize=20:fontfile=Kaiti.ttf:text='中文水印':fontcolor=green:box=1:boxcolor=yellow";

    //添加图片水印
    char *filters_img = "movie=/storage/emulated/0/DCIM/ic_launcher.png[wm];[in][wm]overlay=100:100[out]";

    //视频二倍速
    char *filters_speed_video = "setpts=0.5*PTS";

    //音频二倍速
    char *filters_speed_audio = "atempo=tempo=1.1";

    //音视频二倍速
    char * filter_spped_both = "[0:v]setpts=0.5*PTS[v];[0:a]atempo=2.0[a]";

    char * filter_rate = "scale=78:24";

    filters_descr = filters_img;

    //5 将这个filter加入图中
    ret = avfilter_graph_parse_ptr(filter_graph, filters_descr, &inputs, &outputs, NULL);
    if (ret < 0) {
        XLOGE("Cannot avfilter_graph_parse_ptr\n");
        return ret;
    }

    avfilter_inout_free(&outputs);
    avfilter_inout_free(&inputs);

    //6 配置构建的图
    ret = avfilter_graph_config(filter_graph, NULL);

    if (ret < 0) {//检查当前所构造的FilterGraph的完整性与可用性
        XLOGE("Cannot avfilter_graph_config eror = %s",av_err2str(ret));
        return ret;
    }
    XLOGI("avfilter_graph_config suc");

    return ret;
}

AVFrame FFDecode::filterFrame(AVFrame *frame) {
    int ret;
    //把解码后视频帧添加到filter_graph
    ret = av_buffersrc_add_frame_flags(buffersrc_ctx, frame, AV_BUFFERSRC_FLAG_KEEP_REF);
    if (ret < 0) {
        XLOGE("Could not av_buffersrc_add_frame");
        return *frame;
    }

    av_frame_unref(frame);
    //把滤波后的视频帧从filter graph取出来
    if ((ret = av_buffersink_get_frame(buffersink_ctx, frame)) >= 0) {

    } else {
        XLOGE("Could not av_buffersink_get_frame");
    }
    return *frame;
}

