//
// Copyright(c) 1999-2012 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  使用消息中心的tcp/udp客户端.
//

#ifndef __BP_HYYUN_CTEST_HPP_20120210141227__
#define __BP_HYYUN_CTEST_HPP_20120210141227__

#include "hyc/hycLeakDetector.h"
#include "hyyun/hyyun.h"


using namespace hyc;
using namespace hyyun;

//----------------------------------------------------------------------------------------------------
//!	消息处理器.
//--------------------------------------------------------------------------------------------------
class MessageHandler : YunClientHandler
{
public:
    MessageHandler( YunClient* yunClient, u16 mark ) : YunClientHandler(yunClient, mark)  { }

    virtual void    onMessageRecv( const YunStream& readStream, void* userdata )
    {

    }
};

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

    //-----------------------------------------------
    // 接收消息.
    //-----------------------------------------------
    void    onMsg(YunStream& msg, Endpoint& ep)
    {
        Connection& conn = (Connection&)ep;

        // msg 表示客户端发送来的消息.

        // 给conn发送消息.
        YunStream& stream = conn.startWrite(100);   // 表示预分配100字节的流.
        stream.writeU32(1);
        conn.finishWrite(stream);   // 发送; 取消发送应调用conn.cancelWrite(stream);
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

    //
    // 设置事件处理.
    //
    EventHandler    handler;
    client.event_connecting().add(&handler, &EventHandler::onConnecting);
    client.event_disconnected().add(&handler, &EventHandler::onDisconnected);

    //
    // 添加消息处理器.
    //
    client.addHandler(new MessageHandler(&client, 1)); // 添加两个消息处理器, 使用消息处理器后, 不要使用getMessage().
    client.addHandler(new MessageHandler(&client, 2));

    //
    // 连接.
    //
    if (client.connect(host, port))
    {
        // 启动自动接收.
        client.autoRecvMessage(true);
        client.autoHeartBeat(true);

        // 发送消息.
        client.sendMessage();

        // 统计.
        client.getBps();        // 当前网络byte/s.
        client.getSendSize();   // 当前发送字节数.
        client.getRecvSize();   // 当前接收字节数.
        client.resetSendAndRecvSize();  // 清空统计.

        client.disconnect();
    }

    return 0;
}
