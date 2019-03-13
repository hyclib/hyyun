//
// Copyright(c) 1999-2012 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  基本的udp服务器.
//

#include "include/hyc/hycLeakDetector.h"
#include "include/hyyun/hyyun_s.h"

//----------------------------------------------------------------------------------------------------
//!	事件处理对象.
//--------------------------------------------------------------------------------------------------
class EventHandler
{
    YunServer_udp&              mrServer;
    // 所有的客户端.
    SyncObject                  mSyncClients;
    ArrayFind<Endpoint>         mClients;
public:
    EventHandler(YunServer_udp& sever):mrServer(server)  {}

    //-----------------------------------------------
    // 服务器错误.
    //-----------------------------------------------
    void    onError( Connection& newConnection )
    {
        // 需重新启动服务器.
        //mrServer.stop();
        //mrServer.go()
    }

    //-----------------------------------------------
    // 接收消息.
    //-----------------------------------------------
    void    onMsg(YunStream& msg, Endpoint& ep)
    {
        // 保存客户端信息.
        mSyncClients.lock();
        mClients.push_back(ep);
        mSyncClients.unlock();

        // msg 表示客户端发送来的消息.

        // 给ep发送消息.
        YunStream& stream = mrServer.startWrite(100);   // 表示预分配100字节的流.
        stream.writeU32(1);
        mrServer.finishWrite(stream, ep);   // 发送; 取消发送应调用mrServer.cancelWrite(stream);
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
    // 服务器参数.
    //
    YunServerParam_udp param;
    {
        param.timeout = -1;         // 超时.
        param.threadNum = 8;        // 工作线程数.
        param.queLenWrite = 2;      // 发送队列长度.
        param.queLenRead = 2;       // 读取队列长度.
    }
    YunServer_udp ser(param);

    //
    // 设置事件处理.
    //
    EventHandler    handler(ser);
    ser.event_error().set(&handler, &EventHandler::onError);
    ser.event_msg().set(&handler, &EventHandler::onMsg);

    ser.go(ip, port);

    return 0;
}