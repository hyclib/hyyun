//
// Copyright(c) 1999-2011 hyrj. All rights reserved.
// base network libaray
// author:       lipengxiang
// description:  
//

#ifndef __BP_HYYUN_S_TCP_H_20110627170753__
#define __BP_HYYUN_S_TCP_H_20110627170753__

#include "yunServer.h"
#include "yunConnection.h"

namespace hyyun
{
    //----------------------------------------------------------------------------------------------------
    //*	tcp服务器启动参数.
    //----------------------------------------------------------------------------------------------------
    struct YunServerParam_tcp
    {
        YunServerParam_tcp() : timeout(-1), threadNum(0), queLenRead(8), queLenWrite(8), heartBeatInterval(1000*45), recvBufLength(0)   {}

        /**
         * @brief 超时, <=0则为默认值; (非主服务器时忽略).
         */
        s32         timeout;
        /**
         * @brief 工作线程数, 0则为cpu个数; (非主服务器时忽略).
         */
        u32         threadNum;
        /**
         * @brief 接收队列的长度, 超过此长度若还有消息进入, 则等待. 消息被处理则从队列中移除.
         */
        u32         queLenRead;
        /**
         * @brief 发送队列的长度, 超过此长度若还有消息需发送, 则等待.
         */
        u32         queLenWrite;
        /**
         * @brief 心跳的间隔(单位为millisecond).
         */
        u32         heartBeatInterval;
        /**
         * @brief 初始的recv buf长度(0为默认值), 最大64k.
         */
        u32         recvBufLength;
    };

    //----------------------------------------------------------------------------------------------------
    //* tcp服务端.
    //----------------------------------------------------------------------------------------------------
    class HYYUN_API YunServer_tcp : public YunServer
    {
    public:
        YunServer_tcp( const YunServerParam_tcp& param );
        virtual ~YunServer_tcp();

        /**
         * @brief 获得运行参数.
         */
        const YunServerParam_tcp& getParam() const;

        /**
         * @brief 获得连接数情况.
         */
        u32     getCurConnectedNum() const;
        bool    setMaxConnectionNum( u32 count );
        s32     getMaxConnectionNum() const;

        /**
         * @brief 启动服务器 (参见YunServer::go).
         */
        virtual bool    go( const String& host, int port, ProtocolSupport protocol = ProtocolSupport_IPv4 );

        /**
         * @brief 停止服务器/停止服务器并等待所有的连接断开后停止.
         *        当服务器启动超时为无限等待时,服务器不能停止.
         */
        virtual void    stop();
        virtual void    stopWaitAll();

        /**
         * @brief 暂停接受新连接, 不影响已连接客户端工作.
         */
        void        pauseAccept();

        /**
         * @brief 继续接受新连接.
         */
        void        resumeAccept();

        /**
         * @brief 批量发送数据.
         *          调用startBatchWrite分配数据, 并对YunStream进行设置后, 然后调用finishBatchWrite完成发送.
         *          必须形成如下的调用配对, 不然造成内存丢失: 
         *              确认发送: [startBatchWrite - 设置流数据 - finishBatchWrite]
         *              取消发送: [startBatchWrite - 设置流数据 - cancelBatchWrite]
         *          startBatchWrite能保证一定能够分配成功, 如果暂时内存不足将等待直至分配成功.
         * @param reserveSize
         *          预分配的内存, 准确的值将会减少内存分配的次数.
         * @param arrEp
         *          接收数据的连接数组.
         * @param yunStream
         *          必须为startBatchWrite分配的对象.
         */

        YunStream&   startBatchWrite( u32 reserveSize );
        void         finishBatchWrite( YunStream& yunStream, ConnectionPtr arrEp[], u32 arrSize );
        void         cancelBatchWrite( YunStream& yunStream );


    public:
        /**
         * @brief 新连接的回调; 
         * @param newConnection 新连接的客户端.
         * @return false表明不允许继续连接.
         */
        typedef EventSingle< bool( Connection& newConnection ) >    Event_Connected;
        Event_Connected&        event_connected();

        /**
         * @brief 接收到的消息不合法;
         * @param conn 发送不合法消息的连接.
         * @return false表示将不再继续接收消息; 不再接收消息后, 将会激发event_disconnect.
         */
        typedef EventSingle< bool( Connection& conn ) >   Event_MessageIllegally;
        Event_MessageIllegally& event_messageIllegally();

        /**
         * @brief 连接断开的回调.
         * @param conn 断开的连接.
         */
        typedef EventSingle< void( Connection& conn ) >     Event_Disconnect;
        Event_Disconnect&       event_disconnect();

    protected:
        /**
         * @brief 消息处理, 当无需继续处理时返回false
         */
        virtual bool _onMsg( TcpAgent& agent, YunStream& buf, OUT bool& illegally );
        /**
         * @brief 新连接连上时的处理.
         */
        virtual void _onDealNewConnection( ConnectionPtr& pConn );

    private:
        //! 内部调用.
        void    _onConnected( Connection& newConnection );  // 将新连接添入处理队列中.

    private:
        PRIVATE_CLASS_DECLARE;

        friend class TcpConnectEvent;
        friend class TcpConnectConsumer;
        friend class TcpAgent;
    };

} // end namespace hyyun

#endif // end of define __BP_HYYUN_S_TCP_H_20110627170753__

