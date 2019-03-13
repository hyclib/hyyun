//
// Copyright(c) 1999-2011 hyrj. All rights reserved.
// base network libaray
// author:       lipengxiang
// description:  
//

#include "preheader.h"
#include "thread/serverMemoryPoolThread.h"

//-----------------------------------------------
inline static void dll_init()
{
    CN_LEAK_DETECTOR;
    print_debug_line(_T("load hyyun_s"));

    new hyyun::ServerMemoryPoolThread;
    hyyun::ServerMemoryPoolThread::getSingletonPtr()->start();

}

//-----------------------------------------------
inline static void dll_destory()
{
    if (hyyun::ServerMemoryPoolThread::getSingletonPtr())
    {
        delete hyyun::ServerMemoryPoolThread::getSingletonPtr();
    }
    print_debug_line(_T("unload hyyun_s"));
}


#if CN_PLATFORM == CN_PLATFORM_WINDOWS
namespace hyyun
{
    //----------------------------------------------------------------------------------------------------
    //! dll初始化
    //----------------------------------------------------------------------------------------------------

    class DLLInit
    {
    public:
        /*---------------------------
         初始化
        ---------------------------*/
        DLLInit()
        {
            dll_init();
        }

        /*---------------------------
         析构
        ---------------------------*/
        ~DLLInit()
        {
            dll_destory();
        }
    };
    static DLLInit g_dllInit;

} // end namespace hyyun
#else
__attribute__((constructor)) void dll_main_hyyun_init() { 
    dll_init();
} 

__attribute__((destructor)) void dll_main_hyyun_destory() { 
    dll_destory();
} 
#endif
