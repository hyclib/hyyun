//
// Copyright(c) 1999-2011 hyrj. All rights reserved.
// base network libaray
// author:       lipengxiang
// description:  
//

#ifndef __BP_HYSERVER_P_HPP_2011080111851__
#define __BP_HYSERVER_P_HPP_2011080111851__

#include "tcpHeartBeat.h"
#include "tcpConnectEvent.h"

namespace hyyun
{
    //----------------------------------------------------------------------------------------------------
    //! YunServer::PrivateClass
    //----------------------------------------------------------------------------------------------------
    class YunServer_tcp::PrivateClass
    {
    public:
        inline static TcpHeartBeat&       GetHeartBeat( YunServer_tcp& server )     { PRIVATE_CLASS_GET_OTHER(&server, p); return p->_heartBeat; }
    public:
        PrivateClass( YunServerParam_tcp param, Selector& selector );
        ~PrivateClass();


        //! 默认的消息处理函数, 避免空处理方法异常.
        bool    _defaultEventConnected( Connection& newConnection )         { return true; }
        bool    _defaultEventMessageIllegally( Connection& newConnection )  { return true; }
        void    _defaultEventDisconnect( Connection& newConnection )        { }

    public:
        // 运行参数.
        YunServerParam_tcp      _param;
        u32                     _maxConnectionNum;
        bool                    _allowAccept;

        // 通信器.
        YunTcpAcceptor          _acceptor;
        TcpConnectEventPtr      _connectEventPtr;

        // 心跳.
        TcpHeartBeat            _heartBeat;
        ThreadPtr               _pHeartBeatThread;

        hymq::MessageQueuePtr   _pConnectMessageQueue;

        // reflex
        Event_Connected             _eventConnected;
        Event_Disconnect            _eventDisconnected;
        Event_MessageIllegally      _eventMessageIllegally;
    };

} // end namespace hyyun

#endif // end of define __BP_HYSERVER_P_HPP_2011080111851__