//
// Copyright(c) 1999-2013 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#ifndef __BP_UDPASYNCINFO_H_20130409145753__
#define __BP_UDPASYNCINFO_H_20130409145753__

namespace hyyun
{
    struct UdpAsyncData : AsyncData
    {
        UdpAsyncData() 
            : ep(Endpoint::EmptyEp) {}

        Endpoint        ep;
    };	

} // end namespace hyyun

#endif // end of define __BP_UDPASYNCINFO_H_20130409145753__
