//
// Copyright(c) 1999-2011 hyrj. All rights reserved.
// base network libaray
// author:       lipengxiang
// description:  独立线程接收的方式 (适用于linux)
//

#ifndef __BP_HYWORKITEMACCEPT_HPP_20110628163343__
#define __BP_HYWORKITEMACCEPT_HPP_20110628163343__

namespace hyyun
{
//     //----------------------------------------------------------------------------------------------------
//     //!	Accept Callback
//     //----------------------------------------------------------------------------------------------------
//     class AcceptCBWorkItem : public ThreadPoolWorkItem
//     {
//         Server&         _server;
//         ConnectionPtr   _connection;
//     public:
//         AcceptCBWorkItem(ConnectionPtr connection, Server& server) : _server(server), _connection(connection){ }
// 
//         virtual u32     onInvoke()
//         {
//             try
//             {
//                 if (_connection.get())
//                 {
//                     _server.onConnected( _connection );
//                 }
//             }
//             catch (...)
//             {
//                 _TRACE_LOG("ERROR break in AcceptCBWorkItem!");
//             }
// 
//             delete this;
//             return 0;
//         }
//     };
// 
// 
//     //----------------------------------------------------------------------------------------------------
//     //! Accept
//     //----------------------------------------------------------------------------------------------------
//     class AcceptWorkItem : public ThreadPoolWorkItem
//     {
//         TcpAcceptor*        _pAccept;
//         IOCPSelector&       _selector;
//         Server&             _server;
// 
//     public:
//         AcceptWorkItem(TcpAcceptor* accept, IOCPSelector& selector, Server& server) : _server(server), _selector(selector), _pAccept(accept) { assert(_pAccept); }
// 
//         virtual u32     onInvoke()
//         {
//             try
//             {
//                 // 接收连接
//                 while ( true )
//                 {
//                     IOCPTransceiverPtr pTrans = _pAccept->acceptEx();
//                     if (!pTrans.get())
//                     {
//                         _TRACE_LOG("AcceptWorkItem: acceptEx");
//                         break;
//                     }
// 
//                     ConnectionPtr connPtr = new Connection(pTrans, _selector);
// 
//                     // 初始化
//                     _selector.initialize( connPtr.get() );
// 
//                     ThreadPool::getSingleton().QueueWorkItem( new AcceptCBWorkItem(connPtr, _server) );
//                 }
//             }
//             catch (...)
//             {
//                 _TRACE_LOG("ERROR break in AcceptWorkItem!");
//             }
// 
//             delete this;
//             return 0;
//         }
//     };	 

} // end namespace hyyun

#endif // end of define __BP_HYWORKITEMACCEPT_HPP_20110628163343__

