//
// Copyright(c) 1999-2012 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  基本的tcp/udp客户端.
//

#ifndef __BP_HYYUN_CTEST_HPP_20120210141227__
#define __BP_HYYUN_CTEST_HPP_20120210141227__

#include "hyc/hycLeakDetector.h"
#include "hyyun/hyyun.h"

using namespace hyc;
using namespace hyyun;

//----------------------------------------------------------------------------------------------------
//!	事件处理对象.
//--------------------------------------------------------------------------------------------------
class EventHandler
{
public:

    //-----------------------------------------------
    // 连接之前的回调,可用于p2p设置等.
    //-----------------------------------------------
    void    onConnecting()
    {
    }

    //-----------------------------------------------
    // 客户端断开.
    //-----------------------------------------------
    void    onDisconnected( bool isInitiative )
    {
    }

};

//----------------------------------------------------------------------------------------------------
//!	程序入口
//--------------------------------------------------------------------------------------------------
int main ()
{
    //
    // 打开内存泄漏跟踪
    // 打开系统日志.
    //
    CN_LEAK_DETECTOR;
    Trace_init(TraceType_console, NULL);

    //
    // 客户端.
    //
    YunClient client(Protocol_Tcp); // 使用Protocol_Udp即为udp.
    client.autoHeartBeat(true);
//    client.autoRecvMessage(true);
    
    client.setPacketAnalyser(YunStreamEmptyAnalyser::Instance());

    //
    // 设置事件处理.
    //
    EventHandler    handler;
    client.event_connecting().add(&handler, &EventHandler::onConnecting);
    client.event_disconnected().add(&handler, &EventHandler::onDisconnected);

    //
    // 连接.
    //
    if (client.connect(host, port))
    {
        client.setTimeout(1000);
        // 发送消息.
        //client.sendMessage();
        
        // 接收消息.
        YunStream msg;
        msg.writeU8(1);
        client.sendMessage(msg);

        FileStream fs;
        fs.open("1.psd", FileStream::FM_Binary|FileStream::FM_ReadWrite|FileStream::FM_New);

        while (true) {
            YunStream& msg = client.getMessage();
            if (!msg.isLegalYun())
                return 0;
            fs.writeByte(msg.begin(), msg.size());
        }

        return 0;

        // 统计.
        client.getBps();        // 当前网络byte/s.
        client.getSendSize();   // 当前发送字节数.
        client.getRecvSize();   // 当前接收字节数.
        client.resetSendAndRecvSize();  // 清空统计.

        system("pause");
        client.disconnect();
    }

    return 0;
}


#endif