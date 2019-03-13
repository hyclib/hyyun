//
// Copyright(c) 1999-2011 hyrj. All rights reserved.
// base network libaray
// author:       lipengxiang
// description:  
//

#ifndef __BP_CLIENTHEARTBEATTHREAD_HPP_20110801104417__
#define __BP_CLIENTHEARTBEATTHREAD_HPP_20110801104417__

#include "../preheader.h"

namespace hyyun
{
    //-----------------------------------------------
    class ClientHeartBeatThread : public Thread
    {
        YunClient&   _conns;
        u32&         _elapse;
        u32          sleepTime;
    public:
        ClientHeartBeatThread( YunClient& conns, u32& elapse )
            : _conns(conns), _elapse(elapse)
        {
        }

        virtual bool    onStart(void* lpParam);
        virtual bool    onInvoke(void* lpParam);
        virtual void    onFinish(void* lpParam);
    };

} // end namespace hyyun

#endif // end of define __BP_CLIENTHEARTBEATTHREAD_HPP_20110801104417__

