//
// Copyright(c) 1999-2011 hyrj. All rights reserved.
// base network libaray
// author:       lipengxiang
// description:  s
//

// boost pool
#include <boost/pool/poolfwd.hpp>
#include <boost/pool/singleton_pool.hpp>

#include "../preheader.h"

#ifdef new
#undef new
#endif

#include "../../../include/hyyun_s/yunServer.h"
#include "../../../include/hyyun_s/yunConnection.h"
#include "connectionPool.hpp"


namespace hyyun
{
    //----------------------------------------------------------------------------------------------------
    //!	Connection
    //----------------------------------------------------------------------------------------------------
    typedef boost::singleton_pool<Connection, sizeof(Connection)>   Connection_singleton_pool;

    //-----------------------------------------------
    Connection* ConnectionPool::malloc(YunServer_tcp& ser, YunTransceiverPtr& pTrans, const struct sockaddr_storage& addr)
    {
#ifdef _DEBUG
        static s32 g_i = 0;
        g_i++;
#endif // _DEBUG
        void* p = Connection_singleton_pool::malloc();
        if (!p)
        {
            release_memory();
            trace(_T("ConnectionPool 整理了一次内存"));
            p = Connection_singleton_pool::malloc();
        }
        if ( p )
        {
            //Connection* pp = (Connection*)p;
            //pp->Connection::Connection(ser, pTrans, addr);
        	Connection* pp = new (p)Connection(ser, pTrans, addr);
            return pp;
        }

        trace_all(_T("ConnectionPool::malloc 内存不足."));
        exit(0);

        return NULL;
    }

    //-----------------------------------------------
    void ConnectionPool::free( Connection* p )
    {
        if (p)
        {
#ifdef _DEBUG
            static s32 g_i = 0;
            g_i++;
#endif // _DEBUG

            p->Connection::~Connection();
            Connection_singleton_pool::free(p);
        }
    }

    //-----------------------------------------------
    void ConnectionPool::purge_memory()
    {
        Connection_singleton_pool::purge_memory();
    }

    //-----------------------------------------------
    void ConnectionPool::release_memory()
    {
        Connection_singleton_pool::release_memory();
    }
} // end namespace hyyun

