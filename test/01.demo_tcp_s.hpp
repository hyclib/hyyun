//
// Copyright(c) 1999-2012 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  基本的tcp服务器.
//

#include "hyc/hycLeakDetector.h"
#include "hyyun/hyyun.h"


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

        FileStream fs;
        bool r = fs.open("/root/1.psd", hyc::FileStream::FM_Read|FileStream::FM_Binary);
        trace_all("open file %d", r?1:0);

        char str[50*1024];

        int len = 50*1024;


        mSyncClients.lock();
        for (int i = 0; i < mClients.size(); i++)
        {
        	YunStream& stream = mClients[i]->startWrite(len);   // 表示预分配100字节的流.
        	stream.writeU64(54545);
        	mClients[i]->finishWrite(stream);   // 发送; 取消发送应调用conn.cancelWrite(stream);
        }
        mSyncClients.unlock();

        while (true) {
        	len = fs.readByte(str, len);
        	trace_all("  %d", len);

        	if (len < 0)
        		return;

            // 给conn发送消息.
            YunStream& stream = conn.startWrite(len);   // 表示预分配100字节的流.
            stream.writeRaw((Byte*)str, len);
            conn.finishWrite(stream);   // 发送; 取消发送应调用conn.cancelWrite(stream);

            if (len < 50*1024)
            	return;
        }


//        // 给conn发送消息.
//        YunStream& stream = conn.startWrite(100);   // 表示预分配100字节的流.
//        stream.writeU32(1);
//        conn.finishWrite(stream);   // 发送; 取消发送应调用conn.cancelWrite(stream);
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

    EventHandler    handler;

    //
    // 设置事件处理.
    //
    ser.event_connected().set(&handler, &EventHandler::onConnect);
    ser.event_disconnect().set(&handler, &EventHandler::onDisconnect);
    ser.event_msg().set(&handler, &EventHandler::onMsg);

    ser.go(ip, port);

    String ss;
    while (true)
    {
    	Trace_scans(ss);
    	if (ss == _T("exit"))
    	{
    		return 0;
    	}

    	trace_all("%s", YunServer::TraceDebugInfo().c_str());
    }

    return 0;
}
