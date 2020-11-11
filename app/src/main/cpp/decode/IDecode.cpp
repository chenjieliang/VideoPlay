//
// Created by chenjieliang on 2019/2/12.
//

#include "IDecode.h"
#include "../Xlog.h"

int num = 0;
//由主体notify的数据
void IDecode::Update(XData pkt)
{
    if(pkt.isAudio != isAudio)
    {
        return;
    }

    while (!isExit){
        packsMutex.lock();

        //阻塞
        XLOGE("IDecode::isAudio %d packs.size() %d",isAudio, packs.size());
        if(packs.size() < maxList){
            //生产者
            packs.push_back(pkt);
            packsMutex.unlock();
            break;
        }
        packsMutex.unlock();
        XSleep(1);
    }
}

void IDecode::Clear()
{
    while (!packs.empty())
    {
        packs.front().Drop();
        packs.pop_front();
    }
    pts = 0;
    synPts = 0;
}

void IDecode::Main()
{
    while(!isExit)
    {
        if(IsPause())
        {
            XSleep(2);
            continue;
        }
        packsMutex.lock();
        //XLOGE("IDecode::isAudio %d packs.size() %d",isAudio, packs.size());
        //判断音视频同步
        if(!isAudio && synPts >0){
            //c
            //XLOGD("synPts %d pts %d ",synPts,pts);
            if(synPts < pts){
                packsMutex.unlock();
                XSleep(1);
                continue;
            }
        }

        if(packs.empty()){
            packsMutex.unlock();
            XSleep(1);
            continue;
        }
        //取出packet 消费者
        XData pack = packs.front();
        packs.pop_front();

        //发送数据到解码线程， 一个数据包，可能解码多个结果
        if(this->SendPacket(pack))
        {
            while(!isExit)
            {
                //获取解码数据
                XData frame = RecvFrame();
                if(!frame.data){
                    /*if (!isAudio) {
                        num++;
                        XLOGE("jieliang num %d ",num);

                    }
                    XLOGE("jieliang break");*/
                    break;
                }

                this->Notify(frame);
            }
        }
        pack.Drop();
        packsMutex.unlock();
    }


}