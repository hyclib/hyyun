//
// Copyright(c) 1999-2013 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#ifndef __BP_HYYUN_C_P_HPP_20130408101656__
#define __BP_HYYUN_C_P_HPP_20130408101656__


namespace hyyun
{
    //----------------------------------------------------------------------------------------------------
    //! YunClient::PrivateClass
    //----------------------------------------------------------------------------------------------------
    class YunClient::PrivateClass
    {
    private:
        bool            _isDisconnect;
    public:
        YunClient&      _client;
        bool            _autoRecv;
        bool            _autoHeartBeat;
        TransceiverPtr  _pTransceiver;
        ProtocolType    _protocol;
        s32             _timeout;
        Time            _preSend;   // 上次发送时间.
        u32             _heartBeatElapse;
        AtomicBase<u32> _sequence;
//         u32 _sequence;

        ThreadPtr       _pMsgThread;
        ThreadPtr       _pHeartBeatThread;
        ClientMessageCenter*  _pMsgCenter;

        YunStream       _recvBuf;
        YunStream       _recvBuf2;
        YunStream       _heartBeat;

        SyncObject      _sendSync;
        bool            _extSocket;

        YunStreamAnalyserPtr    _pAnalyser;

        Event_Disconnectd _eventDisconnect;
        Event_Connect   _eventConnecting;

    public:
        bool    isDisconnect() const  { return _isDisconnect || _pTransceiver.isEmpty(); }
        void    disconnect( bool notifyCB = true )  
        {
            if (!_isDisconnect) { _isDisconnect = true; _eventDisconnect(!notifyCB); onDisconnected(); }
        }


    public:
        PrivateClass( YunClient& pParent, ProtocolType protocol, s32 timeout )
            : _autoRecv(false)
            , _autoHeartBeat(false)
            , _isDisconnect(true)
        	, _client(pParent)
            , _protocol(protocol)
            , _timeout(timeout)
        	, _preSend(DateTime::GetSysTime()-_heartBeatElapse/2)
        	, _heartBeatElapse(1000*15)
        	, _sequence(0)
            , _extSocket(false)
        {
            _pMsgCenter = new ClientMessageCenter();

            _recvBuf.resize( YUN_CONTENT_MAX_LENGTH );
            _recvBuf._curPos = NULL;
        }

        ~PrivateClass()
        {
            SAFE_DELETE(_pMsgCenter);
        }

        //-----------------------------------------------
        // 连接成功.
        void onConnected()
        {
            SyncTokenPtr token = _sendSync.getToken();

            assert(_isDisconnect);
            _isDisconnect = false;
        }

    private:
        void onDisconnected()
        {
            if (_pTransceiver)
                _pTransceiver->close();

            // 心跳.
            _client.autoHeartBeat(false);

            // 接收线程.
            _client.autoRecvMessage(false);

            SyncTokenPtr token = _sendSync.getToken();
            _pTransceiver = NULL;
        }
    };

} // end namespace hyyun

#endif // end of define __BP_HYYUN_C_P_HPP_20130408101656__
