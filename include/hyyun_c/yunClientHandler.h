//
// Copyright(c) 1999-2012 hyrj. All rights reserved.
// base network libaray
// author:       lipengxiang
// description:  
//

#ifndef __BP_YUNCLIENTOBJECT_H_20120131143703__
#define __BP_YUNCLIENTOBJECT_H_20120131143703__


//! 创建流(线程安全)
#define CLI_STREAM_CREATE_SAFE   YunStream __ys;
//! 获得发送的流
#define CLI_STREAM_GET      __ys
//! 获得通信对象
#define CLI_CLIENT_GET      _yunClient

namespace hyyun
{
    class YunClient;

    //----------------------------------------------------------------------------------------------------
    //!	YunClientHandler; 客户端通信对象
    //! 自定的通信函数在数据发送失败的时候会抛出 YunSendFailException 异常
    //----------------------------------------------------------------------------------------------------
    class HYYUN_API YunClientHandler : public RefCount
    {
    public:
        YunClientHandler( YunClient* yunClient, u16 mark ) : _yunClient(yunClient), _mark(mark)  {}
        virtual ~YunClientHandler()  {}

        //
        //! 获得客户端对象.
        //
        YunClient*      getClient() { return _yunClient; }

        //
        //! 获得能够处理的消息包的标识符
        //
        const u16&      getMessageMark() const  { return _mark; }

        //! \brief
        //!     收到消息的回调
        //!     (能够保证一个连接的接收是单线程的, 同一时刻, 只可能有一个onMessageRecv函数在运行)
        //! \param readStream
        //!     接收到的流.
        virtual void    onMessageRecv( const YunStream& readStream, void* userdata ) = 0;

    protected:
        YunClient*  _yunClient;
        u16         _mark;
    };
    typedef ObjectPtr<YunClientHandler> YunClientHandlerPtr;
	

} // end namespace hyyun

#endif // end of define __BP_YUNCLIENTOBJECT_H_20120131143703__

