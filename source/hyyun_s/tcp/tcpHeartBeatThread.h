//
// Copyright(c) 1999-2013 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#ifndef __BP_SERVERHEARTBEATTHREAD_H_20130325143334__
#define __BP_SERVERHEARTBEATTHREAD_H_20130325143334__


namespace hyyun
{
    //-----------------------------------------------
    //  用于心跳处理, 以及内存池的清理.
    class TcpHeartBeatThread : public Thread
    {
        TcpHeartBeat& _heartBeat;
        u32&            _elapse;

    public:
        TcpHeartBeatThread( TcpHeartBeat& heartBeat, u32& elapse );
        ~TcpHeartBeatThread();

        virtual bool     onInvoke(void* lpParam);
    };

} // end namespace hyyun

#endif // end of define __BP_SERVERHEARTBEATTHREAD_H_20130325143334__