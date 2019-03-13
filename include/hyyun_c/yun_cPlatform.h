// (c) 1999-2011 hyrj. All rights reserved.
// base network libaray
// author:       lipengxiang
// description:  
//

#ifndef hyyun_c_hyyun_cPlatform_h
#define hyyun_c_hyyun_cPlatform_h

#include "../hyyunPlatform.h"

//----------------------------------------------------------------------------------------------------
//!	compile option
//----------------------------------------------------------------------------------------------------


//! recevie buffer size.
#define CLI_RECV_BUF_LENGTH     1024*64

//! udp receiver retry times.
#define CLI_TIMEOUT_RETRY_NUM       3

namespace hyyun
{
    class YunClientHandler;
    typedef ObjectPtr<YunClientHandler> YunClientHandlerPtr;
}


#endif
