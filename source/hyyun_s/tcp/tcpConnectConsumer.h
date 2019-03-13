
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy yun network framework
// author:       lipengxiang
// create at:    2014-04-11 16:58:31
// description:  
//

#ifndef __BP_TCPCONNECTCONSUMER_H_20140411165831__
#define __BP_TCPCONNECTCONSUMER_H_20140411165831__

#include "../../../include/hyyun_s/yunConnection.h"

namespace hyyun
{
    struct TcpConnectMessageData {
        ConnectionPtr   pConn;
    };

    class TcpConnectConsumer : public hymq::MessageConsumer
    {
    public:
        TcpConnectConsumer( YunServer_tcp& server );

        virtual void onPop( hymq::MessageData pMsgData );
        virtual void onMessage( hymq::MessageData pMsgData );

    private:
        YunServer_tcp&  _server;
    };

} // end namespace hyyun

#endif // end of define __BP_TCPCONNECTCONSUMER_H_20140411165831__
