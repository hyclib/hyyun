//
// Copyright(c) 1999-2013 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#include "../preheader.h"
#include "../pool/connectionPool.hpp"
#include "../../../include/hyyun_s/yunServerTcp.h"

#include "yunServerTcp_p.hpp"
#include "tcpConnectEvent.h"

namespace hyyun
{
    TcpConnectEvent::TcpConnectEvent(
        YunTcpAcceptor& acceptor,
        YunServer_tcp&  server, 
        bool&           allowAccept
        )
        : _acceptor(acceptor)
        , _selector(server.getSelector())
        , _server(server)
        , _allowAccept(allowAccept)
        , _heartBeat(YunServer_tcp::PrivateClass::GetHeartBeat(server))
    {
    	_selector.initialize(this);
    }

    TcpConnectEvent::~TcpConnectEvent()
    {
        finished();
    }

    //-----------------------------------------------
    bool TcpConnectEvent::startAsync( AsyncInfo* pInfo, AsyncData* pData )
    {
        //SyncTokenPtr token = _sync.getToken();

#if defined(CN_NET_IOCP)
    	addRef();   // 引用计数加1, 由finishAsync释放.
        _acceptor.startAccept(pInfo);
#elif defined(CN_NET_EPOLL)
    	_selector.enable(this, &_recvInfo);	// epoll only call once.
#endif

        return true;
    }

    //-----------------------------------------------
    void TcpConnectEvent::finishAsync( AsyncInfo* pInfo )
    {
        //SyncTokenPtr token = _sync.getToken();

        //assert(pInfo == &_info);

        YunTransceiverPtr pTrans = _acceptor.doAccept();
        while (pTrans)
        {
        	// 连接未满或允许接受.
        	if (_allowAccept && _heartBeat.size() < _server.getMaxConnectionNum())
        	{
        		struct sockaddr_storage cacheAddr;

        		// 保存地址信息.
        	    if( SocketUtils::AddrRemoteAddress(pTrans->socket(), const_cast<struct sockaddr_storage&>(cacheAddr)) )
        	    {
        	    	// 创建连接.
        	    	ConnectionPtr ptr = ConnectionPool::malloc(_server, pTrans, cacheAddr);
        	    	_server._onConnected(*ptr.get());
        	    }
        	} // if

        	pTrans = _acceptor.doAccept();
        }

#if defined(CN_NET_IOCP)
        _acceptor.finishAccept(pInfo);
        startConnect();    // 再投递.
        release();
#endif

        return;
    }

    //-----------------------------------------------
    void TcpConnectEvent::finished()
    {
        _selector.remove( this, &_recvInfo );
    }

} // end namespace hyyun
