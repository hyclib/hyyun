//
// Copyright(c) 1999-2011 hyrj. All rights reserved.
// base network libaray
// author:       lipengxiang
// description:  
//

#pragma once

#include <vcclr.h>
#using <mscorlib.dll>

using namespace Hy;
using namespace System;
using namespace System::Runtime::InteropServices;

namespace Hyyun
{
    ref class YunClient;

    /// <summary>
    /// ClientMessageProcessor; 消息处理器, 用来处理匹配标识的消息包
    /// </summary>
    public interface class IClientMessageProcessor  
    {
    public:
        /// <summary>
        /// 获得能够处理的消息包的标识符
        /// </summary>
        hyc::u16     getMessageMark();

        /// <summary>
        /// 收到消息的回调
        ///     (能够保证一个连接的接收是单线程的, 同一时刻, 只可能有一个onMessageRecv函数在运行)
        /// </summary>
        /// <param name="readStream">接收到的流</param>
        void    onMessageRecv( Hyyun::YunClient^% conn, Container::YunStream^% readStream );
    };

    namespace internal
    {
        class __ClientMessageProcessor : public hyyun::ClientMessageProcessor
        {
            gcroot<Hyyun::IClientMessageProcessor^>    _processor;
        public:
            __ClientMessageProcessor( IClientMessageProcessor^ pp )  { _processor = pp; }

            //! 获得能够处理的消息包的标识符
            virtual u16     getMessageMark() const
            {
                if (_processor)
                    return _processor->getMessageMark();
                return -1;
            }

            //! \brief
            //!     收到消息的回调
            //!     (能够保证一个连接的接收是单线程的, 同一时刻, 只可能有一个onMessageRecv函数在运行)
            //! \param readStream
            //!     接收到的流
            virtual void    onMessageRecv( hyyun::YunClient& conn, const hyc::YunStream& readStream );
        };
    }

} // end namespace hyyun_cs
