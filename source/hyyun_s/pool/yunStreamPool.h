//
// Copyright(c) 1999-2011 hyrj. All rights reserved.
// base network libaray
// author:       lipengxiang
// description:  分配固定长度的内存
//

#ifndef __BP_YUNSTREAMPOOL_H_20110714162120__
#define __BP_YUNSTREAMPOOL_H_20110714162120__


namespace hyyun
{
    //----------------------------------------------------------------------------------------------------
    //!	分配 YunStream容器
    //----------------------------------------------------------------------------------------------------
    class HYYUN_API YunStreamPool
    {
    public:
        // 获得允许的最大包大小.
        static u32          getMaxSize();

        // 如果使用智能指针包裹,则无需调用free.
        static YunStream*   malloc( YunStream::DeallocCallback deallocCB );
        static void         free(YunStream* p);

        // 分配智能指针的方式.
        static YunStream*   mallocRef( YunStream::DeallocCallback deallocCB ) { YunStream* pStream = malloc(deallocCB); pStream->addRef(); pStream->_curPos = NULL; return pStream; }
        static void         freeRef(YunStream* p)   { p->release(); }

        // release unuse memory
        static void         release_memory();

        // release all memory
        static void         purge_memory();

        // 对于非pool创建的YunStream使用pool的buffer来分配空间.
        static void         usePoolBuffer(YunStream* p);

        static String       traceInfo();
    };

} // end namespace hyyun

#endif // end of define __BP_YUNSTREAMPOOL_H_20110714162120__

