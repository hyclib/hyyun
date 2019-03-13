//
// Copyright(c) 1999-2013 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#include "../preheader.h"
#include "../../../include/hyyun_s/yunConnection.h"
#include "../../../include/hyyun_s/yunServerTcp.h"
#include "../pool/yunStreamPool.h"
#include "../pool/connectionPool.hpp"

#include "yunServerTcp_p.hpp"

namespace hyyun
{
#ifdef YUN_TRACE_POOL
    static AtomicBase<u32> g_countTotal = 0;
    static AtomicBase<u32> g_count = 0;

    //-----------------------------------------------
    hyc::String Connection::TraceInfo()
    {
        String buf;
        buf.format(_T("Count: %d %d\n"), (u32)g_countTotal, (u32)g_count);
        return buf;
    }
#else
    //-----------------------------------------------
        hyc::String Connection::TraceInfo()
        {
        	return _T("");
        }
#endif // YUN_TRACE_POOL


    //-----------------------------------------------
    Connection::Connection( YunServer_tcp& server, YunTransceiverPtr& pTrans, const struct sockaddr_storage& addr )
        : TcpAgent(server, pTrans, addr)
        , _pDatabinding(NULL)
    {
#ifdef YUN_TRACE_POOL
        ++g_count;
        ++g_countTotal;
//         debug_printf("On Connection %d\r\n", g_count);
#endif // YUN_TRACE_POOL
    }

    //-----------------------------------------------
    Connection::~Connection()
    {
#ifdef YUN_TRACE_POOL
        --g_count;
//         debug_printf("~On Connection %d\r\n", g_count);
#endif // YUN_TRACE_POOL
    }

    //-----------------------------------------------
    void Connection::onDestroy()
    {
        ConnectionPool::free(this);
    }

} // end namespace hyyun
