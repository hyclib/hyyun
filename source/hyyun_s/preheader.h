//
// Copyright(c) 1999-2012 hyrj. All rights reserved.
// base network libaray
// author:       lipengxiang
// description:  
//

#ifndef __BP_PREHEADER_H_20120203101227__
#define __BP_PREHEADER_H_20120203101227__

#include "hyc/hyc.h"
#include "hyc/hycLeakDetector.h"
using namespace hyc;
#include "hymq/hymq.h"

#include "../../include/hyyun_s/yun_sPlatform.h"
#include "../../include/hyyun_s/yunServer.h"


#define  YUN_TRACE_POOL

// tcp心跳标记.
#define TCP_HEARTBEAT_MARK   1
#define TCP_HEARTBEAT_REMOVE 0

#define TCP_DO_PACKAGE_COUNT_BATCH  4   // 每个socket每次最多处理的数据包个数.

#endif // end of define __BP_PREHEADER_H_20120203101227__

