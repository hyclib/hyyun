//
// Copyright(c) 1999-2013 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#ifndef __BP_YUNSERVERSTATICOBJECT_H_20130326155237__
#define __BP_YUNSERVERSTATICOBJECT_H_20130326155237__

namespace hyyun
{
    //----------------------------------------------------------------------------------------------------
    //*	YunObject; 静态通信对象, 对象是所有连接公用的, 并且是线程并发的
    //----------------------------------------------------------------------------------------------------
    class HYYUN_API YunServerHandler : public RefCount
    {
    public:
        YunServerHandler(YunServer* server, u16 mark, YunStatus status) : _server(server), _classMark(mark), _status(status)  { }
        virtual ~YunServerHandler()  {}

        /**
         * @brief 获得能够处理的消息包的标识符.
         */
        virtual const u16&  getMessageMark() const { return _classMark; }
        
        /**
         * @brief 获得通信对象所处的状态(仅tcp拥有状态).
         */
        const YunStatus&    getStatus() const { return _status; }

        /**
         * @brief 获得所在服务器.
         */
        YunServer*          getServer() { return _server; }

        /**
         * @brief 收到消息的回调.
         * @param readStream
         *          接收到的流.
         * @return 返回false表示不再处理此连接 (udp无意义).
         */
        virtual bool        onMessageRecv( Endpoint& ep, const YunStream& readStream, void* userdata ) = 0;

    protected:
        YunServer*  _server;
        u16         _classMark;
        YunStatus   _status;
    };
    typedef ObjectPtr<YunServerHandler> YunServerHandlerPtr;

} // end namespace hyyun

#endif // end of define __BP_YUNSERVERSTATICOBJECT_H_20130326155237__
