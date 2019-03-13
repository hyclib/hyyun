//
// Copyright(c) 1999-2013 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#include "../preheader.h"

#include "../../../include/hyyun_s/yunServerTcp.h"
#include "yunServerTcp_p.hpp"
#include "tcpHeartBeatThread.h"

namespace hyyun
{
    //-----------------------------------------------
    TcpHeartBeatThread::TcpHeartBeatThread( TcpHeartBeat& heartBeat, u32& elapse )
        : _heartBeat(heartBeat)
        , _elapse(elapse)
    {
        if (_elapse < 1000) _elapse = 1000;
    }

    TcpHeartBeatThread::~TcpHeartBeatThread()
    {
    }

    bool TcpHeartBeatThread::onInvoke(void* lpParam)
    {
        Thread::Sleep(_elapse);

        print_debug(_T("Heart Beat remove start "));
        print_debug(_T("%d >"), _heartBeat.size());

        //
        // 心跳
        //
        _heartBeat.eraseDead();

        print_debug_line(_T(" %d"), _heartBeat.size());

        return false;
    }

} // end namespace hyyun
