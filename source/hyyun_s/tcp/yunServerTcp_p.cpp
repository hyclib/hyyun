//
// Copyright(c) 1999-2013 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#include "../preheader.h"
#include "../../../include/hyyun_s/yunServerTcp.h"
#include "yunServerTcp_p.hpp"
#include <algorithm>

namespace hyyun
{



    //-----------------------------------------------
    YunServer_tcp::PrivateClass::PrivateClass( YunServerParam_tcp param, Selector& selector )
        : _param(param)
        , _maxConnectionNum((u32)-1)
        , _allowAccept(true)
        , _acceptor(selector, false)
    {
        _param.recvBufLength = SER_RECV_BUF_INIT_LENGTH;
        _eventConnected.set(this, &YunServer_tcp::PrivateClass::_defaultEventConnected);
        _eventDisconnected.set(this, &YunServer_tcp::PrivateClass::_defaultEventDisconnect);
        _eventMessageIllegally.set(this, &YunServer_tcp::PrivateClass::_defaultEventMessageIllegally);
    }

    //-----------------------------------------------
    YunServer_tcp::PrivateClass::~PrivateClass()
    {
    }


} // end namespace hyyun
