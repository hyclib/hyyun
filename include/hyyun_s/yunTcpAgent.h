//
// Copyright(c) 1999-2013 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#ifndef __BP_HYTCPAGENT_H_20130407162106__
#define __BP_HYTCPAGENT_H_20130407162106__

namespace hyyun
{
    //----------------------------------------------------------------------------------------------------
    //*	Agent类用于表示一个客户端代理,可以使用此类与连接进行数据通信.
    //* 数据的接收是非主动模式,程序不可以主动请求数据,当有数据产生时系统将使用通信对象来通知.
    //----------------------------------------------------------------------------------------------------
    class HYYUN_API TcpAgent : public SelectorEvent, public Endpoint, public INoncopyable
    {
    public:
        ~TcpAgent();  // can't be inherited.

        /**
         * @brief 获得所属服务器.
         */
        inline YunServer_tcp&   getServer()    { return _rServer; }

        /**
         * @brief 获得socket.
         */
        inline SOCKET           getSocket()    { return socket(); }

        /**
         * @brief 判断代理是否关闭.
         */
        inline bool             isClosed() const { integer expected = TRANS_ATOMIC_UNSET; return atomic_compare_exchange((integer volatile*)&_atomicTransOld, &expected, TRANS_ATOMIC_UNSET); }

        /**
         * @brief 关闭当前代理.
         */
        void                    close();

        /**
         * @brief 代理当前的状态.
         */
        YunStatus               getStatus() const;
        void                    setStatus( const YunStatus& status );

        /**
         * @brief 发送整个yunStream的数据.
         *          在发送之前系统会将yunStream的引用计数加一.
         *          在发送完成之后系统会将yunStream的引用计数减一.
         *          需保证在数据发送成功之前yunStream对象有效.
         * @param yunStream
         *          使用异步的通信方式,因此在发送完成后,系统将对yunStream的引用计数减1.
         *          如不仅将yunStream用于发送给此代理,则需在发送前对yunStream的引用计数增1.
         */
        void                    writeAll( YunStream& yunStream );

        /**
         * @brief 发送数据.
         *          调用startWrite分配数据, 并对YunStream进行设置后, 然后调用finishWrite完成发送.
         *          必须形成如下的调用配对, 不然造成内存丢失: 
         *              确认发送: [startWrite - 设置流数据 - finishWrite]
         *              取消发送: [startWrite - 设置流数据 - cancelWrite]
         *          startWrite能保证一定能够分配成功, 如果暂时内存不足将等待直至分配成功.
         * @param reserveSize
         *          预分配的内存, 准确的值将会减少内存分配的次数.
         * @param yunStream
         *          必须为startWrite分配的对象.
         */
        YunStream&              startWrite( u32 reserveSize );
        bool                    finishWrite( YunStream& yunStream );
        void                    cancelWrite( YunStream& yunStream );


    protected:
        TcpAgent( YunServer_tcp& server, YunTransceiverPtr& pTrans, const struct sockaddr_storage& addr );

        //! Interface of RefCountSafe.
        virtual void        onDestroy() = 0;

        //! Interface of SelectorEvent.
        virtual SOCKET      socket();
        virtual bool        startAsync( AsyncInfo* pInfo, TcpAsyncData* pTcpInfo );
        virtual void        finishAsync( AsyncInfo* pInfo );
        virtual void        doAsync( AsyncInfo* pInfo );

        // 开始读写操作.
        bool                startRW( AsyncInfo* pInfo, TcpAsyncData* pTcpInfo, bool rightnow );

        // 数据对象分配.
        void                initAsyncInfo();
        TcpAsyncData*       getAsyncRead()  { return _pReadAsyncData; }
        AsyncInfo*			mallocAsyncWrite();
		TcpAsyncData*	    mallocTcpAsyncWrite( YunStream& yunStream );
        TcpAsyncData*       mallocTcpAsyncWriteAll( YunStream& yunStream );
        void                freeTcpAsync( TcpAsyncData* pInfo );
        void                freeAsync( AsyncInfo* pInfo );

        // info类型判断.
        inline bool         isReadInfo( AsyncInfo* pInfo )  { return pInfo == &_recvInfo; }
        inline bool         isWriteInfo( AsyncInfo* pInfo ) { return pInfo != &_recvInfo; }

        // 异步完成处理.
        void                _finishAsync_read( YunTransceiverPtr& pTrans, AsyncInfo* pInfo );
        bool                _finishAsync_read_deal( YunStream& buf, AsyncInfo* pInfo ); // 返回值表明是否继续接收消息.
        void                _finishAsync_write( YunTransceiverPtr& pTrans, AsyncInfo* pInfo );
        bool                _finishAsync_error( YunTransceiverPtr& pTrans, AsyncInfo* pInfo );

        // 消息处理.
        bool                _onReflexMsgRecv( YunStream& buf );
        bool                _onReflexMsgIllegally();
        void                _onDisconnectCallback();
        void                _onError( AsyncInfo* pInfo );

        // 内部调用.
        void         		_finishConnected();
        inline u8           _getHeartBeatFlag() const    { return _heartBeatFlag; }
        inline void         _setHeartBeatFlag( u8 flag ) { _heartBeatFlag = flag; }

    protected:
        YunServer_tcp&                      _rServer;
        YunSelector&                        _rSelector;
        TcpHeartBeat&                       _rHeartBeat;
        YunServer::Event_Msg&               _rEventMsg;
        YunTransceiverPtr                   _pTrans;         // hold pointer.
        volatile integer                    _atomicTransOld; // for atomic compare.
        volatile integer                    _atomicQueuOld;  // is in workqueue (write queue).
        u8                  		        _heartBeatFlag;  // 1-live; 0-dead.
        YunStatus                    		_status;

#ifdef CN_NET_IOCP
        AsyncInfo					        _recvInfo;
#endif // CN_NET_IOCP
        TcpAsyncData*                       _pReadAsyncData;

        friend class YunServer_tcp;
        friend class TcpHeartBeat;
        friend class TcpMessageConsumer;
    };

} // end namespace hyyun

#endif // end of define __BP_HYTCPAGENT_H_20130407162106__
