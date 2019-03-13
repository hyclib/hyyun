
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy yun network framework
// author:       lipengxiang
// create at:    2014-04-18 11:31:23
// description:  
//

#ifndef __BP_UDPWRITECONSUMER_H_20140418113123__
#define __BP_UDPWRITECONSUMER_H_20140418113123__


namespace hyyun
{
    class UdpAgent;

    class UdpWriteConsumer : public hymq::MessageConsumer
    {
    public:
        UdpWriteConsumer();
        ~UdpWriteConsumer();

        void    setAgent( UdpAgent* pUdpAgent );

        virtual void onMessage( hymq::MessageData pMsgData );
        virtual void onPop( hymq::MessageData pMsgData );
    private:
        UdpAgent* _pUdpAgent;
    };
} // end namespace hyyun

#endif // end of define __BP_UDPWRITECONSUMER_H_20140418113123__
