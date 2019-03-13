// (c) 1999-2011 hyrj. All rights reserved.
// base network libaray
// author:       lipengxiang
// description:  
//

#ifndef __BP_HYYUN_C_H_20110629163251__
#define __BP_HYYUN_C_H_20110629163251__

#include "yun_cPlatform.h"
#include "yunStatistics.h"
#include "yunClientHandler.h"

namespace hyyun
{
    
    //----------------------------------------------------------------------------------------------------
    //! yun客户端; 不要对 YunClient的接收发送方法进行多线程编程.
    //----------------------------------------------------------------------------------------------------
    class HYYUN_API YunClient : public INoncopyable, public YunStatistics
    {
        PRIVATE_CLASS_DECLARE;
    public:
        /**
         * @brief 内部创建socket的方式来创建客户端
         * @param timeout   
         *      超时, -1则为默认值
         */
        YunClient( hyc::ProtocolType protocol, s32 timeout = -1 );
        virtual ~YunClient();
        
        SOCKET              getSocket();
        s32                 getTimeout() const;
        void                setTimeout( s32 timeout );

        /**
         * @brief 设置解析器.
         */
        void                setPacketAnalyser( YunStreamAnalyserPtr pAnalyser );
        YunStreamAnalyserPtr getPacketAnalyser() const;

        /**
         * @brief 每次调用加1, 可用于udp包序号.
         */
        u32                 sequence();

        /**
         * @brief 心跳间隔.
         */
        void                setHeartBeatElapse( u32 elapse = 1000*15 );
        const u32&          getHeartBeatElapse() const;

        /**
         * @brief 上次发送的时间.
         */
        Time                getPreSendTime() const;

        /**
         * @brief 消息中心, 返回false表明同样的mark已经存在.
         */
        bool                addHandler( YunClientHandlerPtr msgProcessor );
        YunClientHandlerPtr& getHandler( u16 mark );
        
        /**
         * @brief
         *    返回false表明连接超时; 连接成功之后就不能再调用addMessageProcessor
         * @param host 可以使用ip或域名来指定对外的连接地址, 如果host为空串则使用本机ip.
         */
        virtual bool        connect( const String& host, PORT port, ProtocolSupport protocol = ProtocolSupport_IPv4, const String& mcastInterface = _T(""), int mcastTtl = -1 );
        virtual void        disconnect();
        virtual bool        isDisconnect() const;
        
        /**
         * @brief
         *    s发送消息指服务端; 返回false表明发送失败;
         * @param msg
         *    只要发送/接收不是多线程异步的, msg就可以使用getmessage返回的对象.
         */
        virtual bool        sendMessage( YunStream& msg );
        virtual bool        sendHeartBeat();
        
        /**
         * @brief
         *    当有消息到达时的回调;
         *      WARNING: 如果使用消息中心异步处理消息的话, 只能在消息中心线程内调用getMessage, 其他线程中调用都将失败
         *      WARNING: 不要对这个函数使用多线程, 一个Client的接收只能在一个线程中
         * @return 
         *     当YunStream isLegalYun 表明接收正确.
         *     当返回的流不合法时, !isDisconnect 则表示为超时.
         */
        virtual YunStream&  getMessage();


        /**
         * @brief 自动发送心跳包.
         */
        virtual void        autoHeartBeat( bool enable );
        virtual bool        isAutoHeartBeat() const;

        /**
         * @brief 自动接收消息,并且由消息中心处理.
         */
        virtual void        autoRecvMessage( bool enable );
        virtual bool        isAutoRecvMessage() const;

        //-----------------------------------------------
        //! 事件处理.
        //-----------------------------------------------
    public:
        /**
         * @brief
         *    连接之前的回调, 在此做端口绑定等处理.
         */
        typedef Event<void(void)>   Event_Connect;
        Event_Connect&      event_connecting();
        
        /**
         * @brief
         *    断线后的回调.
         * @param isInitiative 指明是否是由主动调用disconnect引起.
         */
        typedef Event<void(bool isInitiative)>   Event_Disconnectd;
        Event_Disconnectd&  event_disconnected();

        /**
         * @brief 接收到消息, 手动传入消息,可以使用消息中心处理.
         */
        virtual bool        onMsg( const YunStream& buf );

    protected:
        // 对心跳包进行封装.
        virtual void                        _onWrapHeartbeat( YunStream& heartbeat );
        // YunStream创建后的操作.
        virtual void                        _onMallocStream( bool useForSend, YunStream& buf )    {}
        // YunStream的自定义析构;处理YunStream析构前的清除.
        virtual YunStream::DeallocCallback  _onGetStreamDeallocCallback()                         { return NULL; }

    public:
        void            _exitInThread();                // 内部使用,接收线程内调用, 外部不允许调用.

    protected:
        bool    _swapMsg(); // 返回值表明是否存在粘包.
        bool    _dealMsg();
    };
    
    // 定义指针和引用类型.
    typedef ObjectPtr<YunClient>    YunClientPtr;
    
} // end namespace hyyun

#endif // end of define __BP_HYYUN_C_H_20110629163251__

