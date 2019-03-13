
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy yun network framework
// author:       lipengxiang
// create at:    2014-04-11 10:33:15
// description:  
//

#ifndef __BP_UDPMESSAGECONSUMER_H_20140411103315__
#define __BP_UDPMESSAGECONSUMER_H_20140411103315__


namespace hyyun
{
    class UdpAgent;

    struct UdpMessageData
    {
        Endpoint    ep;
        ByteStream* pBuf;
    };

    class UdpMessageConsumer : public hymq::MessageConsumer
    {
    public:
        UdpMessageConsumer();
        ~UdpMessageConsumer();

        void    setAgent( UdpAgent* pUdpAgent );

        virtual void onMessage( hymq::MessageData pMsgData );
        virtual void onPop( hymq::MessageData pMsgData );

    private:
        UdpAgent* _pUdpAgent;
    };

} // end namespace hyyun

#endif // end of define __BP_UDPMESSAGECONSUMER_H_20140411103315__
