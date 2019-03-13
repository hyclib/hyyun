//
// Copyright(c) 1999-2011 hyrj. All rights reserved.
// base network libaray
// author:       lipengxiang
// description:  
//

#pragma once

#include "ClientMessageCenter.h"

using namespace Hy;
using namespace System;
using namespace System::Runtime::InteropServices;

namespace Hyyun
{
    ref class YunClient;

    class __YunClientEvent
    {
        gcroot<Hyyun::YunClient^>    _host;
    public:
        __YunClientEvent(Hyyun::YunClient^ host, hyyun::YunClient* y)
        {
            _host = host;
            y->onConnect().add(this, &Hyyun::__YunClientEvent::_onConnect);
            y->onDisconnect().add(this, &Hyyun::__YunClientEvent::_onDisconnect);
        }
        void _onConnect();
        void _onDisconnect();
    };


    public delegate void YunClientConnect(void);
    public delegate void YunClientDisconnect(void);

    /// <summary>
    /// YunClient; 不要对 YunClient的接收发送方法进行多线程编程
    /// </summary>
    public ref class YunClient  
    {
    public:

        /// <summary>
        /// 不自动释放
        /// </summary>
        YunClient( IntPtr ptr )
        {
            _client = (hyyun::YunClient*)ptr.ToPointer();
            _clientEvent = new __YunClientEvent(this, _client);
            _autoRelease = false;
            _timeout = -1;
            _useMessageCenter = false;
        }

        /// <param name="timeout">超时, 小于=0则为默认值</param>
        /// <param name="autoHeartBeat">是否自动使用心跳包</param>
        /// <param name="useMessageCenter">使用消息中心来异步处理消息, 如果使用了消息中心来处理消息, 就不应该调用getMessage</param>
        YunClient( hyc::s32 timeout, bool autoHeartBeat, bool useMessageCenter )
        {
            _timeout = timeout;
            _useMessageCenter = useMessageCenter;

            _client = new hyyun::YunClient(Protocol_Tcp, timeout, autoHeartBeat, useMessageCenter);
            _autoRelease = true;

            _clientEvent = new __YunClientEvent(this, _client);

//             _client->onConnect().add(this, &YunClient::_onConnect);
//             _client->onDisconnect().add(this, &YunClient::_onDisconnect);
        }
        ~YunClient()
        {
            this->!YunClient();
        }
        !YunClient()
        {
            if (_autoRelease)
                SAFE_DELETE(_client);
            _client = NULL;
            SAFE_DELETE(_clientEvent);
        }

        IntPtr  getPtr()
        {
            return IntPtr( (void*)_client );
        }

        /// <summary>
        ///     返回socket
        /// </summary>
        SOCKET  getSocket() { if (_client) return _client->getSocket(); return INVALID_SOCKET; }

        //! \brief
        //!     添加消息处理器
        //! \return
        //!     返回false表示拥有相同的消息包标识符的处理器已经存在or msgProcessor 为空
        bool    addMessageProcessor( IClientMessageProcessor^ msgProcessor )
        {
            if (!_client)   return false;
            if (msgProcessor == nullptr)    return true;
            return _client->addMessageProcessor( new internal::__ClientMessageProcessor(msgProcessor) );
        }


        /// <summary>
        ///     返回false表明连接超时; 连接调用之后就不能再调用addMessageProcessor
        /// </summary>
        bool    connect( System::String^ host, int port )
        {
            if (!_client)   return false;

            Hy::String^ str = gcnew Hy::String();
            *str = host;
            return _client->connect(str->ToCString(), port, hyc::ProtocolSupport_IPv4);
        }

        /// <summary>
        ///     返回false表明连接超时
        /// </summary>
        void    disconnect()
        {
            if (_client)    _client->disconnect();
        }

        /// <summary>
        ///     发送消息指服务端; 返回false表明发送失败;
        /// </summary>
        /// <param name="msg">只要发送/接收不是多线程异步的, msg就可以使用getmessage返回的对象</param>
        bool    sendMessage( Container::YunStream^% msg )
        {
            if (!_client)   return false;

            hyc::YunStream* pmsg = (hyc::YunStream*)msg->getPtr().ToPointer();
            return _client->sendMessage( *pmsg );
        }

        /// <summary>
        ///     当有消息到达时的回调
        ///     如果使用消息中心异步处理消息的话, 要同步接收消息时, 需先暂停消息中心, 接收完数据后再开启
        /// </summary>
        /// <return>
        ///     当YunStream isLegalYun 表明接收正确
        /// </return>
        Container::YunStream^  getMessage()
        {
            if (!_client)   return nullptr;

            hyc::YunStream& ys = _client->getMessage();
            if (ys.isLegalYun())
            {
                return gcnew Container::YunStream( IntPtr((void*)&ys) );
            }
            return nullptr;
        }

        /// <summary>
        /// 发送心跳包, 如果是保持连接的应用一段时间内没有与服务器通信则需要定期发送心跳包来保持连接
        /// </summary>
        bool    sendHeartBeat()
        {
            if (!_client)   return false;
            return _client->sendHeartBeat();
        }

        // 事件处理
    public:

        /// <summary>
        /// 连接之前的回调, 在此做端口绑定等处理.
        /// </summary>
        event YunClientConnect^ OnConnect {
            void add(YunClientConnect^ handler)    { _delegateConnect += handler; }
            void remove(YunClientConnect^ handler) { _delegateConnect -= handler; }
            void raise()                           { _delegateConnect(); }
        }

        /// <summary>
        /// 断线后的回调, 主动断线不会回调.
        /// </summary>
        event YunClientDisconnect^ OnDisconnect {
            void add(YunClientDisconnect^ handler)    { _delegateDisconnect += handler; }
            void remove(YunClientDisconnect^ handler) { _delegateDisconnect -= handler; }
            void raise()                              { _delegateDisconnect(); }
        }

    private:
        __YunClientEvent*   _clientEvent;
        hyyun::YunClient*   _client;
        bool                _autoRelease;
        hyc::s32            _timeout;
        bool                _useMessageCenter;
        YunClientConnect^    _delegateConnect;
        YunClientDisconnect^ _delegateDisconnect;
    };
	

} // end namespace hyyun_cs
