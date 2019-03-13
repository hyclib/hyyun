//
// Copyright(c) 1999-2013 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#ifndef __BP_HYYUN_S_UDP_H_20130402160310__
#define __BP_HYYUN_S_UDP_H_20130402160310__

#include "yunEndpoint.h"
#include "yunServer.h"
#include "yunUdpAgent.h"

namespace hyyun
{
    //----------------------------------------------------------------------------------------------------
    //*	udp服务器启动参数.
    //----------------------------------------------------------------------------------------------------
    struct YunServerParam_udp
    {
        YunServerParam_udp() : timeout(-1), threadNum(0), queLenRead(8), queLenWrite(8) {}

        /**
         * @brief 超时, <=0则为默认值; (非主服务器时忽略).
         */
        s32         timeout;
        /**
         * @brief 工作线程数, 0则为cpu个数; (非主服务器时忽略).
         */
        u32         threadNum;
        /**
         * @brief 接收队列的长度, 超过此长度若还有消息进入, 则等待. 消息被处理则从队列中移除.
         */
        u32         queLenRead;
        /**
         * @brief 发送队列的长度, 超过此长度若还有消息需发送, 则等待.
         */
        u32         queLenWrite;
    };

    //----------------------------------------------------------------------------------------------------
    //* udp 服务器
    //----------------------------------------------------------------------------------------------------
    class HYYUN_API YunServer_udp : public YunServer
    {
    public:
        YunServer_udp( const YunServerParam_udp& param );
        virtual ~YunServer_udp();

        /**
         * @brief 设置组播网卡接口.
         */
        void                    setMcastInterface( const String& mcastInterface );

        /**
         * @brief 启动服务器. (参见 YunServer::go)
         */
        virtual bool            go( const String& host, int port, ProtocolSupport protocol = ProtocolSupport_IPv4 );

        /**
         * @brief 停止服务器. (参见 YunServer::stop)
         */
        virtual void            stop();

        /**
         * @brief 发送数据. (参见UdpAgent)
         */
        virtual YunStream&   startWrite( u32 reserveSize )                           { YunStream& buf = _pAgent->startWrite(reserveSize); if (_pAnalyser) buf.setAnalyser(_pAnalyser.get()); return buf; }
        virtual void         finishWrite( YunStream& buf, const Endpoint& ep )       { _pAgent->finishWrite(buf, ep); }
        virtual void         cancelWrite( YunStream& buf )                           { _pAgent->cancelWrite(buf); }

        /**
         * @brief 批量发送数据. (参见UdpAgent)
         */
        virtual YunStream&   startBatchWrite( u32 reserveSize )                               { YunStream& buf = _pAgent->startBatchWrite(reserveSize); if (_pAnalyser) buf.setAnalyser(_pAnalyser.get()); return buf; }
        virtual void         finishBatchWrite( YunStream& buf, const Array<Endpoint>& arrEp ) { _pAgent->finishBatchWrite(buf, arrEp); }
        virtual void         cancelBatchWrite( YunStream& buf )                               { _pAgent->cancelBatchWrite(buf); }

    public:
        /**
         * @brief socket错误事件,表明此代理不能再通信.
         */
        typedef EventSingle< void() >     Event_Error;
        Event_Error&        event_error()   { return _eventError; }

        /**
         * @brief 接收到非法消息事件.
         * @param ep 发送非法消息的Endpoint
         */
        typedef EventSingle< void(Endpoint& ep) >    Event_MsgIllegally;
        Event_MsgIllegally& event_msgIllegally()    { return _eventMsgIllegally; }

    protected:
        /**
         * @brief 消息处理.
         */
        virtual void    _onMsg( Endpoint& agent, YunStream& buf, OUT bool& illegally );

    protected:
        YunUdpTransceiver    _trans;
        UdpAgentPtr          _pAgent;
        String               _mcastInterface;

        // event.
        Event_Error          _eventError;
        Event_MsgIllegally   _eventMsgIllegally;
        void                 _callEventError();        
        void                 _callEventMsgIllegally(Endpoint& ep);        

        friend class UdpAgent;
    };

} // end namespace hyyun


#endif // end of define __BP_HYYUN_S_UDP_H_20130402160310__