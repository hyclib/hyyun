//
// Copyright(c) 1999-2011 hyrj. All rights reserved.
// base network libaray
// author:       lipengxiang
// description:  
//

#ifndef __BP_CONNECTSELECTORHANDLER_H_20110707162458__
#define __BP_CONNECTSELECTORHANDLER_H_20110707162458__

#include "tcpHeartBeat.h"

namespace hyyun
{
    //----------------------------------------------------------------------------------------------------
    //!	connectHandler
    //----------------------------------------------------------------------------------------------------
    class TcpConnectEvent : public SelectorEvent
    {
        YunTcpAcceptor&     _acceptor;
        YunSelector&        _selector;
        YunServer_tcp&      _server;
        bool&               _allowAccept;
        TcpHeartBeat&       _heartBeat;
        //SyncObject          _sync;

#ifdef CN_NET_IOCP
        AsyncInfo           _recvInfo;
#endif

    public:
        TcpConnectEvent( 
            YunTcpAcceptor& acceptor, 
            YunServer_tcp&      server, 
            bool&           allowAccept
            );
        ~TcpConnectEvent();

        void                startConnect()  { startAsync(&_recvInfo, NULL); }

        //! Get the native information of the handler, this is used by the selector.
        virtual SOCKET      socket()        { return _acceptor.socket(); }
        virtual bool        startAsync( AsyncInfo* pInfo, AsyncData* pData );
        virtual void        finishAsync( AsyncInfo* pInfo );
        virtual void        finished();
    };

    typedef ObjectPtr<TcpConnectEvent>    TcpConnectEventPtr;


} // end namespace hyyun

#endif // end of define __BP_CONNECTSELECTORHANDLER_H_20110707162458__

