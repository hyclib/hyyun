
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy yun network framework
// author:       lipengxiang
// create at:    2014-04-03 18:02:28
// description:  
//

#ifndef __BP_YUNTCPAGENT_EPOLL_HPP_20160926180228__
#define __BP_YUNTCPAGENT_EPOLL_HPP_20160926180228__

namespace hyyun
{


    //-----------------------------------------------
    TcpAgent::~TcpAgent()
    {
        // free recv info.
        freeAsync(&_recvInfo);

        if (_pReadAsyncData)
            freeTcpAsync(_pReadAsyncData);

        freeAsync(&_writeInfo);
    }


    //-----------------------------------------------
    void TcpAgent::close()
    {
        integer atomicExpected = TRANS_ATOMIC_SET;
        if (atomic_compare_exchange(&_atomicTransOld, &atomicExpected, TRANS_ATOMIC_UNSET))
        {
            _rSelector.remove(this, &_recvInfo);
            _rSelector.remove(this, &_writeInfo);

            if (_pTrans)
            {
                _pTrans->close();
                _pTrans = NULL;
            }

            _setHeartBeatFlag(TCP_HEARTBEAT_REMOVE);

            //_rHeartBeat.erase( this );   心跳线程中会调用close方法,不能在此调用.

            this->release();
        }
    }

    //-----------------------------------------------
    bool TcpAgent::startRW(AsyncInfo* pInfo, TcpAsyncData* pTcpInfo, bool rightnow)
    {
        if (!pTcpInfo)
            return false;

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
    }

    //-----------------------------------------------
    bool TcpAgent::startAsync(AsyncInfo* pInfo, TcpAsyncData* pTcpInfo)
    {
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
                // 不在队列中则塞入.
                integer expected = IN_QUEUE_ATOMIC_UNSET;
                integer desired = IN_QUEUE_ATOMIC_SET;
                if (atomic_compare_exchange(&_atomicQueuOld, &expected, desired))
                {
                    TcpMessageData* pData = TcpMessageDataPool::malloc();
                    pData->pAgent = this;
                    pData->pInfo = pInfo;

                    SelectorEvent::addRef();

                    _rServer._pushMessageWrite(pData, ((uinteger)this) / sizeof(Connection));
                }

            }
        }
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
                if (pAsync->haveInfo()) // 重新塞入队列.
                {
                    finishAsync(pAsync);
                }
                break;
            }

            TcpAsyncData* pTcpInfo;
            if (pAsync->__info.top_front(pTcpInfo))
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
                                                                pAsync->popInfo(pTcpInfo);
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
                                                          pAsync->popInfo(pTcpInfo);
                                                          assert(pAsync->isFinishBuf(pTcpInfo));
                                                          freeTcpAsync(pTcpInfo);
                                                          continue;
                } break;
                default:
                    pAsync->popInfo(pTcpInfo);
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

        this->addRef();	// 保证选择器一定有效.
        _rSelector.enable(this, &_recvInfo);
        _rSelector.enable(this, &_writeInfo);
    }

    //-----------------------------------------------
    AsyncInfo* TcpAgent::mallocAsyncWrite()
    {
        AsyncInfo* pInfo
            = &_writeInfo;
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
    }
}

#endif // end of define __BP_YUNTCPAGENT_EPOLL_HPP_20160926180228__

