
#include "Stdafx.h"
#include "include/hyyun_c.h"
#include "include/hyyun_s.h"

#include "hyyun_c.h"
#include "ClientMessageCenter.h"

//-----------------------------------------------
void Hyyun::internal::__ClientMessageProcessor::onMessageRecv( hyyun::YunClient& conn, const hyc::YunStream& readStream )
{
    if (_processor)
    {
        Container::YunStream^ ys = gcnew Container::YunStream(IntPtr((void*)&readStream));
        _processor->onMessageRecv( gcnew Hyyun::YunClient(IntPtr(&conn)), ys );
        delete ys;
        ys = nullptr;
    }
}
