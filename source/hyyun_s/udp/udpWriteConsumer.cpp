
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy yun network framework
// author:       lipengxiang
// create at:    2014-04-18 11:31:46
// description:  
//

#include "../preheader.h"
#include "udpWriteConsumer.h"

#include "../../../include/hyyun_s/yunUdpAgent.h"

namespace hyyun
{

    //-----------------------------------------------
    UdpWriteConsumer::UdpWriteConsumer()
        : _pUdpAgent(NULL)
    {
    }

    //-----------------------------------------------
    UdpWriteConsumer::~UdpWriteConsumer()
    {
        if (_pUdpAgent)
            _pUdpAgent->release();
    }

    //-----------------------------------------------
    void UdpWriteConsumer::setAgent( UdpAgent* pUdpAgent )
    {
        _pUdpAgent = pUdpAgent;
        _pUdpAgent->addRef();
    }

    //-----------------------------------------------
    void UdpWriteConsumer::onMessage( hymq::MessageData pMsgData )
    {
        assert(_pUdpAgent);
        AsyncInfo* pInfo = (AsyncInfo*)pMsgData;

        _pUdpAgent->doAsync_write(pInfo);


        //_pUdpAgent->SelectorEvent::release();
    }

    void UdpWriteConsumer::onPop(hymq::MessageData pMsgData)
    {
#ifdef CN_NET_IOCP
        _pUdpAgent->freeAsync((AsyncInfo*)pMsgData);
#endif // CN_NET_IOCP
    }


} // end namespace hyyun

