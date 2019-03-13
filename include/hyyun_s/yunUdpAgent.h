//
// Copyright(c) 1999-2013 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#ifndef __BP_HYUDPAGENT_H_20130407110625__
#define __BP_HYUDPAGENT_H_20130407110625__

namespace hyyun
{
    //----------------------------------------------------------------------------------------------------
    //*	Agent表示udp通信代理. UdpAgent 在内部使用,外部无需细节.
    //* 数据的接收是非主动模式,程序不可以主动请求数据,当有数据产生时系统将使用通信对象来通知.
    //----------------------------------------------------------------------------------------------------
    class HYYUN_API UdpAgent : public SelectorEvent, public INoncopyable
    {
    public:
        UdpAgent( YunServer_udp& server, YunUdpTransceiver& trans, YunServer::Event_Msg* event );
        ~UdpAgent();

        /**
         * @brief 获得socket.
         */
        inline SOCKET   getSocket()      { return socket(); }

        /**
         * @brief 判断代理是否关闭.
         */
        inline bool     isClosed() const { integer expected = TRANS_ATOMIC_UNSET; return atomic_compare_exchange((integer volatile*)&_atomicTransOld, &expected, TRANS_ATOMIC_UNSET); }
        
        /**
         * @brief 关闭当前代理.
         */
        void            close();

        /**
         * @brief 开始工作. 只有在调用此方法后代理才生效, 不要多次调用.
         */
        void            start();

        /**
         * @brief 给指定Endpoint发送数据.(参见TcpAgent)
         */
        void            writeAll( YunStream& yunStream, const Endpoint& ep );

        /**
         * @brief 给指定Endpoint发送消息.(参见TcpAgent)
         */
        YunStream&      startWrite( u32 reserveSize);
        bool            finishWrite( YunStream& yunStream, const Endpoint& ep );
        void            cancelWrite( YunStream& yunStream );

        /**
         * @brief 批量发送数据.(参见YunServerTcp)
         */
        YunStream&      startBatchWrite( u32 reserveSize );
        void            finishBatchWrite( YunStream& yunStream, const Array<Endpoint>& arrEp );
        void            cancelBatchWrite( YunStream& yunStream );

    protected:
        //! Interface of SelectorEvent.
        virtual SOCKET      socket()        { return _rTrans.socket(); }
        virtual bool        startAsync( AsyncInfo* pInfo, AsyncData* data );
        virtual void        finishAsync( AsyncInfo* pInfo );
        virtual void        doAsync_read( UdpMessageData* pData );
        virtual void        doAsync_write( AsyncInfo* pInfo )   { _finishAsync_write(pInfo); }

        // 开始读写操作.
        bool                startRW( AsyncInfo* pInfo, UdpAsyncData* pData, bool rightnow );

        // 数据对象分配.
        void                initAsyncInfo();
        UdpAsyncData*       getReadAsync()  { return _pReadAsyncData; }
        AsyncInfo*			mallocAsyncWrite();
        UdpAsyncData*	    mallocUdpAsyncWrite( const Endpoint& ep, YunStream& yunStream );
        void                freeUdpAsync( UdpAsyncData* pInfo );
        void                freeAsync( AsyncInfo* async );
        void                freeAsyncOnly( AsyncInfo* async );
        UdpMessageData*     mallocMsgData( const Endpoint& ep, YunStream& yunStream, u32 offset, u32 len );
        void                freeMsgData( UdpMessageData* pData );


        // info类型判断.
        inline bool         isReadInfo( AsyncInfo* pInfo )  { return pInfo == &_recvInfo; }
        inline bool         isWriteInfo( AsyncInfo* pInfo ) { return pInfo != &_recvInfo; }

        // 异步完成处理.
        void                        _finishAsync_read( AsyncInfo* pInfo );
        bool                        _finishAsync_read_deal( YunStream& buf, AsyncInfo* pInfo, const Endpoint& ep ); // 返回值表明是否继续接收消息.
        void                        _finishAsync_write( AsyncInfo* pInfo );
        AsyncTransceiver::Result    _finishAsync_write_once( AsyncInfo* pInfo, UdpAsyncData* pData );
        bool                        _finishAsync_error( AsyncInfo* pInfo );

        // 消息处理.
        void                _onReflexMsgDeal( YunStream& readStream, Endpoint& ep );  // 处理消息.
        void                _onReflexMsgIllegally( Endpoint& ep );
        void                _onError( AsyncInfo* async );

    protected:
        YunServer_udp&          _rServer;
        YunSelector&            _rSelector;
        YunServer::Event_Msg&   _rEventMsg;
        YunUdpTransceiver&      _rTrans;
        volatile integer        _atomicTransOld; // for atomic compare.

#ifdef CN_NET_IOCP
        AsyncInfo               _recvInfo;
#endif // CN_NET_IOCP
        UdpAsyncData*           _pReadAsyncData;

        friend class UdpMessageConsumer;
        friend class UdpWriteConsumer;
    };

    typedef ObjectPtr<UdpAgent> UdpAgentPtr;

} // end namespace hyyun


#endif // end of define __BP_HYUDPAGENT_H_20130407110625__
