//
// Copyright(c) 1999-2011 hyrj. All rights reserved.
// base network libaray
// author:       lipengxiang
// description:  
//

#include "../preheader.h"
#include "../../../include/hyyun_s/yunServerTcp.h"

#include "tcpHeartBeat.h"
#include "tcpHeartBeatThread.h"
#include "tcpConnectEvent.h"
#include "tcpMessageConsumer.h"

#include "../thread/serverMemoryPoolThread.h"
#include "../pool/pools.hpp"

#include "yunServerTcp_p.hpp"
#include "tcpConnectConsumer.h"

namespace hyyun
{
    
    //----------------------------------------------------------------------------------------------------
    //! YunServer
    //----------------------------------------------------------------------------------------------------

    //-----------------------------------------------
    YunServer_tcp::YunServer_tcp( const YunServerParam_tcp& param )
        : YunServer(param.timeout, param.threadNum, param.queLenRead, param.queLenWrite)
    {
        PRIVATE_CLASS_CREATE(param, _selector);

        PRIVATE_CLASS_GET(p);
        p->_pConnectMessageQueue = new hymq::MessageQueue(new TcpConnectConsumer(*this), param.queLenRead);
        if (param.recvBufLength != 0)
        {
            p->_param.recvBufLength = Math::Min(param.recvBufLength, YunStreamPool::getMaxSize());
        }
    }
    
    //-----------------------------------------------
    YunServer_tcp::~YunServer_tcp()
    {
        stop();

        PRIVATE_CLASS_DESTROY;
    }

    //-----------------------------------------------
    const hyyun::YunServerParam_tcp& YunServer_tcp::getParam() const
    {
        PRIVATE_CLASS_GET_CONST(p);
        return p->_param;
    }

    //-----------------------------------------------
    void YunServer_tcp::pauseAccept()
    {
        PRIVATE_CLASS_GET(p);
        p->_allowAccept = false;
    }

    //-----------------------------------------------
    void YunServer_tcp::resumeAccept()
    {
        PRIVATE_CLASS_GET(p);
        p->_allowAccept = true;
    }

    //-----------------------------------------------
    bool YunServer_tcp::setMaxConnectionNum( u32 count )
    {
        PRIVATE_CLASS_GET(p);
        if (isRunning())
            return false;
        p->_maxConnectionNum = count;
        return true;
    }

    //-----------------------------------------------
    hyc::s32 YunServer_tcp::getMaxConnectionNum() const
    {
        PRIVATE_CLASS_GET_CONST(p);
        return p->_maxConnectionNum;
    }

    //-----------------------------------------------
    hyc::u32 YunServer_tcp::getCurConnectedNum() const
    {
        PRIVATE_CLASS_GET_CONST(p);
        return p->_heartBeat.size();
    }

    //-----------------------------------------------
    void hyyun::YunServer_tcp::stop()
    {
        PRIVATE_CLASS_GET(p);
        if (!isRunning())   return;

        _isRunning = false;

        // 连接handler.
        if (p->_connectEventPtr.get())
        {
            p->_connectEventPtr->finished();
            p->_connectEventPtr.release();
        }

        // 心跳.
        if (p->_pHeartBeatThread)
        {
            p->_pHeartBeatThread->exit();
            p->_pHeartBeatThread->abort();
//             p->_pHeartBeatThread->join();
            p->_pHeartBeatThread.release();
        }

        // 移除全部的心跳.
        p->_heartBeat.eraseAll();

        // selector
        _selector.destroy();

        // 等待所有线程完成.
        _exitWorkItems();
        
        p->_acceptor.close();
        p->_pConnectMessageQueue->stopImmediately();
    }


    //-----------------------------------------------
    void YunServer_tcp::stopWaitAll()
    {
        PRIVATE_CLASS_GET(p);
        if (!isRunning())   return;

        // 连接handler.
        if (p->_connectEventPtr.get())
        {
            p->_connectEventPtr->finished();
            p->_connectEventPtr.release();
        }

        // 等待全部的连接退出.
        p->_heartBeat.waitForEmpty();

        stop();
    }

    //-----------------------------------------------
    bool YunServer_tcp::go( const String& host1, int port, ProtocolSupport protocol )
    {
        String host = host1;
        if (host.empty())
        {
            host = SocketUtils::AddrToStr(SocketUtils::AddrLocal());
        }

        PRIVATE_CLASS_GET(p);
        if (isRunning())
        {
            trace_all( _T("tcp服务器已经在运行") );
            return false;
        }

        _isRunning = true;

        //
        // create consumer.
        //
        Array<hymq::MessageConsumer*> arrConsumer;
        Array<hymq::MessageConsumer*> arrConsumerWrite;
        for (u32 i = 0; i < _threadNum; i++)
        {
            arrConsumer.push_back(new TcpMessageConsumer);
            arrConsumerWrite.push_back(new TcpMessageConsumer);
        }
        _go(host, port, protocol, arrConsumer, arrConsumerWrite);
        p->_pConnectMessageQueue->start();

        if (!p->_acceptor.listen( host, port, protocol ))
        {
            p->_acceptor.close();
            _isRunning = false;
            return false;
        }

        //
        // 接收工作项
        //
        p->_connectEventPtr = new TcpConnectEvent(p->_acceptor, *this, p->_allowAccept);
        assert(p->_connectEventPtr.get());

        // 绑定接收器至selector.
        p->_connectEventPtr->startConnect();    // 投递一个连接.

        //
        // 主工作项;
        //
        _initWorkItems(_selector);

        //
        // 心跳包.
        //
        p->_pHeartBeatThread = new TcpHeartBeatThread(p->_heartBeat, p->_param.heartBeatInterval);
        p->_pHeartBeatThread->start();

        return true;
    }

    //-----------------------------------------------
    YunServer_tcp::Event_Connected& YunServer_tcp::event_connected()
    {
        PRIVATE_CLASS_GET(p);
        return p->_eventConnected;
    }

    //-----------------------------------------------
    YunServer_tcp::Event_Disconnect& YunServer_tcp::event_disconnect()
    {
        PRIVATE_CLASS_GET(p);
        return p->_eventDisconnected;
    }

    //-----------------------------------------------
    YunServer_tcp::Event_MessageIllegally& YunServer_tcp::event_messageIllegally()
    {
        PRIVATE_CLASS_GET(p);
        return p->_eventMessageIllegally;
    }

    //-----------------------------------------------
    void YunServer_tcp::_onConnected( Connection& newConnection )
    {
        PRIVATE_CLASS_GET(p);
        newConnection._setHeartBeatFlag(TCP_HEARTBEAT_MARK);
        //p->_heartBeat.addConn(&newConnection);

        TcpConnectMessageData* pData = TcpConnectMessageDataPool::malloc();
        pData->pConn = &newConnection;

        p->_pConnectMessageQueue->push(pData);
    }

    //-----------------------------------------------
    void YunServer_tcp::_onDealNewConnection( ConnectionPtr& pConn )
    {
        PRIVATE_CLASS_GET(p);

        if (!pConn->isClosed() && event_connected()(*pConn))
        {
            p->_heartBeat.addConn(pConn.get());

            // 投入接收池.
            pConn->_finishConnected();
        }
    }


    //-----------------------------------------------
    bool YunServer_tcp::_onMsg( TcpAgent& agent, YunStream& buf, OUT bool& illegally )
    {
        // 可能是心跳包.
        if (0 == buf.getColNum()) return true;

        // 非法消息.
        if ( buf.getColType(0) != YunStream::Type_u16 )
        {
            illegally = true;
        }
        else
        {
            u16 mark;
            HY_VERIFY( buf.getColValue(0, mark) );

            YunServerHandlerPtr& obj = getHandler(mark);
            if (obj)
            {
                if (agent.getStatus() == obj->getStatus())
                    return obj->onMessageRecv(agent, buf, NULL);
            }
            illegally = true;

        } // if..else

        return true;
    }

    //-----------------------------------------------
    YunStream& YunServer_tcp::startBatchWrite( u32 reserveSize )
    {
        YunStream::DeallocCallback cb = NULL;
        _onStreamDeallocCallback(cb);

        YunStream* pBuf = YunStreamPool::mallocRef(cb);
        if (_pAnalyser) pBuf->setAnalyser(_pAnalyser.get());
        pBuf->reserve(reserveSize);
        _onStreamMalloc(true, *pBuf);
        return *pBuf;
    }

    //-----------------------------------------------
    void YunServer_tcp::cancelBatchWrite( YunStream& yunStream )
    {
        YunStreamPool::freeRef(&yunStream);
    }

    //-----------------------------------------------
    void YunServer_tcp::finishBatchWrite( YunStream& yunStream, ConnectionPtr arrEp[], u32 arrSize )
    {
        assert(/*yunStream.isLegalYun() && */arrEp && yunStream.getRef() == 1);

        if (yunStream.empty() || 0 == arrSize)
        {
            YunStreamPool::freeRef(&yunStream);
        }
        else
        {
            for (u32 i = 0; i < arrSize; i++)
            {
                if (arrEp[i])
                {
                    arrEp[i]->writeAll(yunStream);
                }
            }
            YunStreamPool::freeRef(&yunStream);
        } // if..else
    }

} // end namespace hyyun

