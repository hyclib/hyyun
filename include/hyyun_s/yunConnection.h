//
// Copyright(c) 1999-2013 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#ifndef __BP_HYCONNECTION_H_20130407162106__
#define __BP_HYCONNECTION_H_20130407162106__

#include "yunEndpoint.h"
#include "yunTcpAgent.h"

namespace hyyun
{
    //----------------------------------------------------------------------------------------------------
    //*	绑定的数据, 不要保存ObjectPtr<Connection>类型成员变量, 否则可能造成内存丢失.
    //* Connection->ConnectionDataBind, Connection拥有ConnectionDataBind成员, 
    //* 如果ConnectionDataBind中存储了Connection的指针, 注意在Connection释放时及时清除.
    //----------------------------------------------------------------------------------------------------
    class HYYUN_API ConnectionDataBind : public RefCountSafe
    {
    public:
        virtual ~ConnectionDataBind()   {}
    };
    typedef ObjectPtr<ConnectionDataBind>   ConnectionDataBindPtr;


    //----------------------------------------------------------------------------------------------------
    //*	Connection代表一个连接, 连接只能串行读取数据,但可以并行发送数据.
    //* 连接由系统来维护,可以调用close来关闭当前连接. (更多操作参见TcpAgent)
    //----------------------------------------------------------------------------------------------------
    class HYYUN_API Connection : public TcpAgent
    {
    public:
        ~Connection();  // can't be inherited.

        /**
         * @brief register the databind.
         */
        inline void                     setDataBind( ConnectionDataBindPtr pDatabinding )   { _pDatabinding = pDatabinding; }
        inline ConnectionDataBindPtr    getDataBind()                                       { return _pDatabinding; }


    protected:
        Connection( YunServer_tcp& server, YunTransceiverPtr& pTrans, const struct sockaddr_storage& addr );
        virtual void        onDestroy();

    protected:
        ConnectionDataBindPtr  _pDatabinding;

    public:
        static String  TraceInfo();

        friend class ConnectionPool;
    };

    typedef ObjectPtr<Connection> ConnectionPtr;

} // end namespace hyyun

#endif // end of define __BP_HYCONNECTION_H_20130407162106__
