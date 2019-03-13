//
// Copyright(c) 1999-2013 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#ifndef __BP_HYYUN_S_H_20130402160729__
#define __BP_HYYUN_S_H_20130402160729__

#include <map>
#include "yun_sPlatform.h"
#include "yunEndpoint.h"
#include "yunServerHandler.h"

namespace hyyun
{
    //----------------------------------------------------------------------------------------------------
    //* 服务器.
    //----------------------------------------------------------------------------------------------------
    class HYYUN_API YunServer : public INoncopyable
    {
    public:
        /**
         * @brief 输出调试信息.
         */
        static String  TraceDebugInfo();

        /**
         * @brief 设置内存池整理间隔; 默认为10分钟, 单位为millisecond.
         */
        static void    SetMemoryPoolClearupElapse( u32 elapse );
        static u32     GetMemoryPoolClearupElapse();

    public:
        /**
         * @brief 构造服务器.
         * @param timeout   通信超时
         * @param threadNum 工作线程数
         * @param queLenRead    接收队列长度.
         * @param queLenWrite   发送队列长度.
         */
        YunServer( s32 timeout, u32 threadNum, u32 queLenRead, u32 queLenWrite );
        virtual ~YunServer();

        /**
         * @brief 设置包解析器.
         */
        void                    setPacketAnalyser( YunStreamAnalyserPtr pAnalyser );
        YunStreamAnalyserPtr    getPacketAnalyser() const;

        /**
         * @brief 添加消息处理器, 服务启动后不要调用.
         * @return 表明是否成功写入.
         */
        bool                    addHandler( YunServerHandlerPtr msgHandler );
        YunServerHandlerPtr&    getHandler( u16 mark );

        /**
         * @brief 消息选择器.
         */
        virtual YunSelector&    getSelector()       { return _selector; }

        /**
         * @brief 启动后的服务器地址信息.
         */
        virtual const Endpoint& getEndpoint() const { return _ep; }

        /**
         * @brief 服务器是否在运行.
         */
        virtual bool            isRunning() const   { return _isRunning; }

        /**
         * @brief 启动服务器.
         * @param host 可以使用ip或域名来指定对外的连接地址, 如果host为空串则使用本机ip.
         * @param port 服务器端口.
         * @param protocol 指定为IPv4或IPv6.
         * @return 指明启动是否成功.
         */
        virtual bool            go( const String& host, int port, ProtocolSupport protocol = ProtocolSupport_IPv4 ) = 0;

        /**
         * @brief 停止服务器.
         */
        virtual void            stop() = 0;

    public:
        /**
         * @brief 初始化事件.(在即将要开始接收连接前的回调).
         * @param arrWorkThreadIds 所有工作线程的id.
         */
        typedef EventSingle<void(Array<integer> arrWorkThreadIds)> Event_Init;
        Event_Init&     event_init() { return _eventInit; }

        /**
         * @brief 接收到消息回调. (接收到的流为 [begin~_curPos)
         *          设置消息处理将不使用消息中心来处理消息,所有的消息将由此回调来处理.
         * @param msg
         *          接收到的消息.
         * @param ep
         *          发送消息的Endpoint.
         */
        typedef EventSingle<void(YunStream& msg, Endpoint& ep)>    Event_Msg;
        Event_Msg&      event_msg() { return _eventMsg; }


    protected:
        // YunStream创建后的操作.
        virtual void    _onStreamMalloc( bool useForSend, YunStream& buf )  {}
        // YunStream的自定义析构,非NULL的析构中需调用YunStreamPool::free.
        virtual void    _onStreamDeallocCallback( YunStream::DeallocCallback& cb )  {}

    protected:
        void    _initWorkItems( YunSelector& selector );
        void    _exitWorkItems();
        void    _pushMessageRead( void* data, uinteger mask );
        void    _pushMessageWrite( void* data, uinteger mask );

        // 启动前调用.
        // @param arrMsgConsumer 数量需与工作线程数相同, 且内存交由本类释放.
        void    _go(const String& host, int port, ProtocolSupport protocol, const Array<hymq::MessageConsumer*> arrReadConsumer, const Array<hymq::MessageConsumer*> pWriteConsumer);

    protected:
        Event_Msg               _eventMsg;
        Event_Init              _eventInit;
        bool                    _isRunning;
        u32                     _threadNum;
        s32                     _timeout;
        YunSelector             _selector;
        typedef std::map<u16, YunServerHandlerPtr>  MsgProMap;
        MsgProMap               _mapHandler;        
        Endpoint                _ep;
        ThreadPtr               _pWorkThread;
        MessageQueue*           _pMessageQueue;
        MessageQueue*           _pMessageQueue_write;
        u32                     _queLenRead;
        u32                     _queLenWrite;
        YunStreamAnalyserPtr    _pAnalyser;

        friend class UdpAgent;
        friend class TcpAgent;
    };

} // end namespace hyyun

#endif // end of define __BP_HYYUN_S_H_20130402160729__
