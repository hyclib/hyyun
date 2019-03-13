
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy yun network framework
// author:       lipengxiang
// create at:    2014-04-03 18:02:28
// description:  
//

#ifndef __BP_POOLS_HPP_20140403180228__
#define __BP_POOLS_HPP_20140403180228__

#ifdef new
#undef new
#endif

#include "poolTemplate.hpp"

#include "yunStreamPool.h"
#include "connectionPool.hpp"

#include "../tcp/tcpMessageConsumer.h"
#include "../tcp/tcpConnectConsumer.h"

#include "../udp/udpAsyncInfo.h"
#include "../udp/udpMessageConsumer.h"

#if CN_PLATFORM == CN_PLATFORM_WINDOWS
#ifdef _DEBUG
#   define new CN_LEAK_DETECTOR_ON
#endif // _DEBUG
#endif


namespace hyyun
{
    typedef ObjectPool<AsyncInfo>       AsyncInfoPool;
    typedef ObjectPool<TcpAsyncData>    TcpAsyncDataPool;
    typedef ObjectPool<TcpMessageData>  TcpMessageDataPool;
    typedef ObjectPool<TcpConnectMessageData>  TcpConnectMessageDataPool;

    typedef ObjectPool<UdpAsyncData>    UdpAsyncDataPool;
    typedef ObjectPool<UdpMessageData>  UdpMessageDataPool;


} // end namespace hyyun

#endif // end of define __BP_POOLS_HPP_20140403180228__
