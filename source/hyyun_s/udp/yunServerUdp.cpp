//
// Copyright(c) 1999-2013 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#include "../preheader.h"
#include "../../../include/hyyun_s/yunServerUdp.h"
#include "../pool/pools.hpp"

#include "udpMessageConsumer.h"
#include "udpWriteConsumer.h"

namespace hyyun
{
    static YunStream g_emptyYun;

    //-----------------------------------------------
    YunServer_udp::YunServer_udp( const YunServerParam_udp& param )
        : YunServer(param.timeout, param.threadNum, param.queLenRead, param.queLenWrite)
        , _trans(_selector)
    {
    }

    //-----------------------------------------------
    YunServer_udp::~YunServer_udp()
    {
        stop();
    }

    //-----------------------------------------------
    void YunServer_udp::setMcastInterface( const String& mcastInterface )
    {
        _mcastInterface = mcastInterface;
    }

    //-----------------------------------------------
    bool YunServer_udp::go( const String& host1, int port, ProtocolSupport protocol )
    {
        if (isRunning())
        {
            trace_all( _T("udp服务器已经在运行") );
            return false;
        }

        String host = host1;
        if (host.empty())
        {
            host = SocketUtils::AddrToStr(SocketUtils::AddrLocal());
        }

        _isRunning = true;

        //
        // create consumer.
        //
        Array<hymq::MessageConsumer*> arrConsumer;
        Array<hymq::MessageConsumer*> arrConsumerWrite;
        for (u32 i = 0; i < _threadNum; i++)
        {
            arrConsumer.push_back(new UdpMessageConsumer);
            arrConsumerWrite.push_back(new UdpMessageConsumer);
        }
        UdpWriteConsumer* pWriteConsumer = new UdpWriteConsumer;
        _go(host, port, protocol, arrConsumer, arrConsumerWrite);

        //
        // udp
        //
        if (!_trans.doServer( host, port, protocol, _mcastInterface ))
        {
            _trans.close();
            _isRunning = false;
            return false;
        }

        SocketOperation so = _trans.initialize(); // udp server no need connect
        if (SocketOperation_None != so)
        {
            _trans.close();
            _isRunning = false;
            return false;
        }

        //
        // 创建agent.
        //
        _pAgent = new UdpAgent(*this, _trans, (_eventMsg.isEmpty() ? NULL : &_eventMsg));

        for (u32 i = 0; i < arrConsumer.size(); i++)
        {
            ((UdpMessageConsumer*)arrConsumer[i])->setAgent(_pAgent.get());
        }
        pWriteConsumer->setAgent(_pAgent.get());

        // 主工作项;
        _initWorkItems(_selector);

        _pAgent->start();

        return true;
    }

    //-----------------------------------------------
    void YunServer_udp::stop()
    {
        if (!isRunning())   return;

        _isRunning = false;

        // 等待所有线程完成.
        _exitWorkItems();

        // selector
        _selector.destroy();

        // 停止代理handler.
        _pAgent.release();
        _pAgent = NULL;
        _trans.close();
    }

    //-----------------------------------------------
    void YunServer_udp::_callEventError()
    {
        if (!_trans.isClosed())
        {
            _eventError(); 
            stop();
        }
    }
    //-----------------------------------------------
    void YunServer_udp::_callEventMsgIllegally(Endpoint& ep)
    {
        _eventMsgIllegally(ep);
    }

    //-----------------------------------------------
    void YunServer_udp::_onMsg( Endpoint& agent, YunStream& buf, OUT bool& illegally )
    {
        if (0 == buf.getColNum()) return;

        if ( buf.getColType(0) != YunStream::Type_u16 )
        {
            illegally = true;
        }
        else
        {
            u16 cacheKey;
            HY_VERIFY( buf.getColValue(0, cacheKey) );

            YunServerHandlerPtr& obj = getHandler(cacheKey);
            if (obj)
            {
                obj->onMessageRecv(agent, buf, NULL);
                return;
            }

            illegally = true;
        } // if..else
    }

} // end namespace hyyun
