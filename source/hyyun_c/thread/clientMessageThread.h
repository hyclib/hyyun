//
// Copyright(c) 1999-2013 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#ifndef __BP_CLIENTMESSAGETHREAD_H_20130325100222__
#define __BP_CLIENTMESSAGETHREAD_H_20130325100222__

#include "../preheader.h"

namespace hyyun
{
    //-----------------------------------------------
    class ClientMessageThread : public Thread
    {
        YunClient&                  _client;
        bool                        _natureExit;

    public:
        ClientMessageThread( YunClient& client ) 
            : _client(client), _natureExit(false)   {}

        void natureExit()
        {
            _natureExit = true;
        }

        virtual bool    onStart(void* lpParam)  { Math::SRand((u32)time(NULL));return true; }
        virtual void    onFinish(void* lpParam) {}
        virtual bool    onInvoke(void* lpParam)
        {
            const YunStream& ys = _client.getMessage();
            if (ys.isLegalYun())
            {
                _client.onMsg(ys);
            }
            //                 else
            //                 {
            //                     Sleep(1000);
            //                 }

            if (_natureExit)
            {
                _client._exitInThread();
                return true;
            }

            return false;
        }
    };

} // end namespace hyyun

#endif // end of define __BP_CLIENTMESSAGETHREAD_H_20130325100222__