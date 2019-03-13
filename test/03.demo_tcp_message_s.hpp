//
// Copyright(c) 1999-2012 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  使用消息中心的tcp服务器.
//

#include "include/hyc/hycLeakDetector.h"
#include "include/hyyun/hyyun_s.h"


//----------------------------------------------------------------------------------------------------
//!	消息处理器.
//--------------------------------------------------------------------------------------------------
class MessageHandler : YunServerHandler
{
public:
    MessageHandler( YunServer* server, u16 mark, YunStatus status ) : YunServerHandler(server, mark, status)  { }

    virtual bool  onMessageRecv( Endpoint& ep, const YunStream& readStream, void* userdata )
    {

    }
};

//----------------------------------------------------------------------------------------------------
//!	事件处理对象.
//--------------------------------------------------------------------------------------------------
class EventHandler
{
    // 所有的客户端.
    SyncObject                  mSyncClients;
    ArrayFind<ConnectionPtr>    mClients;
public:

    //-----------------------------------------------
    // 客户端连接.
    //-----------------------------------------------
    bool    onConnect( Connection& newConnection )
    {
        // 打印信息.
        trace_all(
            _T("(%d) %s:%d"),
            newConnection.getSocket(),              // socket.
            newConnection.getEndpointIp().c_str(),  // ip.
            newConnection.getEndpointPort()         // port.
            );

        // 设置非延迟.
        SocketUtils::SetTcpNoDelay(newConnection.getSocket());

        // 保存连接.
        mSyncClients.lock();
        mClients.push_back(&newConnection);
        mSyncClients.unlock();

        // 返回false表示拒绝连接.
        return true;
    }

    //-----------------------------------------------
    // 客户端断开.
    //-----------------------------------------------
    void    onDisconnect( Connection& newConnection )
    {
        // 移除.
        mSyncClients.lock();
        mClients.eraseByIndex(mClients.find(&newConnection));
        mSyncClients.unlock();
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
    // 服务器参数.
    //
    YunServerParam_tcp param;
    {
        param.timeout = -1;         // 超时.
        param.threadNum = 8;        // 工作线程数.
        param.queLenWrite = 2;      // 发送队列长度.
        param.queLenRead = 2;       // 读取队列长度.
        param.recvBufLength = 50;   // 接收包初始大小.
    }
    YunServer_tcp ser(param);

    //
    // 设置事件处理.
    //
    EventHandler    handler;
    ser.event_connected().set(&handler, &EventHandler::onConnect);
    ser.event_disconnect().set(&handler, &EventHandler::onDisconnect);
    //ser.event_msg().set(&handler, &EventHandler::onMsg);

    //
    // 添加消息处理器.
    //
    ser.addHandler(new MessageHandler(&ser, 1, 0)); // 添加两个消息处理器, 使用消息处理器后, 不要使用event_msg().
    ser.addHandler(new MessageHandler(&ser, 2, 0));

    ser.go(ip, port);

    return 0;
}
