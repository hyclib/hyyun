
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy yun network framework
// author:       lipengxiang
// create at:    2014-04-03 18:02:28
// description:  
//

#ifndef __BP_YUNTCPAGENT_IOCP_HPP_20160926180228__
#define __BP_YUNTCPAGENT_IOCP_HPP_20160926180228__

namespace hyyun
{

    //-----------------------------------------------
    TcpAgent::~TcpAgent()
    {
        // free recv info.
        freeAsync(&_recvInfo);

        if (_pReadAsyncData)
            freeTcpAsync(_pReadAsyncData);
    }

    //-----------------------------------------------
    void TcpAgent::close()
    {
        integer atomicExpected = TRANS_ATOMIC_SET;
        if (atomic_compare_exchange(&_atomicTransOld, &atomicExpected, TRANS_ATOMIC_UNSET))
        {
            if (_pTrans)
            {
                _pTrans->close();
                _pTrans = NULL;
            }

            _setHeartBeatFlag(TCP_HEARTBEAT_REMOVE);

            //_rHeartBeat.erase( this );   心跳线程中会调用close方法,不能在此调用.
        }
    }

    //-----------------------------------------------
    bool TcpAgent::startRW(AsyncInfo* pInfo, TcpAsyncData* pTcpInfo, bool rightnow)
    {
        if (!pTcpInfo)
            return false;

        if (isWriteInfo(pInfo))
            pInfo->pushBackInfo(pTcpInfo);
        return startAsync(pInfo, pTcpInfo);
    }

    //-----------------------------------------------
    bool TcpAgent::startAsync(AsyncInfo* pInfo, TcpAsyncData* pTcpInfo)
    {
        SelectorEvent::addRef();

        YunTransceiverPtr pTrans = _pTrans;
        if (!isClosed())
        {
            //
            // read
            //
            if (isReadInfo(pInfo))
            {
                //_readStream._curPos = NULL;    _readStream可能不是第一次接收, 而是一个包分多次接收, 不能=NULL.

                if (pTrans->startRead(pInfo, pTcpInfo))
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

        SelectorEvent::release();
        return false;
    }

    //-----------------------------------------------
    void TcpAgent::finishAsync(AsyncInfo* pInfo)
    {
        if (pInfo->isFail() || isClosed())
        {
            _onError(pInfo);
        }
        else
        if (_finishAsync_error(_pTrans, pInfo))
        {
            if (isReadInfo(pInfo))
            {
                TcpMessageData* pData = TcpMessageDataPool::malloc();
                pData->pAgent = this;
                pData->pInfo = pInfo;

                SelectorEvent::addRef();

                _setHeartBeatFlag(TCP_HEARTBEAT_MARK);
                _rServer._pushMessageRead(pData, ((uinteger)this) / sizeof(Connection));

            }
            else
            {
                {
                    TcpMessageData* pData = TcpMessageDataPool::malloc();
                    pData->pAgent = this;
                    pData->pInfo = pInfo;

                    SelectorEvent::addRef();

                    _rServer._pushMessageWrite(pData, ((uinteger)this) / sizeof(Connection));
                }

            }
        }

        SelectorEvent::release();
    }

    //-----------------------------------------------
    void TcpAgent::_finishAsync_write(YunTransceiverPtr& pTrans, AsyncInfo* pAsync)
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
                break;
            }

            TcpAsyncData* pTcpInfo;
            if (pAsync->popInfo(pTcpInfo))
            {
                AsyncTransceiver::Result rt = pTrans->finishWrite(pAsync, pTcpInfo);

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
                                                          assert(pAsync->isFinishBuf(pTcpInfo));
                                                          freeTcpAsync(pTcpInfo);
                                                          continue;
                } break;
                default:
                    freeTcpAsync(pTcpInfo);
                    _onError(pAsync);
                    return;
                }

            } // if.

            return;

        } // while.
    }

    //-----------------------------------------------
    void TcpAgent::_finishConnected()
    {
        if (this->isClosed())
            return;

        _setHeartBeatFlag(TCP_HEARTBEAT_MARK);

        startAsync(&_recvInfo, getAsyncRead());
    }

    //-----------------------------------------------
    AsyncInfo* TcpAgent::mallocAsyncWrite()
    {
        AsyncInfo* pInfo
            = AsyncInfoPool::malloc();
        return pInfo;
    }

    //-----------------------------------------------
    void TcpAgent::freeAsync(AsyncInfo* pInfo)
    {
        if (isWriteInfo(pInfo))
        {
            TcpAsyncData* pTcpInfo;
            while (pInfo->popInfo(pTcpInfo))
            {
                freeTcpAsync(pTcpInfo);
            }
        }

        if (isReadInfo(pInfo))
            return;

        AsyncInfoPool::free(pInfo);
        return;
    }

}

#endif // end of define __BP_YUNTCPAGENT_IOCP_HPP_20160926180228__
