//
// Copyright(c) 1999-2011 hyrj. All rights reserved.
// base network libaray
// author:       lipengxiang
// description:  用来快速分配回收pool
//

#ifndef __BP_CONNECTIONPOOL_H_20110714144558__
#define __BP_CONNECTIONPOOL_H_20110714144558__


namespace hyyun
{
    class YunServer_tcp;

    class Connection;
    class ConnectionPool
    {
    public:
        static Connection* malloc(YunServer_tcp& ser, YunTransceiverPtr& pTrans, const struct sockaddr_storage& addr);
        static void        free(Connection* p);

        // release unuse memory
        static void        release_memory();

        // release all memory
        static void        purge_memory();
    };

} // end namespace hyyun

#endif // end of define __BP_CONNECTIONPOOL_H_20110714144558__

