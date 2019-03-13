//
// Copyright(c) 1999-2013 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#ifndef __BP_HYENDPOINT_H_20130402161818__
#define __BP_HYENDPOINT_H_20130402161818__


namespace hyyun
{
    //----------------------------------------------------------------------------------------------------
    //*	endpoint hold the address information.
    //----------------------------------------------------------------------------------------------------
	class HYYUN_API Endpoint
    {
    public:
        Endpoint()  {}
        Endpoint(const struct sockaddr_storage& addr) : endpointAddr(addr)  {}
        Endpoint(const Endpoint& ep)    { operator=(ep); }
        virtual ~Endpoint() {}

        // override operator
        virtual operator        bool() const  { return !empty(); }
        virtual const Endpoint& operator=(const Endpoint& ep) const    { const_cast<sockaddr_storage&>(endpointAddr) = ep.endpointAddr; return *this; }
        virtual bool            operator==(const Endpoint& ep) const   { return 0 == SocketUtils::AddrCompare(endpointAddr, ep.endpointAddr); }
        virtual bool            operator!=(const Endpoint& ep) const   { return !(operator==(ep)); }
        virtual bool            operator< (const Endpoint& ep) const   { return 0 > SocketUtils::AddrCompare(endpointAddr, ep.endpointAddr); }

        // reset the address to empty.
        inline Endpoint& reset()               { memset((void*)&endpointAddr, 0, sizeof(struct sockaddr_storage)); return *this; }

        // the addr is empty?
        inline bool    empty() const           { return 0 == memcmp((void*)&endpointAddr, (void*)&(EmptyEp.endpointAddr), sizeof(struct sockaddr_storage)); }

        // get the endpoint ip.
        inline String  getEndpointIp() const   { return SocketUtils::AddrGetIp(endpointAddr); }

        // get the endpoint port.
        inline PORT    getEndpointPort() const { return SocketUtils::AddrGetPort(endpointAddr); }

    public:
        /*const */struct sockaddr_storage endpointAddr;

        /**
         * @brief empty addr use to compare.
         */
        static const Endpoint EmptyEp;
    };

} // end namespace hyyun

#endif // end of define __BP_HYENDPOINT_H_20130402161818__
