// (c) 1999-2011 hyrj. All rights reserved.
// base network libaray
// author:       lipengxiang
// description:  
//

#include "preheader.h"
#include "yunClientMessageCenter.h"
#include "thread/clientMessageThread.h"
#include "thread/clientHeartBeatThread.hpp"

#include "yunClient_p.hpp"

namespace hyyun
{
    
    //----------------------------------------------------------------------------------------------------
    //! YunClient
    //----------------------------------------------------------------------------------------------------
    
    //-----------------------------------------------
    YunClient::YunClient( ProtocolType protocol, s32 timeout /*= -1*/ )
    {
        PRIVATE_CLASS_CREATE(*this, protocol, timeout);
        //PRIVATE_CLASS_GET(p);
        setHeartBeatElapse();
    }
    
    //-----------------------------------------------
    YunClient::~YunClient()
    {
        PRIVATE_CLASS_GET(p);
        if (!p->_extSocket)
            disconnect();

        autoHeartBeat(false);
        autoRecvMessage(false);

        YunStream::DeallocCallback deallocCB = _onGetStreamDeallocCallback();
        if (deallocCB)
        {
            deallocCB(&p->_recvBuf);
            deallocCB(&p->_recvBuf2);
            deallocCB(&p->_heartBeat);
        }

        PRIVATE_CLASS_DESTROY;
    } 


    //-----------------------------------------------
    SOCKET YunClient::getSocket()
    {
        PRIVATE_CLASS_GET(p);
        if (!p->_pTransceiver.get())    return INVALID_SOCKET;
        return p->_pTransceiver->socket();
    }

    //-----------------------------------------------
    bool YunClient::sendHeartBeat()
    {
        PRIVATE_CLASS_GET(p);

        p->_heartBeat._curPos = NULL;
        return sendMessage( p->_heartBeat ); 
    }

    //-----------------------------------------------
    hyc::s32 YunClient::getTimeout() const
    {
        PRIVATE_CLASS_GET_CONST(p);
        return p->_timeout;
    }
    
    //-----------------------------------------------
    void YunClient::setTimeout( s32 timeout )
    {
        PRIVATE_CLASS_GET(p);
        p->_timeout = timeout;

        if (p->_pTransceiver.get() && p->_pTransceiver->socket() != INVALID_SOCKET)
        {
            SocketUtils::SetSendTimeout( p->_pTransceiver->socket(), timeout );
            SocketUtils::SetRevTimeout( p->_pTransceiver->socket(), timeout );
        }
    }
    
    //-----------------------------------------------
    bool YunClient::isDisconnect() const
    {
        PRIVATE_CLASS_GET_CONST(p);
        return p->isDisconnect();
    }
    
    //-----------------------------------------------
    bool YunClient::connect( const String& host1, PORT port, ProtocolSupport protocol, const String& mcastInterface, int mcastTtl )
    {
        PRIVATE_CLASS_GET(p);

        p->_recvBuf.clear();
        p->_recvBuf._curPos = NULL;
        p->_recvBuf2.clear();
        p->_recvBuf2._curPos = NULL;
        
        YunStream::DeallocCallback deallocCB = _onGetStreamDeallocCallback();
        if (deallocCB)
        {
            deallocCB(&p->_recvBuf);
            deallocCB(&p->_recvBuf2);
            deallocCB(&p->_heartBeat);
        }
        _onMallocStream(false, p->_recvBuf);
        _onMallocStream(false, p->_recvBuf2);
        _onMallocStream(true, p->_heartBeat);

        // add default processor
        _onWrapHeartbeat(p->_heartBeat);

        String host = host1;
        if (host.empty())
        {
            host = SocketUtils::AddrToStr(SocketUtils::AddrLocal());
        }
        
        //
        // tcp
        //
        if (Protocol_Tcp == p->_protocol)
        {
            if (!p->_extSocket)
                disconnect();

            p->_extSocket = false;
            
            // Create addr info & Socket
            sockaddr_storage addr = SocketUtils::AddrCreateAuto( host, port );
            p->_pTransceiver = new TcpTransceiver;
            assert( p->_pTransceiver.get() );
            
            TcpTransceiver* pTransceiver = (TcpTransceiver*)(p->_pTransceiver.get());
            assert(pTransceiver);
            
            // Set timeout
            setTimeout(p->_timeout);
            p->_eventConnecting();
            
            // Connect
            ConnectResult cr = (0 < p->_timeout) ? pTransceiver->connectTimeout( addr, p->_timeout ) : pTransceiver->connect( addr );

            p->_pTransceiver->initialize();

            if (ConnectResult_Ok == cr)
            {
                p->onConnected();
            }

            if (isAutoHeartBeat())
            {
                autoHeartBeat(true);
            }
            if (isAutoRecvMessage())
            {
                autoRecvMessage(true);
            }

            return ConnectResult_Ok == cr;
        }
        //
        // udp
        //
        else
        {
            p->_pTransceiver = new UdpTransceiver();
            assert( p->_pTransceiver.get() );

            UdpTransceiver* pUdp = (UdpTransceiver*)(p->_pTransceiver.get());
            
            p->_eventConnecting();
            
            pUdp->doClient( host, port, mcastInterface, mcastTtl );
            
            if (SocketOperation_None == p->_pTransceiver->initialize())
            {
                setTimeout(p->_timeout);
                p->onConnected();

                if (isAutoRecvMessage())
                {
                    autoRecvMessage(true);
                }
                return true;
            }
            return false;
        }
        return false;
    }

    //-----------------------------------------------
    void YunClient::disconnect()
    {
        PRIVATE_CLASS_GET(p);
        p->_recvBuf.clear();
        p->_recvBuf._curPos = NULL;
        p->_recvBuf2.clear();
        p->_recvBuf2._curPos = NULL;
        p->disconnect(false);
    }
    
    //-----------------------------------------------
    bool YunClient::sendMessage( YunStream& msg )
    {
        PRIVATE_CLASS_GET(p);

        //assert(msg.isLegalYun());
        msg._curPos = NULL;
        
        SyncTokenPtr token = p->_sendSync.getToken();

        if (p->isDisconnect())
            return false;
        
        if (p->_pTransceiver.get() && !p->_pTransceiver->isClosed())
        {
            if ( p->_pTransceiver->write( &msg ) )
            {
                _addSendSize(msg.size());
                p->_preSend = DateTime::GetSysTime();
                return true;
            }
        }
        
        p->disconnect();

        return false;
    }

    //-----------------------------------------------
    bool YunClient::_swapMsg()
    {
        PRIVATE_CLASS_GET(p);
        YunStream& recvBuf = p->_recvBuf;
        YunStream& recvBuf2 = p->_recvBuf2;

        if (recvBuf2.size() > 0)
        {
            assert(!recvBuf._curPos);
            recvBuf.resize(YUN_CONTENT_MAX_LENGTH);
            recvBuf._curPos = recvBuf.begin();
            memcpy(recvBuf._curPos, recvBuf.begin(), recvBuf2.size());
            recvBuf._curPos += recvBuf2.size();
            recvBuf2.clear();
            return true;
        }

        return false;
    }

    //-----------------------------------------------
    bool YunClient::_dealMsg()
    {
        PRIVATE_CLASS_GET(p);
        YunStream& recvBuf = p->_recvBuf;
        YunStream& recvBuf2 = p->_recvBuf2;

        assert(recvBuf2.empty());

        // 瑙ｆ瀽娴?.
        s32 needBufLen;
        bool recvOk = recvBuf.analyseSelf( needBufLen );
        if (recvOk)
        {
            if (needBufLen > 0)
            {
                recvBuf2.add(recvBuf._curPos-needBufLen, needBufLen );
            }

            recvBuf.resize(recvBuf._curPos-recvBuf.begin()-needBufLen);
            recvBuf._curPos = NULL;
            return true;
        }
        else
        {
            if (Protocol_Udp == p->_protocol)
            {
                trace(_T(" getMessage(); illegality message!"));
                recvBuf.resize( YUN_CONTENT_MAX_LENGTH );
                recvBuf._curPos = recvBuf.begin();
                return false;   // 闇€缁х画鎺ユ敹.
            }

            if (0 == needBufLen || 0 < needBufLen)
            {
                trace(_T(" getMessage(); illegality message!"));
                recvBuf.resize( YUN_CONTENT_MAX_LENGTH );
                recvBuf._curPos = recvBuf.begin();
                return false;   // 闇€缁х画鎺ユ敹.
            }
            else
            {
                return false;   // 闇€缁х画鎺ユ敹.
            }
        }
    }

    //-----------------------------------------------
    YunStream& YunClient::getMessage()
    {
        static YunStream g_emptyYs;
        PRIVATE_CLASS_GET(p);
        
        YunStream& recvBuf = p->_recvBuf;
        recvBuf.resize( YUN_CONTENT_MAX_LENGTH );
        recvBuf._curPos = NULL;
        
        if (isDisconnect()
            || (p->_pMsgThread.get() && Thread::CurThreadId() != p->_pMsgThread->getThreadId())
            )
        {
            return g_emptyYs;
        }

        if (_swapMsg())
        {
            if (_dealMsg())
            {
                return recvBuf;
            }
        }
        else
        {
            recvBuf.resize(YUN_CONTENT_MAX_LENGTH);
            recvBuf._curPos = NULL;
        }

        assert(p->_recvBuf2.empty());
        
        if (NULL == recvBuf._curPos)    recvBuf._curPos = recvBuf.begin();

        s32 recvLen = 0;

        //Timer timer;
        while (true) {
            //timer.start();
            recvLen = (s32)p->_pTransceiver->readPart( (char*)recvBuf._curPos, recvBuf.end()-recvBuf._curPos );
            if (recvLen < 0)
            {
                //uinteger kk = timer.getMilliseconds();
                s32 err = NetStatus::ErrorCode();

                /////////
                // no err.
                if(NetStatus::IsInterrupted(err))
                {
                    continue;
                }

                /////////
                // timeout.
                if(NetStatus::IsWouldBlock(err))
                {
                    if (SocketUtils::GetRevTimeout(p->_pTransceiver->socket()) != -1)
                    {
                        return g_emptyYs;
                    }

                    continue;
                }
                if(NetStatus::IsTimeout(err))
                {
                    return g_emptyYs;
                }

                recvBuf._curPos = NULL;
                recvBuf.clear();
                p->disconnect();
                return g_emptyYs;
            }
            else if(0 == recvLen)
            {
                trace(_T(" getMessage(); disconnect"));
                recvBuf._curPos = NULL;
                recvBuf.clear();
                p->disconnect();
                return g_emptyYs;
            }
            else
            {
                //assert(recvLen == needBufLen);
                recvBuf._curPos += recvLen;
                _addRecvSize(recvLen);

                if (_dealMsg())
                {
                    return recvBuf;
                }
                else
                {
                    continue;
                }

            } // if..else
        } // while

        recvBuf.clear();
        return recvBuf;
    }

    //-----------------------------------------------
    YunClient::Event_Connect& YunClient::event_connecting()
    {
        PRIVATE_CLASS_GET(p);
        return p->_eventConnecting;
    }

    //-----------------------------------------------
    YunClient::Event_Disconnectd& YunClient::event_disconnected()
    {
        PRIVATE_CLASS_GET(p);
        return p->_eventDisconnect;
    }

    //-----------------------------------------------
    void YunClient::_exitInThread()
    {
        PRIVATE_CLASS_GET(p);
        p->_pMsgThread.release();
//         if (p->_pTransceiver)
//             p->_pTransceiver->close();
//         p->_pTransceiver = NULL;
        //p->_sendSync.forceUnlock();
    }

    //-----------------------------------------------
    hyc::Time YunClient::getPreSendTime() const
    {
        PRIVATE_CLASS_GET_CONST(p);
        return p->_preSend;
    } 

    //-----------------------------------------------
    void YunClient::setHeartBeatElapse( u32 elapse /*= 1000*15 */ )
    {
        PRIVATE_CLASS_GET(p);
        p->_heartBeatElapse = elapse;
    }

    //-----------------------------------------------
    const u32& YunClient::getHeartBeatElapse() const
    {
        PRIVATE_CLASS_GET_CONST(p);
        return p->_heartBeatElapse;
    }

    //-----------------------------------------------
    hyc::u32 YunClient::sequence()
    {
        PRIVATE_CLASS_GET(p);
        return ++(p->_sequence);
    }

    //-----------------------------------------------
    void YunClient::setPacketAnalyser( YunStreamAnalyserPtr pAnalyser )
    {
        PRIVATE_CLASS_GET(p);
        p->_pAnalyser = pAnalyser;

        p->_recvBuf.setAnalyser(pAnalyser.get());
        p->_recvBuf2.setAnalyser(pAnalyser.get());
        p->_heartBeat.setAnalyser(pAnalyser.get());
    }

    //-----------------------------------------------
    YunStreamAnalyserPtr YunClient::getPacketAnalyser() const
    {
        PRIVATE_CLASS_GET_CONST(p);
        return p->_pAnalyser;

    }

    //-----------------------------------------------
    bool YunClient::onMsg( const YunStream& buf )
    {
        if ( buf.getColNum() <= 0 || buf.getColType(0) != YunStream::Type_u16 )
        {
            return false;
        }
        else
        {
            PRIVATE_CLASS_GET(p);
            if (p->_pMsgCenter)
            {
                u16 key = 0;
                HY_VERIFY( buf.getColValue(0, key) );


                YunClientHandlerPtr& processor = p->_pMsgCenter->getProcessor(key);
                if (processor)
                {
                    processor->onMessageRecv(buf, NULL);
                    return true;
                }
            } // if.
        }
        
        return false;
    }

    //-----------------------------------------------
    bool YunClient::addHandler( YunClientHandlerPtr msgProcessor )
    {
        PRIVATE_CLASS_GET(p);
        if (!p->_pMsgCenter)
            return false;

        return p->_pMsgCenter->addProcessor(msgProcessor);
    }

    //-----------------------------------------------
    YunClientHandlerPtr& YunClient::getHandler( u16 mark )
    {
        static YunClientHandlerPtr obj;
        PRIVATE_CLASS_GET(p);
        if (!p->_pMsgCenter)
            return obj;
        return p->_pMsgCenter->getProcessor(mark);
    }

//     //-----------------------------------------------
//     ClientMessageCenter& YunClient::getMessageCenter()
//     {
//         PRIVATE_CLASS_GET(p);
//         return *p->_pMsgCenter;
//     }

    //-----------------------------------------------
    void YunClient::_onWrapHeartbeat( YunStream& heartbeat )
    {
        heartbeat.clear();
        heartbeat.addColBegin(false, true);
        heartbeat.addColEnd();
        assert(heartbeat.isLegalYun());
    }

    //-----------------------------------------------
    void YunClient::autoHeartBeat( bool enable )
    {
        PRIVATE_CLASS_GET(p);
        p->_autoHeartBeat = enable;
        if (Protocol_Tcp == p->_protocol)
        {
            if (!isDisconnect() || !enable)
            {
                if (enable)
                {
                    if (!p->_pHeartBeatThread || p->_pHeartBeatThread->getStatus() == ThreadStatus_Dead)
                    {
                        p->_pHeartBeatThread = new ClientHeartBeatThread(*this, p->_heartBeatElapse);
                        p->_pHeartBeatThread->start();
                    }
                    return;
                }

                if (p->_pHeartBeatThread)
                {
                    s32 t = getTimeout();
                    if (t < 0)
                    {
                        setTimeout(500);
                        trace(_T("the timeout is -1 maybe block. set to 500"));
                    }

                    p->_pHeartBeatThread->exit();
                    p->_pHeartBeatThread->abort();
                    p->_pHeartBeatThread.release();
                }
            } //if.
        }
    }

    //-----------------------------------------------
    void YunClient::autoRecvMessage( bool enable )
    {
        PRIVATE_CLASS_GET(p);
        p->_autoRecv = enable;
        if (!isDisconnect() || !enable)
        {
            if (enable)
            {
                if (!p->_pMsgThread || p->_pMsgThread->getStatus() == ThreadStatus_Dead)
                {
                    p->_pMsgThread = new ClientMessageThread(*this);
                    p->_pMsgThread->start();
                }
                return;
            }

            if (p->_pMsgThread.get())
            {
                s32 t = getTimeout();
                if (t < 0)
                {
                    setTimeout(500);
                    trace(_T("the timeout is -1 maybe block. set to 500"));
                }

                if (Thread::CurThreadId() != p->_pMsgThread->getThreadId())
                {
                    p->_pMsgThread->exit();
                    p->_pMsgThread->abort();
                    p->_pMsgThread.release();
                }
                else
                {
                    //((ClientMessageThread*)p->_pMsgThread.get())->natureExit();
                    p->_pMsgThread->exit();
                    p->_pMsgThread->join();
                    p->_pMsgThread.release();
                }
            }
        } // if.
    }

    //-----------------------------------------------
    bool YunClient::isAutoHeartBeat() const
    {
        PRIVATE_CLASS_GET_CONST(p);
        return p->_autoHeartBeat; //p->_pHeartBeatThread && p->_pHeartBeatThread->getStatus() != ThreadStatus_Dead;
    }

    //-----------------------------------------------
    bool YunClient::isAutoRecvMessage() const
    {
        PRIVATE_CLASS_GET_CONST(p);
        return p->_autoRecv; //p->_pMsgThread && p->_pMsgThread->getStatus() != ThreadStatus_Dead;
    }

//    //-----------------------------------------------
//    void YunClient::messageCenterPause()
//    {
//        PRIVATE_CLASS_GET(p);
//        if (p->_pMsgThread.get() && Thread::CurThreadId() != p->_pMsgThread->getThreadId())
//            p->_pMsgThread->suspend();
//    }
//    
//    //-----------------------------------------------
//    void YunClient::messageCenterStart()
//    {
//        PRIVATE_CLASS_GET(p);
//        if (p->_pMsgThread.get() && Thread::CurThreadId() != p->_pMsgThread->getThreadId())
//            p->_pMsgThread->start();
//    }
//    
//    //-----------------------------------------------
//    void YunClient::messageCenterDoMsg(YunStream& msg)
//    {
//        PRIVATE_CLASS_GET(p);
//        if (p->_pMsgCenter.get())
//            p->_pMsgCenter->onMessageRecv( p->_client, msg );
//    }
    
    
} // end namespace hyyun

