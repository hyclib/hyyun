//
// Copyright(c) 1999-2013 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#ifndef __BP_COLLECTIONS_HPP_20130409150728__
#define __BP_COLLECTIONS_HPP_20130409150728__

#include "connectionPool.hpp"
#include "yunStreamPool.h"

#include "pools.hpp"



namespace hyyun
{
	extern "C" void CollectionsMemoryPool()
    {
        YunStreamPool::release_memory();
        ConnectionPool::release_memory();

        AsyncInfoPool::release_memory();
        TcpAsyncDataPool::release_memory();
        TcpMessageDataPool::release_memory();
        TcpConnectMessageDataPool::release_memory();

        UdpAsyncDataPool::release_memory();
        UdpMessageDataPool::release_memory();
    }

} // end namespace hyyun

#endif // end of define __BP_COLLECTIONS_HPP_20130409150728__