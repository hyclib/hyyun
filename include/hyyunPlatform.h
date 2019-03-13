//
// Copyright(c) 1999-2015 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#ifndef __BP_HYYUNPLATFORM_H_20150903153302__
#define __BP_HYYUNPLATFORM_H_20150903153302__


#include "hyc/hyc.h"
using namespace hyc;

//----------------------------------------------------------------------------------------------------
//! 导出符号.
//----------------------------------------------------------------------------------------------------

#if CN_PLATFORM == CN_PLATFORM_WINDOWS
#   ifdef HYYUN_S_EXPORTS
#       define HYYUN_API __declspec(dllexport)
#   elif  defined(_LIB) && defined(HYC_COMPILE_STATIC_LIB)
#       define HYYUN_API
#   else
#       define HYYUN_API __declspec(dllimport)
#   endif // HYYUN_EXPORTS
#else
#   define  HYYUN_API
#endif // CN_PLATFORM == CN_PLATFORM_WINDOWS


#include "utils/yunStream.h"
#include "utils/yunStreamAnalyser.h"
#include "utils/yunStreamEmptyAnalyser.hpp"

// YUN_TRACE.
#define YUN_TRACE(format, ...)   hyyun::YunTrace_log((format), ##__VA_ARGS__)

namespace hyyun
{
    // 默认时, debug为enable, release为disable.
    void HYYUN_API YunTrace_enable( bool enable );
    void HYYUN_API YunTrace_log( const tchar* pMsg, ... );
}

#endif // end of define __BP_HYYUNPLATFORM_H_20150903153302__
