//
// Copyright(c) 1999-2011 hyrj. All rights reserved.
// base network libaray
// author:       lipengxiang
// description:  
//

#include "preheader.h"


#include "../../include/hyyun_s/yunConnection.h"

#include "thread/serverMemoryPoolThread.h"
#include "thread/serverWorkThread.h"

#include "pool/pools.hpp"

#include "yunMessageQueue.h"

namespace hyyun
{

    //-----------------------------------------------
    String YunServer::TraceDebugInfo()
    {
        String buf;
        Platform::MemoryStatus ms;
        Platform::GetMemoryStatus(ms);

        buf.format(_T("Total Memory: %llu KB Available Memory: %llu KB\r\n"), ms.totalPhy/1024, ms.availPhy/1024);

        return _T("===================================================\n")
            + buf 
            + YunStreamPool::traceInfo() 
            + _T("Connection     ") + Connection::TraceInfo() 
            + _T("AsyncInfo      ") + AsyncInfoPool::TraceInfo() 
            + _T("TcpMessageData ") + TcpMessageDataPool::TraceInfo() 
            + _T("TcpAsyncData   ") + TcpAsyncDataPool::TraceInfo()
            + _T("UdpMessageData ") + UdpMessageDataPool::TraceInfo() 
            + _T("UdpAsyncData   ") + UdpAsyncDataPool::TraceInfo()
            + _T("===================================================\n");
    }

    //-----------------------------------------------
    void YunServer::SetMemoryPoolClearupElapse( u32 elapse )
    {
        ServerMemoryPoolThread::getSingleton().setElapse(elapse);
    }

    //-----------------------------------------------
    hyc::u32 YunServer::GetMemoryPoolClearupElapse()
    {
        return ServerMemoryPoolThread::getSingleton().getElapse();
    }

    //-----------------------------------------------
    YunServer::YunServer( s32 timeout /*= -1*/, u32 threadNum /*= 0 */, u32 queLen/* = 8*/, u32 queLenWrite )
        : _isRunning(false)
        , _threadNum(threadNum)
        , _timeout(timeout)
        , _pMessageQueue(NULL)
        , _pMessageQueue_write(NULL)
        , _queLenRead(queLen)
        , _queLenWrite(queLenWrite)
    {
        if (0 == _queLenRead)
            _queLenRead = 1;

        // 默认的线程数.
        if (0 == _threadNum)
        {
            _threadNum = Platform::GetProcessorNum();
        }
    }
    
    //-----------------------------------------------
    YunServer::~YunServer()
    {
        SAFE_DELETE(_pMessageQueue);
        SAFE_DELETE(_pMessageQueue_write);
    }

    //-----------------------------------------------
    bool YunServer::addHandler( YunServerHandlerPtr msgProcessor )
    {
        if (isRunning())            return false;
        if (!msgProcessor.get())    return false;
        MsgProMap::iterator itr = _mapHandler.find( msgProcessor->getMessageMark() );
        if ( itr != _mapHandler.end())
        {
            return false;
        }
        else
        {
            _mapHandler.insert( std::make_pair(msgProcessor->getMessageMark(), msgProcessor) );
            return true;
        }
    }

    //-----------------------------------------------
    hyyun::YunServerHandlerPtr& YunServer::getHandler( u16 mark )
    {
        static YunServerHandlerPtr obj;
        MsgProMap::iterator itr = _mapHandler.find(mark);
        if (itr != _mapHandler.end())
        {
            return itr->second;
        }

        return obj;
    }

    //-----------------------------------------------
    void YunServer::_initWorkItems( YunSelector& selector )
    {
        /*//-----------------------------------------------
        // 主工作项; 创建cpu个数的工作线程.
        assert(_threadPoolItems.size() == 0);

        _threadPoolItems.clear();
        for (u32 i = 0; i < _threadNum; ++i)
        {
            ServerMainWorkItem* pMainWorkItem 
                = new ServerMainWorkItem( selector, _timeout );

            assert(pMainWorkItem);
            ThreadPool::getSingleton().QueueWorkItem( pMainWorkItem, ThreadPoolMode_LongWork );
            _threadPoolItems.push_back(pMainWorkItem);
        }*/

        _pMessageQueue->start();
        _pMessageQueue_write->start();

        // 回调所有线程id.
        Array<integer> arrIds;
        _pMessageQueue->getThreadIds(arrIds);
        event_init()( arrIds );

        _pWorkThread = new ServerWorkThread(selector, _timeout);
        _pWorkThread->start();
    }

    //-----------------------------------------------
    void YunServer::_exitWorkItems()
    {
        /*assert(_threadPoolItems.size() == _threadNum);

        for (u32 i = 0; i < _threadNum; ++i)
        {
            _threadPoolItems[i]->exit();
        }
        _threadPoolItems.clear();

        print_debug_line(_T("server exit"));*/
		if (_pWorkThread)
		{
			_pWorkThread->exit();
            Thread::Sleep(2000);
            _pWorkThread->join();
            //_pWorkThread->abort();
            _pWorkThread = NULL;
		}

        if (_pMessageQueue)
            _pMessageQueue->stop();
        if (_pMessageQueue_write)
            _pMessageQueue_write->stop();
    }

    //-----------------------------------------------
    void YunServer::_go( const String& host, int port, ProtocolSupport protocol,
        const Array<hymq::MessageConsumer*> arrMsgConsumer, const Array<hymq::MessageConsumer*> arrMsgWriteConsumer)
    {
        const_cast<sockaddr_storage&>(_ep.endpointAddr) = SocketUtils::AddrCreate(host, port, protocol, true);

        // create logic thread.
        assert(!_pMessageQueue && _threadNum == arrMsgConsumer.size());
        _pMessageQueue = new MessageQueue(_queLenRead, arrMsgConsumer);
        _pMessageQueue_write = new MessageQueue(_queLenWrite, arrMsgWriteConsumer);
    }

    //-----------------------------------------------
    void YunServer::_pushMessageRead( void* data, uinteger mask )
    {
        _pMessageQueue->push(data, mask);
    }

    //-----------------------------------------------
    void YunServer::_pushMessageWrite(void* data, uinteger mask)
    {
        _pMessageQueue_write->push(data, mask);
    }

    //-----------------------------------------------
    void YunServer::setPacketAnalyser( YunStreamAnalyserPtr pAnalyser )
    {
        _pAnalyser = pAnalyser;
    }

    //-----------------------------------------------
    YunStreamAnalyserPtr YunServer::getPacketAnalyser() const
    {
        return _pAnalyser;
    }


} // end namespace hyyun

