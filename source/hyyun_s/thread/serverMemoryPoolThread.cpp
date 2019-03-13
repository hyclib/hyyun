//
// Copyright(c) 1999-2013 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#include "../preheader.h"
#include "serverMemoryPoolThread.h"
#include "../pool/collections.hpp"

IMPLEMENT_INTERFACE_SINGLETON(hyyun::ServerMemoryPoolThread);

namespace hyyun
{
    //-----------------------------------------------
    ServerMemoryPoolThread::ServerMemoryPoolThread()
        : _elapseClearupPool(1000*10*60)
    {

    }

    //-----------------------------------------------
    ServerMemoryPoolThread::~ServerMemoryPoolThread()
    {

    }

    //-----------------------------------------------
    bool ServerMemoryPoolThread::onInvoke( void* lpParam )
    {
        Thread::Sleep(_elapseClearupPool);

        CollectionsMemoryPool();

        print_debug_line(_T("MemoryPool Clearup"));

        return false;
    }

} // end namespace hyyun
