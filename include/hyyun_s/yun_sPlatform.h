//
// Copyright(c) 1999-2011 hyrj. All rights reserved.
// base network libaray
// author:       lipengxiang
// description:  only support WIndows platform
//

#ifndef __BP_HYYUN_SPLATFORM_H_20110620172859__
#define __BP_HYYUN_SPLATFORM_H_20110620172859__

#include "../hyyunPlatform.h"

//----------------------------------------------------------------------------------------------------
//!	无锁消息队列命名空间.
//---------------------------------------------------------------------------------------------------- 

namespace hymq
{
    class MessageConsumer;
    class MessageQueue;
}

//----------------------------------------------------------------------------------------------------
//!	常用类声明.
//----------------------------------------------------------------------------------------------------

namespace hyyun
{
    typedef s32 YunStatus;

    class   YunServer;
    class   YunServer_udp;
    class   YunServer_tcp;
    class   Endpoint;
    class   ConnectionPool;
    class   MessageQueue;

    class   TcpAgent;
    class   TcpHeartBeat;
    class   TcpMessageConsumer;
    class   TcpConnectConsumer;
    class   TcpConnectEvent;
    struct  TcpConnectMessageData;
    typedef AsyncData TcpAsyncData;
    
    class   UdpMessageConsumer;
    class   UdpWriteConsumer;
    struct  UdpMessageData;
    struct  UdpAsyncData;

    //
    // 跨平台通信器名称定义.
    //
#ifdef CN_NET_IOCP
    typedef AsyncTransceiver        YunTransceiver;
    typedef AsyncTransceiverPtr     YunTransceiverPtr;
    typedef IOCPTcpAcceptor         YunTcpAcceptor;
    typedef IOCPTcpTransceiver      YunTcpTransceiver;
    typedef IOCPUdpTransceiver      YunUdpTransceiver;
    typedef IOCPSelector            YunSelector;
#elif defined(CN_NET_EPOLL)
    typedef AsyncTransceiver        YunTransceiver;
    typedef AsyncTransceiverPtr     YunTransceiverPtr;
    typedef EPOLLTcpAcceptor        YunTcpAcceptor;
    typedef EPOLLTcpTransceiver     YunTcpTransceiver;
    typedef EPOLLUdpTransceiver     YunUdpTransceiver;
    typedef EpollSelector           YunSelector;
#else
    typedef Transceiver             YunTransceiver;
    typedef TransceiverPtr          YunTransceiverPtr;
    typedef TcpAcceptor             YunTcpAcceptor;
    typedef TcpTransceiver          YunTcpTransceiver;
    typedef UdpTransceiver          YunUdpTransceiver;
#endif // CN_NET_IOCP


} // end namespace of hyyun


//----------------------------------------------------------------------------------------------------
//!	编译选项.
//----------------------------------------------------------------------------------------------------

//! 每个客户端的接收发送缓冲大小
//! (客户端发送来的包, 每个不能大于SER_RECV_BUF_LENGTH) 最大的udp包.
#define SER_RECV_BUF_LENGTH         UdpTransceiver::MAX_PACKET_SIZE
#define SER_RECV_BUF_INIT_LENGTH    16

//! 预留的处理器的最大mark. (不可修改).
#define SER_MARK_MAX_RESERVED   9
#define SER_MARK_MIN_BUNDLE     SER_MARK_MAX_RESERVED
#define SER_MARK_MAX_BUNDLE     10000

//! 传输器状态原子比较.
#define TRANS_ATOMIC_SET   1
#define TRANS_ATOMIC_UNSET 0

//! 是否在队列中原子比较.
#define IN_QUEUE_ATOMIC_SET   1
#define IN_QUEUE_ATOMIC_UNSET 0


#if CN_PLATFORM == CN_PLATFORM_WINDOWS
#pragma warning(disable: 4251)
#pragma warning(disable: 4996)
#pragma warning(disable: 4355)
#endif



#endif // end of define __BP_HYYUN_SPLATFORM_H_20110620172859__
