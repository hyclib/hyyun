
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy yun network framework
// author:       lipengxiang
// create at:    2014-04-11 16:58:56
// description:  
//

#include "../preheader.h"
#include "tcpConnectConsumer.h"

#include "../pool/pools.hpp"

#include "../../../include/hyyun_s/yunServerTcp.h"

namespace hyyun
{


    //-----------------------------------------------
    TcpConnectConsumer::TcpConnectConsumer( YunServer_tcp& server )
        :_server(server)
    {

    }

    void TcpConnectConsumer::onPop(hymq::MessageData pMsgData)
    {
        TcpConnectMessageDataPool::free((TcpConnectMessageData*)pMsgData);

    }

    //-----------------------------------------------
    void TcpConnectConsumer::onMessage( hymq::MessageData pMsgData )
    {
       TcpConnectMessageData* pData = (TcpConnectMessageData*)pMsgData;
        _server._onDealNewConnection(pData->pConn);
    }

} // end namespace hyyun

