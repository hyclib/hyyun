
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy hyyun
// author:       lipengxiang
// create at:    2014-04-01 17:02:36
// description:  
//

#ifndef __BP_TCPMESSAGECONSUMER_H_20140401170236__
#define __BP_TCPMESSAGECONSUMER_H_20140401170236__


namespace hyyun
{
    class TcpAgent;

    struct TcpMessageData {
        TcpAgent*   pAgent;
        AsyncInfo*  pInfo;
    };

    class TcpMessageConsumer : public hymq::MessageConsumer
    {
    public:
        virtual void onMessage( hymq::MessageData pMsgData );
        virtual void onPop( hymq::MessageData pMsgData );
    };

} // end namespace hyyun

#endif // end of define __BP_TCPMESSAGECONSUMER_H_20140401170236__
