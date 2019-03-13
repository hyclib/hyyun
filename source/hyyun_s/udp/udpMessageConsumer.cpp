
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy yun network framework
// author:       lipengxiang
// create at:    2014-04-11 10:32:34
// description:  
//

#include "../preheader.h"
#include "udpMessageConsumer.h"

#include "../pool/pools.hpp"
#include "../../../include/hyyun_s/yunUdpAgent.h"

namespace hyyun
{

    //-----------------------------------------------
    UdpMessageConsumer::UdpMessageConsumer()
        : _pUdpAgent(NULL)
    {
    }

    //-----------------------------------------------
    UdpMessageConsumer::~UdpMessageConsumer()
    {
        if (_pUdpAgent)
            _pUdpAgent->release();
    }

    //-----------------------------------------------
    void UdpMessageConsumer::setAgent( UdpAgent* pUdpAgent )
    {
        _pUdpAgent = pUdpAgent;
        _pUdpAgent->addRef();
    }

    //-----------------------------------------------
    void UdpMessageConsumer::onMessage( hymq::MessageData pMsgData )
    {
        assert(_pUdpAgent);
        UdpMessageData* pData = (UdpMessageData*)pMsgData;

        if (!_pUdpAgent->isClosed())
        {
            _pUdpAgent->doAsync_read(pData);
        }

        //_pUdpAgent->SelectorEvent::release();
    }

    void UdpMessageConsumer::onPop(hymq::MessageData pMsgData)
    {
        _pUdpAgent->freeMsgData((UdpMessageData*)pMsgData);
    }

} // end namespace hyyun

