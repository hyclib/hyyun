//
// Copyright(c) 1999-2013 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#ifndef __BP_SERVERMEMORYPOOLTHREAD_H_20130325143753__
#define __BP_SERVERMEMORYPOOLTHREAD_H_20130325143753__


namespace hyyun
{
    //-----------------------------------------------
    //  用于内存池的清理.
    class ServerMemoryPoolThread : public Thread, public Singleton<ServerMemoryPoolThread>
    {
        u32             _elapseClearupPool;

    public:
        ServerMemoryPoolThread();
        ~ServerMemoryPoolThread();


        void    setElapse( u32 elapse ) { _elapseClearupPool = elapse; }
        u32     getElapse() const       { return _elapseClearupPool; }

        virtual bool     onInvoke(void* lpParam);
    };
} // end namespace hyyun

#endif // end of define __BP_SERVERMEMORYPOOLTHREAD_H_20130325143753__