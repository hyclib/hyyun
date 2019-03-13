
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// base network libaray
// author:       lipengxiang
// create at:    2014-04-08 18:08:41
// description:  
//

#include "../preheader.h"


#include "pools.hpp"

namespace hyyun
{
    OBJECT_POOL_STATIC( AsyncInfo );
    OBJECT_POOL_STATIC( TcpAsyncData );
    OBJECT_POOL_STATIC( TcpMessageData );
    OBJECT_POOL_STATIC( TcpConnectMessageData );

    OBJECT_POOL_STATIC( UdpAsyncData );
    OBJECT_POOL_STATIC( UdpMessageData );

} // end namespace hyyun

