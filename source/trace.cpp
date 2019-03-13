//
// Copyright(c) 1999-2015 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#include "../include/hyyun_s/yun_sPlatform.h"

#ifdef _DEBUG
static bool g_enable = true;
#else
static bool g_enable = false;
#endif // _DEBUG

namespace hyyun
{
    //-----------------------------------------------
    void YunTrace_enable( bool enable )
    {
        g_enable = enable;
    }

    //-----------------------------------------------
    void YunTrace_log( const tchar* pMsg, ... )
    {
        if (g_enable)
        {
            int err = Platform::GetLastError();

            String str2;
            va_list arg_ptr;
            va_start(arg_ptr, pMsg);
            str2.vformat(pMsg, arg_ptr);
            va_end(arg_ptr);

            DateTime dt;
            dt.setToSysTime();

            String strTime;
            strTime.format(_T("[%hu-%hu-%hu %hu:%hu:%hu]\n"), dt.getYear(), dt.getMonth(), dt.getDay(), dt.getHours(), dt.getMinutes(), dt.getSeconds());
            
            str2 = strTime + str2;

            print_all_line(_T("%s"), str2.c_str());
        } // if.
    }
}

