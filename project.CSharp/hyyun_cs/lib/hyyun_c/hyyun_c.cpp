#include "Stdafx.h"
#include "hyc/include/hyc.h"
#include "hynet/hyYun/include/hyyun_c.h"
#include "hyyun_c.h"


//-----------------------------------------------
void Hyyun::__YunClientEvent::_onConnect()
{
    _host->OnConnect();
}

//-----------------------------------------------
void Hyyun::__YunClientEvent::_onDisconnect()
{
    _host->OnDisconnect();

}
