//
// Copyright(c) 1999-2013 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#include "../preheader.h"
#include "../../../include/hyyun_s/yunConnection.h"
#include "../../../include/hyyun_s/yunServerTcp.h"

#include "../pool/pools.hpp"

#include "yunServerTcp_p.hpp"

#ifdef CN_NET_EPOLL
#include "yunTcpAgent_epoll.hpp"
#else
#include "yunTcpAgent_iocp.hpp"
#endif

namespace hyyun
{
    //-----------------------------------------------
    TcpAgent::TcpAgent( YunServer_tcp& server, YunTransceiverPtr& pTrans, const struct sockaddr_storage& addr )
        : Endpoint(addr)
        , _rServer(server)
        , _rSelector(server.getSelector())
    	, _rHeartBeat(YunServer_tcp::PrivateClass::GetHeartBeat(server))
    	, _rEventMsg(server._eventMsg)
        , _pTrans(pTrans)
        , _atomicTransOld(TRANS_ATOMIC_SET)
        , _atomicQueuOld(IN_QUEUE_ATOMIC_UNSET)
        , _heartBeatFlag(TCP_HEARTBEAT_MARK)
        , _status(0)
        , _pReadAsyncData(NULL)
    {
        _rSelector.initialize(this);

        // create recv info.
        initAsyncInfo();
    }

    //-----------------------------------------------
    TcpAgent::~TcpAgent()
    {
        // free recv info.
        freeAsync(&_recvInfo);

        if (_pReadAsyncData)
            freeTcpAsync(_pReadAsyncData);

#ifdef CN_NET_EPOLL
        freeAsync(&_writeInfo);
#endif
    }

    //-----------------------------------------------
    YunStatus   TcpAgent::getStatus() const
    {
        return _status;
    }

    //-----------------------------------------------
    void TcpAgent::setStatus( const YunStatus& status )
    {
        _status = status;
    }

    //-----------------------------------------------
    void TcpAgent::close()
    {
        integer atomicExpected = TRANS_ATOMIC_SET;
        if (atomic_compare_exchange(&_atomicTransOld, &atomicExpected, TRANS_ATOMIC_UNSET))
        {
#ifdef CN_NET_EPOLL
        	_rSelector.remove(this, &_recvInfo);
        	_rSelector.remove(this, &_writeInfo);
#endif

            if (_pTrans)
            {
                _pTrans->close();
                _pTrans = NULL;
            }
            
            _setHeartBeatFlag(TCP_HEARTBEAT_REMOVE);

            //_rHeartBeat.erase( this );   心跳线程中会调用close方法,不能在此调用.

#ifdef CN_NET_EPOLL
            this->release();
#endif
        }
    }

    //-----------------------------------------------
    void TcpAgent::_onError( AsyncInfo* pInfo )
    {
        _rHeartBeat.erase( this );
        freeAsync(pInfo);
        close();
    }

    //-----------------------------------------------
    void TcpAgent::writeAll( YunStream& yunStream )
    {
        YunTransceiverPtr pTrans = _pTrans;
        if (isClosed())    return;

        yunStream.addRef();
        startRW(mallocAsyncWrite(), mallocTcpAsyncWriteAll(yunStream), true);
    }

    //-----------------------------------------------
    YunStream& TcpAgent::startWrite( u32 reserveSize )
    {
        //SelectorEvent::addRef();

//         YunStream* pBuf = YunStreamPool::mallocBuf();
//         pBuf->reserve( reserveSize );
//         return *pBuf;
        return _rServer.startBatchWrite(reserveSize);
    }

    //-----------------------------------------------
    void TcpAgent::cancelWrite( YunStream& buf )
    {
//         assert(buf.getRef() == 1);
//         YunStreamPool::freeBuf(&buf);

        _rServer.cancelBatchWrite(buf);

        //SelectorEvent::release();
    }

    //-----------------------------------------------
    bool TcpAgent::finishWrite( YunStream& yunStream )
    {
        YunTransceiverPtr pTrans = _pTrans;
        if (isClosed() || yunStream.empty()) {
            cancelWrite(yunStream);
            return false;
        }

        //assert(yunStream.isLegalYun());

        yunStream._curPos = NULL;    // 第一次开始写从begin开始.

        return startRW(mallocAsyncWrite(), mallocTcpAsyncWrite(yunStream), true);
    }

    //-----------------------------------------------
    bool TcpAgent::startRW( AsyncInfo* pInfo, TcpAsyncData* pTcpInfo, bool rightnow )
    {
        if (!pTcpInfo)
            return false;

#if defined(CN_NET_IOCP)
        if (isWriteInfo(pInfo))
            pInfo->pushBackInfo(pTcpInfo);
        return startAsync(pInfo, pTcpInfo);

#elif defined(CN_NET_EPOLL)
        if (rightnow && isWriteInfo(pInfo))
        {
            YunTransceiverPtr pTrans = _pTrans;
            if (isClosed())
            {
                freeTcpAsync(pTcpInfo);
                _onError(pInfo);
                return false;
            }
            else
            {
//             	if (pInfo->pushBackInfo(pTcpInfo))
//             	{
//             		this->_finishAsync_write(pTrans, pInfo);
//             	}
                pInfo->pushBackInfo(pTcpInfo);
                finishAsync(pInfo);
            } // if..else.
        } // if.

        return true;
#endif
    }

    //-----------------------------------------------
    bool TcpAgent::startAsync( AsyncInfo* pInfo, TcpAsyncData* pTcpInfo )
    {
#if defined(CN_NET_IOCP)
        SelectorEvent::addRef();
#endif

        YunTransceiverPtr pTrans = _pTrans;
        if (!isClosed()) 
        {
            //
            // read
            //
            if( isReadInfo(pInfo) )
            {
                //_readStream._curPos = NULL;    _readStream可能不是第一次接收, 而是一个包分多次接收, 不能=NULL.

                if (pTrans->startRead( pInfo, pTcpInfo ) )
                {
                    return true;
                }
            }
            //
            // write
            //
            else
            {
                if (pTrans->startWrite(pInfo, pTcpInfo))
                {
                    return true;
                }
            }
        }

        if (!NetStatus::IsWouldBlock(NetStatus::ErrorCode()))
            _onError(pInfo);

#if defined(CN_NET_IOCP)
        SelectorEvent::release();
#endif
        return false;
    }

    //-----------------------------------------------
    void TcpAgent::finishAsync( AsyncInfo* pInfo )
    {
		if (pInfo->isFail() || isClosed())
		{
            _onError(pInfo);
        }
        else
#if defined(CN_NET_IOCP)
        if (_finishAsync_error(_pTrans, pInfo))
#endif
        {
            if (isReadInfo(pInfo))
            {
                TcpMessageData* pData = TcpMessageDataPool::malloc();
                pData->pAgent = this;
                pData->pInfo = pInfo;

                SelectorEvent::addRef();

                _setHeartBeatFlag(TCP_HEARTBEAT_MARK);
                _rServer._pushMessageRead(pData, ((uinteger)this)/sizeof(Connection));
                
            }
            else
            {
#ifdef CN_NET_EPOLL
                // 不在队列中则塞入.
                integer expected = IN_QUEUE_ATOMIC_UNSET;
                integer desired = IN_QUEUE_ATOMIC_SET;
                if (atomic_compare_exchange(&_atomicQueuOld, &expected, desired))
#endif // CN_NET_EPOLL
                {
                    TcpMessageData* pData = TcpMessageDataPool::malloc();
                    pData->pAgent = this;
                    pData->pInfo = pInfo;

                    SelectorEvent::addRef();

                    _rServer._pushMessageWrite(pData, ((uinteger)this) / sizeof(Connection));
                }

            }
        }

#if defined(CN_NET_IOCP)
        SelectorEvent::release();
#endif
    }

    //-----------------------------------------------
    void TcpAgent::doAsync( AsyncInfo* pInfo )
    {
        YunTransceiverPtr pTrans = _pTrans;
        if (isClosed()) {
            _onError(pInfo);
        } else {

            // read.
            if (isReadInfo(pInfo)) {
                _finishAsync_read(pTrans, pInfo);
            }
            // write.
            else {
                _finishAsync_write(pTrans, pInfo);
            }

        } // if..else

        SelectorEvent::release();
    }

    //-----------------------------------------------
    bool TcpAgent::_finishAsync_error( YunTransceiverPtr& pTrans, AsyncInfo* pInfo )
    {
        // 接收错误.
        switch (pInfo->inCountBytes)
        {
        case SOCKET_ERROR:
            {
            	//print_debug_line(_T("tcp disconnect 0"));
                _onError(pInfo);
                return false;
            } break;
        case 0:
            {
            	if (NetStatus::IsWouldBlock(pInfo->inError))
            		return true;

                if (isReadInfo(pInfo))
                {
                    TcpAsyncData* pTcpInfo = getAsyncRead();

                    ByteStream* pBuf = pTcpInfo->pBuf;
                    pInfo->inCountBytes = recv( pTrans->socket(), (char*)pBuf->_curPos, pBuf->end()-pBuf->_curPos, 0 );
                    if (pInfo->inCountBytes != SOCKET_ERROR && pInfo->inCountBytes != 0)
                    {
                        pBuf->_curPos += pInfo->inCountBytes;
                        pInfo->inError = 0;
                    }
                    else
                    {
                        if (!NetStatus::IsWouldBlock(NetStatus::ErrorCode()))
                        {
                            _onError(pInfo);
                        }
                        return false;
                    }
                }
                else
                {
                    print_debug_line(_T("tcp disconnect 1"));
                    _onError(pInfo);
                    return false;
                }
            } break;
        }

        return true;
    }

    //-----------------------------------------------
    bool TcpAgent::_finishAsync_read_deal( YunStream& buf, AsyncInfo* pInfo )
    {
        s32 surplusSize = buf._curPos - buf.begin();
        if (surplusSize == 0)
        	return true;

        // 存在外部消息处理函数.
        if (!_rEventMsg.isEmpty()) {

            u32 old = buf.size();
            buf.resize(buf._curPos-buf.begin());
            _rEventMsg(buf, *this);
            buf.resize(old);
            buf._curPos = NULL;
            return true;
        }

        // 存在消息中心.
        Byte* pPtr = buf.begin();
        while (0 < surplusSize)
        {
            assert( pPtr == buf._curPos - surplusSize );
            // 接收到一个完整包.
            if (buf.analyseSelf_b(surplusSize, pPtr))
            {
                // reflex 监听者处理消息.
                if (!_onReflexMsgRecv(buf))
                {
                    _onError(pInfo);
                    return false;  // 不再接收或在_onReflexMsgRecv内接收.
                }

                assert(surplusSize >= 0);
                // 流有剩余.
                if (0 < surplusSize)
                { 
                    pPtr = buf._curPos - surplusSize;
                    continue;
                }
                // 重新读取.
                else
                {
                    assert(0 == surplusSize);
                    buf._curPos = NULL;
                    return true;
                }
            }
            // 未收到一个完整包.
            else
            {
                assert(surplusSize <= 0);
                // 非法流.
                if (0 == surplusSize)
                {
                    // 非法消息.
                    if (!_onReflexMsgIllegally())
                    {
                        _onError(pInfo);
                        return false;
                    }

                    //assert(false);
                    buf._curPos = NULL;    // 第一次开始写从begin开始.
                    return true;
                }
                // 流不够.
                else
                {
                    u32 size;

                    if (pPtr != buf.begin()) {
                        size = buf._curPos-pPtr;
                        memmove( buf.begin(), pPtr, size );
                        buf._curPos = buf.begin() + size;
                    }

                    // buf不够长.
                    if (buf._curPos >= buf.end())
                    {
                        size = buf._curPos-buf.begin();
                        u32 newSize = buf.size() + (-surplusSize);
                        if (newSize > UdpTransceiver::MAX_PACKET_SIZE)
                            newSize = UdpTransceiver::MAX_PACKET_SIZE;
                        buf.resize( newSize );
                        buf._curPos = buf.begin() + size;
                    }

                    return true;
                }
            } // if..else
        } // while

        assert(0);
        return false;
    }

    //-----------------------------------------------
    void TcpAgent::_finishAsync_read( YunTransceiverPtr& pTrans, AsyncInfo* pInfo )
    {
        TcpAsyncData* pTcpInfo = getAsyncRead();

        while (true)
        {
            YunStream& buf = *(YunStream*)pTcpInfo->pBuf;

            AsyncTransceiver::Result rt = pTrans->finishRead( pInfo, pTcpInfo );

//#ifdef CN_NET_EPOLL
//            if (AsyncTransceiver::Result_WouldBlock != rt)
//            {
//            	if (!_finishAsync_error(_pTrans, pInfo))
//            		return;
//            }
//#endif
            switch (rt)
            {
            case AsyncTransceiver::Result_moreData:
                {
                    if (_finishAsync_read_deal(buf, pInfo))
                    {
                        assert(buf.end() - buf._curPos > 0);
                        continue;
                    }
                    else
                    {
                        return;
                    }
                }
                break;
            case AsyncTransceiver::Result_WouldBlock:
                if (_finishAsync_read_deal(buf, pInfo))
                    startRW(pInfo, pTcpInfo, false);
                return;
            default:
                _onError(pInfo);
                return;
            }

        } // while.
    }

    //-----------------------------------------------
    void TcpAgent::_finishAsync_write( YunTransceiverPtr& pTrans, AsyncInfo* pAsync )
    {
        _atomicQueuOld = IN_QUEUE_ATOMIC_UNSET; // 从队列中标记移除,保证信号能到达.

        u32 count = 0;

        //
        // Deal data
        //
        while (true) 
        {
            if (++count > TCP_DO_PACKAGE_COUNT_BATCH)
            {
#ifdef CN_NET_EPOLL
                if (pAsync->haveInfo()) // 重新塞入队列.
                {
                    finishAsync(pAsync);
                }
#endif
                break;
            }

            TcpAsyncData* pTcpInfo;
#ifdef CN_NET_IOCP
            if (pAsync->popInfo(pTcpInfo))
#else
            if (pAsync->__info.top_front(pTcpInfo))
#endif
            {
                AsyncTransceiver::Result rt = pTrans->finishWrite( pAsync, pTcpInfo );

//#ifdef CN_NET_EPOLL
//                if (AsyncTransceiver::Result_WouldBlock != rt)
//                {
//                	TcpAsyncData* pTcpInfo2;
//                	pAsync->popInfo(pTcpInfo2);
//                	freeTcpAsync(pTcpInfo);
//
//                	if (!_finishAsync_error(_pTrans, pAsync))
//                		return;
//                }
//#endif

                switch (rt)
                {
                case AsyncTransceiver::Result_WouldBlock:
                    {
                        if (pAsync->isFinishBuf(pTcpInfo))
                        {
#ifdef CN_NET_EPOLL
                        	pAsync->popInfo(pTcpInfo);
#endif
                            freeTcpAsync(pTcpInfo);
                            if (!pAsync->haveInfo())
                                freeAsync(pAsync);
                        }
                        else
                        {
                            //pAsync->pushFrontInfo(pTcpInfo);
                            //startRW(pAsync, pTcpInfo, false);
                        }

                        return;
                    } break;
                case AsyncTransceiver::Result_moreData:
                    {
#ifdef CN_NET_EPOLL
                        pAsync->popInfo(pTcpInfo);
#endif
                        assert( pAsync->isFinishBuf(pTcpInfo) );
                        freeTcpAsync(pTcpInfo);
                        continue;
                    } break;
                default:
#ifdef CN_NET_EPOLL
                   	pAsync->popInfo(pTcpInfo);
#endif
                    freeTcpAsync(pTcpInfo);
                    _onError(pAsync);
                    return;
                }

            } // if.

            return;

        } // while.
    }

    //-----------------------------------------------
    bool TcpAgent::_onReflexMsgRecv( YunStream& buf )
    {
        bool illegally = false;

        // 非法消息.
        if (!buf.isLegalYun())
        {
            illegally = true;
        } else {

            if (!_rServer._onMsg(*this, buf, illegally))
                return false;

        } // if

        // 非法消息.
        if (illegally)
        {
            if (!_onReflexMsgIllegally())
            {
                return false;
            }
        }

        return true;
    }

    //-----------------------------------------------
    bool TcpAgent::_onReflexMsgIllegally()
    {
        return _rServer.event_messageIllegally()(*(Connection*)this);
    }

    //-----------------------------------------------
    void TcpAgent::_onDisconnectCallback()
    {
        _rServer.event_disconnect()(*(Connection*)this);
        Connection* pConn = (Connection*)this;
        pConn->setDataBind(NULL);
    }

    //-----------------------------------------------
    SOCKET TcpAgent::socket()
    {
        YunTransceiverPtr pT = _pTrans;
        return pT ? pT->socket() : INVALID_SOCKET;
    }

    //-----------------------------------------------
    void TcpAgent::_finishConnected()
    {
    	if (this->isClosed())
    		return;

        _setHeartBeatFlag(TCP_HEARTBEAT_MARK);

#if defined(CN_NET_IOCP)
        startAsync(&_recvInfo, getAsyncRead());

#elif defined(CN_NET_EPOLL)
        this->addRef();	// 保证选择器一定有效.
        _rSelector.enable(this, &_recvInfo);
        _rSelector.enable(this, &_writeInfo);

#endif // CN_NET_EPOLL
    }

    //-----------------------------------------------
    AsyncInfo* TcpAgent::mallocAsyncWrite()
    {
        AsyncInfo* pInfo 
#if defined(CN_NET_IOCP)
            = AsyncInfoPool::malloc();
#elif defined(CN_NET_EPOLL)
            = &_writeInfo;
#endif
        return pInfo;
    }

    //-----------------------------------------------
    TcpAsyncData* TcpAgent::mallocTcpAsyncWrite( YunStream& yunStream )
    {
        TcpAsyncData* pTcpInfo = TcpAsyncDataPool::malloc();
        pTcpInfo->pBuf = &yunStream;
        pTcpInfo->pBuf->_curPos = yunStream.begin();

        return pTcpInfo;
    }

    //-----------------------------------------------
    TcpAsyncData* TcpAgent::mallocTcpAsyncWriteAll( YunStream& yunStream )
    {
        TcpAsyncData* pTcpInfo = TcpAsyncDataPool::malloc();
        pTcpInfo->pBuf = &yunStream;
        pTcpInfo->pPosWriteAll = yunStream.begin();

        return pTcpInfo;
    }

    //-----------------------------------------------
    void TcpAgent::initAsyncInfo()
    {
        YunStream::DeallocCallback cb = NULL;
        _rServer._onStreamDeallocCallback(cb);

        // read.
        YunStream* pStream = YunStreamPool::mallocRef(cb);
        YunStreamAnalyser* pAnaylser = _rServer.getPacketAnalyser().get();
        if (pAnaylser)  pStream->setAnalyser(pAnaylser);

        pStream->resize(_rServer.getParam().recvBufLength);
        pStream->_curPos = NULL;
        _rServer._onStreamMalloc(false, *pStream);

        _pReadAsyncData = TcpAsyncDataPool::malloc();
        _pReadAsyncData->pBuf = pStream;
        _recvInfo.pushBackInfo(_pReadAsyncData);
    }

    //-----------------------------------------------
    void TcpAgent::freeTcpAsync( TcpAsyncData* pInfo )
    {
        YunStreamPool::freeRef((YunStream*)pInfo->pBuf);
        TcpAsyncDataPool::free(pInfo);
    }

    //-----------------------------------------------
    void TcpAgent::freeAsync( AsyncInfo* pInfo )
    {
        if (isWriteInfo(pInfo))
        {
            TcpAsyncData* pTcpInfo;
            while (pInfo->popInfo(pTcpInfo))
            {
                freeTcpAsync(pTcpInfo);
            }
        }

#if defined(CN_NET_IOCP)
        if (isReadInfo(pInfo))
            return;

        AsyncInfoPool::free(pInfo);
        return;
#endif
    }


} // end namespace hyyun
