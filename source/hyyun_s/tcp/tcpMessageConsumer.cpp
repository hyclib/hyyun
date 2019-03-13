
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy hyyun
// author:       lipengxiang
// create at:    2014-04-01 17:07:59
// description:  
//

#include "../preheader.h"
#include "../../../include/hyyun_s/yunTcpAgent.h"

#include "tcpMessageConsumer.h"

#include "../pool/pools.hpp"


namespace hyyun
{
    //-----------------------------------------------
    void TcpMessageConsumer::onMessage( hymq::MessageData pMsgData )
    {
        TcpMessageData* pData = (TcpMessageData*)pMsgData;
        pData->pAgent->doAsync(pData->pInfo);

    }

    void TcpMessageConsumer::onPop(hymq::MessageData pMsgData)
    {
        TcpMessageDataPool::free((TcpMessageData*)pMsgData);
    }

} // end namespace hyyun

