//
// Copyright(c) 1999-2013 hyrj. All rights reserved.
// base network libaray
// author:       lipengxiang
// description:  
//

#include "clientHeartBeatThread.hpp"

namespace hyyun
{

    //-----------------------------------------------
    bool ClientHeartBeatThread::onStart( void* lpParam )
    {
        sleepTime = _elapse;
        return true;
    }

    //-----------------------------------------------
    void ClientHeartBeatThread::onFinish( void* lpParam )
    {

    }

    //-----------------------------------------------
    bool ClientHeartBeatThread::onInvoke( void* lpParam )
    {
        Time e = DateTime::GetSysTime()-_conns.getPreSendTime();
        e -= _elapse/1000;

        if (e >= (-2))
        {
            sleepTime = _elapse;
            if (!_conns.sendHeartBeat())
                return true;
        }
        else
        {
            sleepTime = (u32)(-e*1000);
        }

        Thread::Sleep(sleepTime);

        return false;
    }


} // end namespace hyyun
