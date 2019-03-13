//
// Copyright(c) 1999-2013 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#include "../preheader.h"
#include "../../../include/hyyun_s/yunUdpAgent.h"
#include "../../../include/hyyun_s/yunServerUdp.h"

#include "udpAsyncInfo.h"
#include "udpMessageConsumer.h"
#include "../pool/pools.hpp"


namespace hyyun
{
    //-----------------------------------------------
    UdpAgent::UdpAgent( YunServer_udp& server, YunUdpTransceiver& trans, YunServer::Event_Msg* event )
        : _rServer(server)
        , _rSelector(server.getSelector())
        , _rEventMsg(server._eventMsg)
        , _rTrans(trans)
        , _atomicTransOld(TRANS_ATOMIC_SET)
        , _pReadAsyncData(NULL)
    {
        _rSelector.initialize(this);
        initAsyncInfo();
    }

    //-----------------------------------------------
    UdpAgent::~UdpAgent()
    {
        if (_pReadAsyncData)
            freeUdpAsync(_pReadAsyncData);
        close();
    }

    //-----------------------------------------------
    void UdpAgent::close()
    {
        integer atomicExpected = TRANS_ATOMIC_SET;
        if (atomic_compare_exchange(&_atomicTransOld, &atomicExpected, TRANS_ATOMIC_UNSET))
        {
            _rTrans.close();
        }
    }

    //-----------------------------------------------
    void UdpAgent::_onError( AsyncInfo* async )
    {
        freeAsync(async);

        if (!isClosed())
        {
            close();
            _rServer._callEventError();
        }
    }

    //-----------------------------------------------
    void UdpAgent::start()
    {
        // 投递.
        startAsync( &_recvInfo, getReadAsync() );

#ifdef CN_NET_EPOLL
        startAsync( &_writeInfo, NULL );
#endif // CN_NET_EPOLL
    }

    //-----------------------------------------------
    bool UdpAgent::startRW( AsyncInfo* pInfo, UdpAsyncData* pData, bool rightnow )
    {
        if (!pData)
            return false;

#if defined(CN_NET_IOCP)
        if (isWriteInfo(pInfo))
            pInfo->pushBackInfo(pData);
        return startAsync(pInfo, pData);
#elif defined(CN_NET_EPOLL)
        if (rightnow) 
        {
            if (isWriteInfo(pInfo))
            {
                if (isClosed())
                {
                    freeUdpAsync(pData);
                    freeAsync(pInfo);
                    return false;
                }
                else
                {
                    AsyncTransceiver::Result rt = _finishAsync_write_once(pInfo, pData);
                    switch (rt)
                    {
                    case AsyncTransceiver::Result_Error:
                        freeUdpAsync(pData);
                        freeAsync(pInfo);
                        return false;
                        break;
                    case AsyncTransceiver::Result_moreData:
                        assert(pInfo->isFinishBuf(pData));
                        freeUdpAsync(pData);
                        return true;
                        break;
                    case AsyncTransceiver::Result_WouldBlock:
                        if (pInfo->isFinishBuf(pData))
                        {
                            freeUdpAsync(pData);
                            return true;
                        }
                        else
                        {
                            pInfo->pushBackInfo(pData);
                            return false;
                        }
                        break;
                    default:
                        assert(0);
                        return false;
                        break;
                    }
                } // if..else
            }
            else
            {
                assert(0);
            }
        } // if.

        return true;
#endif
    }

    //-----------------------------------------------
    void UdpAgent::writeAll( YunStream& yunStream, const Endpoint& ep )
    {
        if (isClosed())    return;

        startRW(mallocAsyncWrite(), mallocUdpAsyncWrite(ep, yunStream), true);
    }

    //-----------------------------------------------
    YunStream& UdpAgent::startWrite( u32 reserveSize )
    {
        //SelectorEvent::addRef();

        YunStream::DeallocCallback cb = NULL;
        _rServer._onStreamDeallocCallback(cb);

        YunStream* pBuf = YunStreamPool::mallocRef(cb);
        pBuf->reserve( reserveSize );
        _rServer._onStreamMalloc(true, *pBuf);
        return *pBuf;
    }

    //-----------------------------------------------
    void UdpAgent::cancelWrite( YunStream& buf )
    {
        //assert(buf.getRef() == 1);
        YunStreamPool::freeRef(&buf);

        //SelectorEvent::release();
    }

    //-----------------------------------------------
    bool UdpAgent::finishWrite( YunStream& yunStream, const Endpoint& ep )
    {
        if (isClosed() || yunStream.empty()) {
            cancelWrite(yunStream);
            return false;
        }

        //assert(yunStream.isLegalYun());

        yunStream._curPos = NULL;    // 第一次开始写从begin开始.

        return startRW(mallocAsyncWrite(), mallocUdpAsyncWrite(ep, yunStream), true);

        //SelectorEvent::release();
    }

    //-----------------------------------------------
    YunStream& UdpAgent::startBatchWrite( u32 reserveSize )
    {
        return startWrite(reserveSize);
    }

    //-----------------------------------------------
    void UdpAgent::cancelBatchWrite( YunStream& buf )
    {
        return cancelWrite(buf);
    }

    //-----------------------------------------------
    void UdpAgent::finishBatchWrite( YunStream& yunStream, const Array<Endpoint>& arrEp )
    {
        //assert(yunStream.isLegalYun());

        if (yunStream.empty() || isClosed() || arrEp.empty())
        {
            cancelBatchWrite(yunStream);
            return;
        }
        else
        {
            u32 i;

            // 分配投递对象.
            Array<AsyncInfo*> arrAsync;
            arrAsync.reserve(arrEp.size());

            Array<UdpAsyncData*> arrUdpData;
            arrUdpData.reserve(arrEp.size());

            for (i = 0; i < arrEp.size(); i++)
            {
                if (!arrEp[i].empty())
                {
                    yunStream.addRef();
                    arrAsync.push_back( mallocAsyncWrite() );
                    arrUdpData.push_back( mallocUdpAsyncWrite(arrEp[i], yunStream) );
                }
            } // for

            // fix ref count.
            //assert(yunStream.getRef() >= 2);
            cancelBatchWrite(yunStream);

            // 投递.
            for (i = 0; i < arrAsync.size(); i++)
            {
                startRW(arrAsync[i], arrUdpData[i], true);
            } // for

        } // if..else

        //SelectorEvent::release();
    }

    //-----------------------------------------------
    bool UdpAgent::startAsync( AsyncInfo* pInfo, AsyncData* data )
    {
        bool r = true;

#ifdef CN_NET_IOCP
        SelectorEvent::addRef();
#endif

        assert(pInfo);

        if (!isClosed())
        {
            UdpAsyncData* pUdpAsync = (UdpAsyncData*)data;

            //
            // read
            //
            if( isReadInfo(pInfo) )
            {
                pUdpAsync->ep.reset();
                if ( _rTrans.startReadFrom( pInfo, pUdpAsync, (sockaddr_storage*)&pUdpAsync->ep.endpointAddr ) )
                {
                    return r;
                }

                r = false;
            }
            //
            // write
            //
            else
            {
                if ( _rTrans.startWriteTo( pInfo, pUdpAsync, (sockaddr_storage*)&pUdpAsync->ep.endpointAddr ) )
                {
                    return r;
                }

                r = false;
            }


            if (!NetStatus::IsWouldBlock(NetStatus::ErrorCode()))
                _onError(pInfo);
        }
        else
        {
            r = false;
        }

#ifdef CN_NET_IOCP
        SelectorEvent::release();
#endif
        return r;
    }


    //-----------------------------------------------
    void UdpAgent::finishAsync( AsyncInfo* pInfo )
    {
        if (pInfo->isFail() || isClosed())
        {
            _onError(pInfo);
        } 
        else 
#ifdef CN_NET_IOCP
            if (_finishAsync_error(pInfo)) 
#endif // CN_NET_IOCP
            {
                if (isReadInfo(pInfo))
                {
                    _finishAsync_read(pInfo);
                }
                else
                {
                    //SelectorEvent::addRef();
                    _rServer._pushMessageWrite(pInfo, -1);
                }
            } // if..else

#ifdef CN_NET_IOCP
            SelectorEvent::release();
#endif // CN_NET_IOCP
    }

    //-----------------------------------------------
    bool UdpAgent::_finishAsync_error( AsyncInfo* pAsync )
    {
        assert(pAsync);

        // error.
        switch (pAsync->inCountBytes)
        {
        case SOCKET_ERROR:
            {
                trace_all(_T("server _finishAsync_error"));
                _onError(pAsync);
                return false;
            } break;
        case 0:
            {
                if (isReadInfo(pAsync))
                {
                    return true;
                }

                //print_debug_line(_T("tcp disconnect 1"));
                _onError(pAsync);
                return false;
            } break;
        }
        return true;
    }

    //-----------------------------------------------
    void UdpAgent::doAsync_read( UdpMessageData* pData )
    {
        // 外部处理函数.
        if (!_rEventMsg.isEmpty()) {
            _rEventMsg(*(YunStream*)pData->pBuf, pData->ep);
            return;
        } 
        // 存在消息中心.
        else {

            YunStream& buf = *(YunStream*)pData->pBuf;

            // 存在消息中心.
            s32 surplusSize = buf._curPos - buf.begin();
            Byte* pPtr = buf.begin();
            while (0 < surplusSize)
            {
                assert( pPtr == buf._curPos - surplusSize );
                // 接收到一个完整包.
                if (buf.analyseSelf_b(surplusSize, pPtr))
                {
                    // reflex 监听者处理消息.
                    _onReflexMsgDeal(buf, pData->ep); // 沙盒处理.

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
                        return;
                    }
                }
                // 未收到一个完整包.
                else
                {
                    assert(surplusSize <= 0);

                    // 非法消息.
                    _onReflexMsgIllegally(pData->ep);

                    return;

                } // if..else
            } // while

        } // if..else.

        return;
    }

    //-----------------------------------------------
    bool UdpAgent::_finishAsync_read_deal( YunStream& buf, AsyncInfo* pInfo, const Endpoint& ep )
    {
        UdpMessageData* pData = mallocMsgData( ep, buf, 0, buf._curPos-buf.begin());
        //SelectorEvent::addRef();
        _rServer._pushMessageRead(pData, -1);

        buf._curPos = NULL;    // 第一次开始写从begin开始.
        return true;
    }

    //-----------------------------------------------
    void UdpAgent::_finishAsync_read( AsyncInfo* pAsync )
    {
        assert(pAsync);

        UdpAsyncData* pUdpAsync = getReadAsync();

        while (true)
        {
            AsyncTransceiver::Result rt = _rTrans.finishRead( pAsync, pUdpAsync );

#ifdef CN_NET_EPOLL
            if (!_finishAsync_error(pAsync))
                return;
#endif // CN_NET_EPOLL

            switch (rt)
            {
            case AsyncTransceiver::Result_Error:
                _onError(pAsync);
                return;
            case AsyncTransceiver::Result_moreData:
                {
                    if (_finishAsync_read_deal(*(YunStream*)pUdpAsync->pBuf, pAsync, pUdpAsync->ep))
                        continue;
                    else
                        return;
                }
                break;
            case AsyncTransceiver::Result_WouldBlock:
                {
                    if (_finishAsync_read_deal(*(YunStream*)pUdpAsync->pBuf, pAsync, pUdpAsync->ep))
                        startRW(pAsync, pUdpAsync, false);
                }
                return;
            default:
                _onError(pAsync);
                assert(0);
                error_all(_T("UdpAgent::_finishAsync_read error"));
                return;
            }

        } // while.
    }

    //-----------------------------------------------
    void UdpAgent::_finishAsync_write(AsyncInfo* pAsync )
    {
        assert(pAsync);

        while (true)
        {
            AsyncData* pData;
            if (pAsync->popInfo(pData))
            {
                UdpAsyncData* pUdpData = (UdpAsyncData*)pData;
                AsyncTransceiver::Result rt = _finishAsync_write_once(pAsync, pUdpData);

                switch (rt)
                {
                case AsyncTransceiver::Result_Error:
                    {
                        freeUdpAsync(pUdpData);
                        return;
                    } break;
                case  AsyncTransceiver::Result_WouldBlock:
                    {
                        if (pAsync->isFinishBuf(pUdpData))
                        {
                            freeUdpAsync(pUdpData);
                            if (pAsync->haveInfo())
                                startRW(pAsync, NULL, false);
                        }
                        else
                        {
                            pAsync->pushFrontInfo(pUdpData);
                            startRW(pAsync, pUdpData, false);
                        }
                        return;
                    } break;
                case AsyncTransceiver::Result_moreData:
                    {
                        assert( pAsync->isFinishBuf(pUdpData) );
                        freeUdpAsync(pUdpData);
                        if (pAsync->haveInfo())
                        {
                            continue;
                        }
                        else
                        {
                            startRW(pAsync, NULL, false);
                            return;
                        }
                    } break;
                default:
                    assert(0);
                    _onError(pAsync);
                    error_all(_T("UdpAgent::_finishAsync_write error"));
                    return;
                }

            } // if.
        } // while.
    }

    //-----------------------------------------------
    AsyncTransceiver::Result UdpAgent::_finishAsync_write_once( AsyncInfo* pAsync, UdpAsyncData* pData )
    {
        AsyncTransceiver::Result rt 
            = _rTrans.finishWrite( pAsync, pData );

#ifdef CN_NET_EPOLL
        if (!_finishAsync_error(pAsync)) {
            return AsyncTransceiver::Result_Error;
        }
#endif // CN_NET_EPOLL

        if (rt == AsyncTransceiver::Result_Error)
        {
            _onError(pAsync);
        }

        return rt;
    }

    //-----------------------------------------------
    void UdpAgent::_onReflexMsgDeal( YunStream& readStream, Endpoint& ep )
    {
        if (!readStream.isLegalYun())
        {
            _onReflexMsgIllegally(ep);
            return;
        }

        bool illegally = false;
        _rServer._onMsg(ep, readStream, illegally);
        if (illegally)
            _onReflexMsgIllegally(ep);
    }

    //-----------------------------------------------
    void UdpAgent::_onReflexMsgIllegally( Endpoint& ep )
    {
        _rServer._callEventMsgIllegally(ep);
    }


    //-----------------------------------------------
    void UdpAgent::initAsyncInfo()
    {
        YunStream::DeallocCallback cb = NULL;
        _rServer._onStreamDeallocCallback(cb);

        YunStream* pStream = YunStreamPool::mallocRef(cb);
        if (_rServer.getPacketAnalyser())
            pStream->setAnalyser(_rServer.getPacketAnalyser().get());
        pStream->resize(SER_RECV_BUF_LENGTH);
        pStream->_curPos = NULL;

        _rServer._onStreamMalloc(false, *pStream);

        _pReadAsyncData = UdpAsyncDataPool::malloc();
        _pReadAsyncData->pBuf = pStream;
        _recvInfo.pushBackInfo(_pReadAsyncData);

#ifdef CN_NET_EPOLL
        _recvInfo.__pHandler = this;
        _writeInfo.__pHandler = this;
#endif // CN_NET_EPOLL
    }


    //-----------------------------------------------
    UdpMessageData* UdpAgent::mallocMsgData( const Endpoint& ep, YunStream& yunStream, u32 offset, u32 len )
    {
        UdpMessageData* pData = UdpMessageDataPool::malloc();
        pData->ep   = ep;

        if (len > 0)
        {
            YunStream::DeallocCallback cb = NULL;
            _rServer._onStreamDeallocCallback(cb);

            YunStream* pStream = YunStreamPool::mallocRef(cb);
            YunStreamAnalyser* pAnalyser = _rServer.getPacketAnalyser().get();
            if (pAnalyser)
                pStream->setAnalyser(pAnalyser);
            pData->pBuf = pStream;
            pStream->resize(len);
            pStream->setPosition(0);
            pStream->writeRaw(yunStream.begin()+offset, len);
            pStream->_curPos = pStream->end();
            _rServer._onStreamMalloc(false, *pStream);
        }
        else
        {
            pData->pBuf = NULL;
        }
        return pData;
    }

    //-----------------------------------------------
    void UdpAgent::freeMsgData( UdpMessageData* pData )
    {
        if (pData->pBuf)
            YunStreamPool::freeRef((YunStream*)pData->pBuf);
        UdpMessageDataPool::free(pData);
    }

    //-----------------------------------------------
    AsyncInfo* UdpAgent::mallocAsyncWrite()
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
    UdpAsyncData* UdpAgent::mallocUdpAsyncWrite( const Endpoint& ep, YunStream& yunStream )
    {
        UdpAsyncData* pUdpInfo = UdpAsyncDataPool::malloc();
        pUdpInfo->pBuf = &yunStream;
        pUdpInfo->ep = ep;
        pUdpInfo->pPosWriteAll = yunStream.begin();

        return pUdpInfo;
    }

    //-----------------------------------------------
    void UdpAgent::freeUdpAsync( UdpAsyncData* pInfo )
    {
        YunStreamPool::freeRef((YunStream*)pInfo->pBuf);
        UdpAsyncDataPool::free(pInfo);
    }

    //-----------------------------------------------
    void UdpAgent::freeAsync( AsyncInfo* async )
    {
        if (isWriteInfo(async))
        {
            AsyncData* data;
            while (async->popInfo(data))
            {
                freeUdpAsync((UdpAsyncData*)data);
            }
        }

#if defined(CN_NET_IOCP)
        if (isReadInfo(async))
            return;

        AsyncInfoPool::free(async);
#endif
    }

} // end namespace hyyun
