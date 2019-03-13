//
// Copyright(c) 1999-2011 hyrj. All rights reserved.
// base network libaray
// author:       lipengxiang
// description:  
//

#include "../preheader.h"

#ifdef new
#undef new
#endif

#include "yunStreamPool.h"

#include "boost/pool/singleton_pool.hpp"
#include "poolTemplate.hpp"


namespace hyyun
{

    //----------------------------------------------------------------------------------------------------
    //!	外部流pool回调
    //----------------------------------------------------------------------------------------------------

    static void YunStreamPoolDeallocCallback( Byte* pBuffer, u32 uSize );
    static void YunStreamPoolReallocCallback( Byte*& pBuffer, u32 uSrcSize, u32& uNewSize );


    //----------------------------------------------------------------------------------------------------
    //!	YunStreamPool_chunk
    //----------------------------------------------------------------------------------------------------
    template<u32 chunkSize>
    class YunStreamPool_chunk
    {
        typedef boost::singleton_pool<YunStreamPool_chunk<chunkSize>, chunkSize>  Singleton_YunStreamPool_chunk;
    public:
        static const u32   size = chunkSize;

#ifdef YUN_TRACE_POOL
        static AtomicBase<s32> numMalloc;
        static AtomicBase<s32> numFree;
        static AtomicBase<s32> numTotal;
#endif // YUN_TRACE_POOL

#ifdef YUN_TRACE_POOL
        static String       traceInfo()
        {
            String buf;
            buf.format(_T("YunStreamPool%d"), size);
            buf.fill(20, _T(' '));
            String buf2;
            buf2.format(_T(": %d %d %d\n"), (s32)numMalloc, (s32)numFree, (s32)numTotal);
            return buf+buf2;
        }
#else
        static String      traceInfo() { return _T(""); }
#endif // YUN_TRACE_POOL

        static Byte*   malloc()
        {
#ifdef YUN_TRACE_POOL
            ++numTotal;
            ++numMalloc;
#endif // YUN_TRACE_POOL
            Byte* p = (Byte*)Singleton_YunStreamPool_chunk::malloc();
            if (!p)
            {
                YunStreamPool::release_memory();
                trace(_T("Singleton_YunStreamPool_chunk 整理了一次内存"));
                p = (Byte*)Singleton_YunStreamPool_chunk::malloc();
            }

            if (p)  return p;

            trace_all(_T("YunStreamPool_chunk x ::malloc 内存不足."));
            exit(0);

            return NULL;
        }
        static void         free(Byte* p)
        {
#ifdef YUN_TRACE_POOL
            --numTotal;
            ++numFree;
#endif // YUN_TRACE_POOL
            if (p) Singleton_YunStreamPool_chunk::free(p);
        }
        static void         purge_memory()
        {
            Singleton_YunStreamPool_chunk::purge_memory();
        }
        static void         release_memory()
        {
            Singleton_YunStreamPool_chunk::release_memory();
        }
    };


    typedef boost::singleton_pool<YunStream, sizeof(YunStream)>  Singleton_YunStreamPool;
    typedef YunStreamPool_chunk<16>         YunStreamPool_16;
    typedef YunStreamPool_chunk<32>         YunStreamPool_32;
    typedef YunStreamPool_chunk<64>         YunStreamPool_64;
    typedef YunStreamPool_chunk<128>        YunStreamPool_128;
    typedef YunStreamPool_chunk<256>        YunStreamPool_256;
    typedef YunStreamPool_chunk<512>        YunStreamPool_512;
    typedef YunStreamPool_chunk<1024>       YunStreamPool_1K;
    typedef YunStreamPool_chunk<1024*2>     YunStreamPool_2K;
    typedef YunStreamPool_chunk<1024*4>     YunStreamPool_4K;
    typedef YunStreamPool_chunk<1024*6>     YunStreamPool_6K;
    typedef YunStreamPool_chunk<1024*8>     YunStreamPool_8K;
    typedef YunStreamPool_chunk<1024*10>    YunStreamPool_10K;
    typedef YunStreamPool_chunk<1024*12>    YunStreamPool_12K;
    typedef YunStreamPool_chunk<1024*14>    YunStreamPool_14K;
    typedef YunStreamPool_chunk<1024*16>    YunStreamPool_16K;
    typedef YunStreamPool_chunk<1024*18>    YunStreamPool_18K;
    typedef YunStreamPool_chunk<1024*20>    YunStreamPool_20K;
    typedef YunStreamPool_chunk<1024*22>    YunStreamPool_22K;
    typedef YunStreamPool_chunk<1024*24>    YunStreamPool_24K;
    typedef YunStreamPool_chunk<1024*26>    YunStreamPool_26K;
    typedef YunStreamPool_chunk<1024*28>    YunStreamPool_28K;
    typedef YunStreamPool_chunk<1024*30>    YunStreamPool_30K;
    typedef YunStreamPool_chunk<1024*32>    YunStreamPool_32K;
    typedef YunStreamPool_chunk<1024*34>    YunStreamPool_34K;
    typedef YunStreamPool_chunk<1024*36>    YunStreamPool_36K;
    typedef YunStreamPool_chunk<1024*38>    YunStreamPool_38K;
    typedef YunStreamPool_chunk<1024*40>    YunStreamPool_40K;
    typedef YunStreamPool_chunk<1024*42>    YunStreamPool_42K;
    typedef YunStreamPool_chunk<1024*44>    YunStreamPool_44K;
    typedef YunStreamPool_chunk<1024*46>    YunStreamPool_46K;
    typedef YunStreamPool_chunk<1024*48>    YunStreamPool_48K;
    typedef YunStreamPool_chunk<1024*50>    YunStreamPool_50K;
    typedef YunStreamPool_chunk<1024*52>    YunStreamPool_52K;
    typedef YunStreamPool_chunk<1024*54>    YunStreamPool_54K;
    typedef YunStreamPool_chunk<1024*56>    YunStreamPool_56K;
    typedef YunStreamPool_chunk<1024*58>    YunStreamPool_58K;
    typedef YunStreamPool_chunk<1024*60>    YunStreamPool_60K;
    typedef YunStreamPool_chunk<1024*62>    YunStreamPool_62K;
    typedef YunStreamPool_chunk<1024*64>    YunStreamPool_64K;

#ifdef YUN_TRACE_POOL
#define STATIC_MEMBER_IMPL(type)    template<> AtomicBase<s32> type::numMalloc = 0; template<> AtomicBase<s32> type::numFree = 0; template<> AtomicBase<s32> type::numTotal = 0
    STATIC_MEMBER_IMPL(YunStreamPool_16);
    STATIC_MEMBER_IMPL(YunStreamPool_32);
    STATIC_MEMBER_IMPL(YunStreamPool_64);
    STATIC_MEMBER_IMPL(YunStreamPool_128);
    STATIC_MEMBER_IMPL(YunStreamPool_256);
    STATIC_MEMBER_IMPL(YunStreamPool_512);
    STATIC_MEMBER_IMPL(YunStreamPool_1K);
    STATIC_MEMBER_IMPL(YunStreamPool_2K);
    STATIC_MEMBER_IMPL(YunStreamPool_4K);
    STATIC_MEMBER_IMPL(YunStreamPool_6K);
    STATIC_MEMBER_IMPL(YunStreamPool_8K);
    STATIC_MEMBER_IMPL(YunStreamPool_10K);
    STATIC_MEMBER_IMPL(YunStreamPool_12K);
    STATIC_MEMBER_IMPL(YunStreamPool_14K);
    STATIC_MEMBER_IMPL(YunStreamPool_16K);
    STATIC_MEMBER_IMPL(YunStreamPool_18K);
    STATIC_MEMBER_IMPL(YunStreamPool_20K);
    STATIC_MEMBER_IMPL(YunStreamPool_22K);
    STATIC_MEMBER_IMPL(YunStreamPool_24K);
    STATIC_MEMBER_IMPL(YunStreamPool_26K);
    STATIC_MEMBER_IMPL(YunStreamPool_28K);
    STATIC_MEMBER_IMPL(YunStreamPool_30K);
    STATIC_MEMBER_IMPL(YunStreamPool_32K);
    STATIC_MEMBER_IMPL(YunStreamPool_34K);
    STATIC_MEMBER_IMPL(YunStreamPool_36K);
    STATIC_MEMBER_IMPL(YunStreamPool_38K);
    STATIC_MEMBER_IMPL(YunStreamPool_40K);
    STATIC_MEMBER_IMPL(YunStreamPool_42K);
    STATIC_MEMBER_IMPL(YunStreamPool_44K);
    STATIC_MEMBER_IMPL(YunStreamPool_46K);
    STATIC_MEMBER_IMPL(YunStreamPool_48K);
    STATIC_MEMBER_IMPL(YunStreamPool_50K);
    STATIC_MEMBER_IMPL(YunStreamPool_52K);
    STATIC_MEMBER_IMPL(YunStreamPool_54K);
    STATIC_MEMBER_IMPL(YunStreamPool_56K);
    STATIC_MEMBER_IMPL(YunStreamPool_58K);
    STATIC_MEMBER_IMPL(YunStreamPool_60K);
    STATIC_MEMBER_IMPL(YunStreamPool_62K);
    STATIC_MEMBER_IMPL(YunStreamPool_64K);
#endif // YUN_TRACE_POOL

    //----------------------------------------------------------------------------------------------------
    //!	YunStreamPoolDeallocCallback
    //----------------------------------------------------------------------------------------------------

    //-----------------------------------------------
    void YunStreamPoolDeallocCallback( Byte* pBuffer, u32 uSize )
    {
        if (pBuffer)
        {
            switch (uSize)
            {
            case YunStreamPool_16::size:
                YunStreamPool_16::free(pBuffer);
                break;
            case YunStreamPool_32::size:
                YunStreamPool_32::free(pBuffer);
                break;
            case YunStreamPool_64::size:
                YunStreamPool_64::free(pBuffer);
                break;
            case YunStreamPool_128::size:
                YunStreamPool_128::free(pBuffer);
                break;
            case YunStreamPool_256::size:
                YunStreamPool_256::free(pBuffer);
                break;
            case YunStreamPool_512::size:
                YunStreamPool_512::free(pBuffer);
                break;
            case YunStreamPool_1K::size:
                YunStreamPool_1K::free(pBuffer);
                break;
            case YunStreamPool_2K::size:
                YunStreamPool_2K::free(pBuffer);
                break;
            case YunStreamPool_4K::size:
                YunStreamPool_4K::free(pBuffer);
                break;
            case YunStreamPool_6K::size:
                YunStreamPool_6K::free(pBuffer);
                break;
            case YunStreamPool_8K::size:
                YunStreamPool_8K::free(pBuffer);
                break;
            case YunStreamPool_10K::size:
                YunStreamPool_10K::free(pBuffer);
                break;
            case YunStreamPool_12K::size:
                YunStreamPool_12K::free(pBuffer);
                break;
            case YunStreamPool_14K::size:
                YunStreamPool_14K::free(pBuffer);
                break;
            case YunStreamPool_16K::size:
                YunStreamPool_16K::free(pBuffer);
                break;
            case YunStreamPool_18K::size:
                YunStreamPool_18K::free(pBuffer);
                break;
            case YunStreamPool_20K::size:
                YunStreamPool_20K::free(pBuffer);
                break;
            case YunStreamPool_22K::size:
                YunStreamPool_22K::free(pBuffer);
                break;
            case YunStreamPool_24K::size:
                YunStreamPool_24K::free(pBuffer);
                break;
            case YunStreamPool_26K::size:
                YunStreamPool_26K::free(pBuffer);
                break;
            case YunStreamPool_28K::size:
                YunStreamPool_28K::free(pBuffer);
                break;
            case YunStreamPool_30K::size:
                YunStreamPool_30K::free(pBuffer);
                break;
            case YunStreamPool_32K::size:
                YunStreamPool_32K::free(pBuffer);
                break;
            case YunStreamPool_34K::size:
                YunStreamPool_34K::free(pBuffer);
                break;
            case YunStreamPool_36K::size:
                YunStreamPool_36K::free(pBuffer);
                break;
            case YunStreamPool_38K::size:
                YunStreamPool_38K::free(pBuffer);
                break;
            case YunStreamPool_40K::size:
                YunStreamPool_40K::free(pBuffer);
                break;
            case YunStreamPool_42K::size:
                YunStreamPool_42K::free(pBuffer);
                break;
            case YunStreamPool_44K::size:
                YunStreamPool_44K::free(pBuffer);
                break;
            case YunStreamPool_46K::size:
                YunStreamPool_46K::free(pBuffer);
                break;
            case YunStreamPool_48K::size:
                YunStreamPool_48K::free(pBuffer);
                break;
            case YunStreamPool_50K::size:
                YunStreamPool_50K::free(pBuffer);
                break;
            case YunStreamPool_52K::size:
                YunStreamPool_52K::free(pBuffer);
                break;
            case YunStreamPool_54K::size:
                YunStreamPool_54K::free(pBuffer);
                break;
            case YunStreamPool_56K::size:
                YunStreamPool_56K::free(pBuffer);
                break;
            case YunStreamPool_58K::size:
                YunStreamPool_58K::free(pBuffer);
                break;
            case YunStreamPool_60K::size:
                YunStreamPool_60K::free(pBuffer);
                break;
            case YunStreamPool_62K::size:
                YunStreamPool_62K::free(pBuffer);
                break;
            case YunStreamPool_64K::size:
                YunStreamPool_64K::free(pBuffer);
                break;
            default:
                trace( _T("YunStreamPoolDeallocCallback error size") );
                assert(false);
                return;
            }
        }
    }

    //-----------------------------------------------
    void YunStreamPoolReallocCallback( Byte*& pBuffer, u32 uSrcSize, u32& uNewSize )
    {
        assert(uSrcSize <= uNewSize);
        if (uSrcSize == uNewSize)
        {
            return;
        }

        //-----------------------------------------------
        // 改变uNewSize 值
        Byte* pNewBuf = NULL;

        if ( YunStreamPool_16::size >= uNewSize )
        {
            uNewSize = YunStreamPool_16::size;
            pNewBuf = YunStreamPool_16::malloc();
        }
        else if ( YunStreamPool_16::size < uNewSize && uNewSize <= YunStreamPool_32::size )
        {
            uNewSize = YunStreamPool_32::size;
            pNewBuf = YunStreamPool_32::malloc();
        }
        else if ( YunStreamPool_32::size < uNewSize && uNewSize <= YunStreamPool_64::size )
        {
            uNewSize = YunStreamPool_64::size;
            pNewBuf = YunStreamPool_64::malloc();
        }
        else if ( YunStreamPool_64::size < uNewSize && uNewSize <= YunStreamPool_128::size )
        {
            uNewSize = YunStreamPool_128::size;
            pNewBuf = YunStreamPool_128::malloc();
        }
        else if ( YunStreamPool_128::size < uNewSize && uNewSize <= YunStreamPool_256::size )
        {
            uNewSize = YunStreamPool_256::size;
            pNewBuf = YunStreamPool_256::malloc();
        }
        else if ( YunStreamPool_256::size < uNewSize && uNewSize <= YunStreamPool_512::size )
        {
            uNewSize = YunStreamPool_512::size;
            pNewBuf = YunStreamPool_512::malloc();
        }
        else if ( YunStreamPool_512::size < uNewSize && uNewSize <= YunStreamPool_1K::size )
        {
            uNewSize = YunStreamPool_1K::size;
            pNewBuf = YunStreamPool_1K::malloc();
        }
        else if ( YunStreamPool_1K::size < uNewSize && uNewSize <= YunStreamPool_2K::size )
        {
            uNewSize = YunStreamPool_2K::size;
            pNewBuf = YunStreamPool_2K::malloc();
        }
        else if ( YunStreamPool_2K::size < uNewSize && uNewSize <= YunStreamPool_4K::size )
        {
            uNewSize = YunStreamPool_4K::size;
            pNewBuf = YunStreamPool_4K::malloc();
        }
        else if ( YunStreamPool_4K::size < uNewSize && uNewSize <= YunStreamPool_6K::size )
        {
            uNewSize = YunStreamPool_6K::size;
            pNewBuf = YunStreamPool_6K::malloc();
        }
        else if ( YunStreamPool_6K::size < uNewSize && uNewSize <= YunStreamPool_8K::size )
        {
            uNewSize = YunStreamPool_8K::size;
            pNewBuf = YunStreamPool_8K::malloc();
        }
        else if ( YunStreamPool_8K::size < uNewSize && uNewSize <= YunStreamPool_10K::size )
        {
            uNewSize = YunStreamPool_10K::size;
            pNewBuf = YunStreamPool_10K::malloc();
        }
        else if ( YunStreamPool_10K::size < uNewSize && uNewSize <= YunStreamPool_12K::size )
        {
            uNewSize = YunStreamPool_12K::size;
            pNewBuf = YunStreamPool_12K::malloc();
        }
        else if ( YunStreamPool_12K::size < uNewSize && uNewSize <= YunStreamPool_14K::size )
        {
            uNewSize = YunStreamPool_14K::size;
            pNewBuf = YunStreamPool_14K::malloc();
        }
        else if ( YunStreamPool_14K::size < uNewSize && uNewSize <= YunStreamPool_16K::size )
        {
            uNewSize = YunStreamPool_16K::size;
            pNewBuf = YunStreamPool_16K::malloc();
        }
        else if ( YunStreamPool_16K::size < uNewSize && uNewSize <= YunStreamPool_18K::size )
        {
            uNewSize = YunStreamPool_18K::size;
            pNewBuf = YunStreamPool_18K::malloc();
        }
        else if ( YunStreamPool_18K::size < uNewSize && uNewSize <= YunStreamPool_20K::size )
        {
            uNewSize = YunStreamPool_20K::size;
            pNewBuf = YunStreamPool_20K::malloc();
        }
        else if ( YunStreamPool_20K::size < uNewSize && uNewSize <= YunStreamPool_22K::size )
        {
            uNewSize = YunStreamPool_22K::size;
            pNewBuf = YunStreamPool_22K::malloc();
        }
        else if ( YunStreamPool_22K::size < uNewSize && uNewSize <= YunStreamPool_24K::size )
        {
            uNewSize = YunStreamPool_24K::size;
            pNewBuf = YunStreamPool_24K::malloc();
        }
        else if ( YunStreamPool_24K::size < uNewSize && uNewSize <= YunStreamPool_26K::size )
        {
            uNewSize = YunStreamPool_26K::size;
            pNewBuf = YunStreamPool_26K::malloc();
        }
        else if ( YunStreamPool_26K::size < uNewSize && uNewSize <= YunStreamPool_28K::size )
        {
            uNewSize = YunStreamPool_28K::size;
            pNewBuf = YunStreamPool_28K::malloc();
        }
        else if ( YunStreamPool_28K::size < uNewSize && uNewSize <= YunStreamPool_30K::size )
        {
            uNewSize = YunStreamPool_30K::size;
            pNewBuf = YunStreamPool_30K::malloc();
        }
        else if ( YunStreamPool_30K::size < uNewSize && uNewSize <= YunStreamPool_32K::size )
        {
            uNewSize = YunStreamPool_32K::size;
            pNewBuf = YunStreamPool_32K::malloc();
        }
        else if ( YunStreamPool_32K::size < uNewSize && uNewSize <= YunStreamPool_34K::size )
        {
            uNewSize = YunStreamPool_34K::size;
            pNewBuf = YunStreamPool_34K::malloc();
        }
        else if ( YunStreamPool_34K::size < uNewSize && uNewSize <= YunStreamPool_36K::size )
        {
            uNewSize = YunStreamPool_36K::size;
            pNewBuf = YunStreamPool_36K::malloc();
        }
        else if ( YunStreamPool_36K::size < uNewSize && uNewSize <= YunStreamPool_38K::size )
        {
            uNewSize = YunStreamPool_38K::size;
            pNewBuf = YunStreamPool_38K::malloc();
        }
        else if ( YunStreamPool_38K::size < uNewSize && uNewSize <= YunStreamPool_40K::size )
        {
            uNewSize = YunStreamPool_40K::size;
            pNewBuf = YunStreamPool_40K::malloc();
        }
        else if ( YunStreamPool_40K::size < uNewSize && uNewSize <= YunStreamPool_42K::size )
        {
            uNewSize = YunStreamPool_42K::size;
            pNewBuf = YunStreamPool_42K::malloc();
        }
        else if ( YunStreamPool_42K::size < uNewSize && uNewSize <= YunStreamPool_44K::size )
        {
            uNewSize = YunStreamPool_44K::size;
            pNewBuf = YunStreamPool_44K::malloc();
        }
        else if ( YunStreamPool_44K::size < uNewSize && uNewSize <= YunStreamPool_46K::size )
        {
            uNewSize = YunStreamPool_46K::size;
            pNewBuf = YunStreamPool_46K::malloc();
        }
        else if ( YunStreamPool_46K::size < uNewSize && uNewSize <= YunStreamPool_48K::size )
        {
            uNewSize = YunStreamPool_48K::size;
            pNewBuf = YunStreamPool_48K::malloc();
        }
        else if ( YunStreamPool_48K::size < uNewSize && uNewSize <= YunStreamPool_50K::size )
        {
            uNewSize = YunStreamPool_50K::size;
            pNewBuf = YunStreamPool_50K::malloc();
        }
        else if ( YunStreamPool_50K::size < uNewSize && uNewSize <= YunStreamPool_52K::size )
        {
            uNewSize = YunStreamPool_52K::size;
            pNewBuf = YunStreamPool_52K::malloc();
        }
        else if ( YunStreamPool_52K::size < uNewSize && uNewSize <= YunStreamPool_54K::size )
        {
            uNewSize = YunStreamPool_54K::size;
            pNewBuf = YunStreamPool_54K::malloc();
        }
        else if ( YunStreamPool_54K::size < uNewSize && uNewSize <= YunStreamPool_56K::size )
        {
            uNewSize = YunStreamPool_56K::size;
            pNewBuf = YunStreamPool_56K::malloc();
        }
        else if ( YunStreamPool_56K::size < uNewSize && uNewSize <= YunStreamPool_58K::size )
        {
            uNewSize = YunStreamPool_58K::size;
            pNewBuf = YunStreamPool_58K::malloc();
        }
        else if ( YunStreamPool_58K::size < uNewSize && uNewSize <= YunStreamPool_60K::size )
        {
            uNewSize = YunStreamPool_60K::size;
            pNewBuf = YunStreamPool_60K::malloc();
        }
        else if ( YunStreamPool_60K::size < uNewSize && uNewSize <= YunStreamPool_62K::size )
        {
            uNewSize = YunStreamPool_62K::size;
            pNewBuf = YunStreamPool_62K::malloc();
        }
        else if ( YunStreamPool_62K::size < uNewSize && uNewSize <= YunStreamPool_64K::size )
        {
            uNewSize = YunStreamPool_64K::size;
            pNewBuf = YunStreamPool_64K::malloc();
        }
        else
        {
            trace_all(_T("YunStreamPoolReallocCallback error pkg size > 64k"));
            assert(false);
            return;
        }

        if (pBuffer)
        {
            // 拷贝
            memcpy( pNewBuf, pBuffer, uSrcSize );

            //-----------------------------------------------
            //  释放.
            YunStreamPoolDeallocCallback( pBuffer, uSrcSize );
        }

        pBuffer = pNewBuf;
    }

    //-----------------------------------------------
    void    YunStreamDeallocCallback( YunStream* pObj )
    {
        YunStreamPool::free(pObj);
    }


    //----------------------------------------------------------------------------------------------------
    //!	YunStreamPool
    //----------------------------------------------------------------------------------------------------

    //-----------------------------------------------
    hyc::u32 YunStreamPool::getMaxSize()
    {
        return YunStreamPool_64K::size;
    }

    //-----------------------------------------------
    void YunStreamPool::usePoolBuffer( YunStream* p )
    {
        p->setExternalBuffer(NULL, 0, YunStreamPoolDeallocCallback, YunStreamPoolReallocCallback);
    }

    //-----------------------------------------------
    YunStream* YunStreamPool::malloc( YunStream::DeallocCallback deallocCB )
    {
        void* p = Singleton_YunStreamPool::malloc();
        if (!p)
        {
            release_memory();
            trace(_T("YunStreamPool 整理了一次内存"));
            p = Singleton_YunStreamPool::malloc();
        }

        if ( p )
        {
#ifdef YUN_TRACE_POOL
        static s32 g_i = 0;
        g_i++;
#endif // YUN_TRACE_POOL
            //YunStream* pp = (YunStream*)p;
            //pp->YunStream::YunStream();
        	YunStream* pp = new (p)(YunStream)();

            pp->setExternalBuffer(NULL, 0, YunStreamPoolDeallocCallback, YunStreamPoolReallocCallback);
            pp->setExternalDealloc(deallocCB ? deallocCB : YunStreamDeallocCallback);

            return pp;
        }

        trace_all(_T("YunStreamPool::malloc 内存不足."));
        exit(0);

        return NULL;
    }

    //-----------------------------------------------
    void YunStreamPool::free( YunStream* p )
    {
        if (p)
        {
#ifdef YUN_TRACE_POOL
            static s32 g_i = 0;
            g_i++;
#endif // YUN_TRACE_POOL

            p->YunStream::~YunStream();
            Singleton_YunStreamPool::free(p);
        }
    }

    //-----------------------------------------------
    void YunStreamPool::purge_memory()
    {
        YunStreamPool_16::purge_memory();
        YunStreamPool_32::purge_memory();
        YunStreamPool_64::purge_memory();
        YunStreamPool_128::purge_memory();
        YunStreamPool_256::purge_memory();
        YunStreamPool_512::purge_memory();
        YunStreamPool_1K::purge_memory();
        YunStreamPool_2K::purge_memory();
        YunStreamPool_4K::purge_memory();
        YunStreamPool_6K::purge_memory();
        YunStreamPool_8K::purge_memory();
        YunStreamPool_10K::purge_memory();
        YunStreamPool_12K::purge_memory();
        YunStreamPool_14K::purge_memory();
        YunStreamPool_16K::purge_memory();
        YunStreamPool_18K::purge_memory();
        YunStreamPool_20K::purge_memory();
        YunStreamPool_22K::purge_memory();
        YunStreamPool_24K::purge_memory();
        YunStreamPool_26K::purge_memory();
        YunStreamPool_28K::purge_memory();
        YunStreamPool_30K::purge_memory();
        YunStreamPool_32K::purge_memory();
        YunStreamPool_34K::purge_memory();
        YunStreamPool_36K::purge_memory();
        YunStreamPool_38K::purge_memory();
        YunStreamPool_40K::purge_memory();
        YunStreamPool_42K::purge_memory();
        YunStreamPool_44K::purge_memory();
        YunStreamPool_46K::purge_memory();
        YunStreamPool_48K::purge_memory();
        YunStreamPool_50K::purge_memory();
        YunStreamPool_52K::purge_memory();
        YunStreamPool_54K::purge_memory();
        YunStreamPool_56K::purge_memory();
        YunStreamPool_58K::purge_memory();
        YunStreamPool_60K::purge_memory();
        YunStreamPool_62K::purge_memory();
        YunStreamPool_64K::purge_memory();

        Singleton_YunStreamPool::purge_memory();
    }

    //-----------------------------------------------
    void YunStreamPool::release_memory()
    {
        YunStreamPool_16::release_memory();
        YunStreamPool_32::release_memory();
        YunStreamPool_64::release_memory();
        YunStreamPool_128::release_memory();
        YunStreamPool_256::release_memory();
        YunStreamPool_512::release_memory();
        YunStreamPool_1K::release_memory();
        YunStreamPool_2K::release_memory();
        YunStreamPool_4K::release_memory();
        YunStreamPool_6K::release_memory();
        YunStreamPool_8K::release_memory();
        YunStreamPool_10K::release_memory();
        YunStreamPool_12K::release_memory();
        YunStreamPool_14K::release_memory();
        YunStreamPool_16K::release_memory();
        YunStreamPool_18K::release_memory();
        YunStreamPool_20K::release_memory();
        YunStreamPool_22K::release_memory();
        YunStreamPool_24K::release_memory();
        YunStreamPool_26K::release_memory();
        YunStreamPool_28K::release_memory();
        YunStreamPool_30K::release_memory();
        YunStreamPool_32K::release_memory();
        YunStreamPool_34K::release_memory();
        YunStreamPool_36K::release_memory();
        YunStreamPool_38K::release_memory();
        YunStreamPool_40K::release_memory();
        YunStreamPool_42K::release_memory();
        YunStreamPool_44K::release_memory();
        YunStreamPool_46K::release_memory();
        YunStreamPool_48K::release_memory();
        YunStreamPool_50K::release_memory();
        YunStreamPool_52K::release_memory();
        YunStreamPool_54K::release_memory();
        YunStreamPool_56K::release_memory();
        YunStreamPool_58K::release_memory();
        YunStreamPool_60K::release_memory();
        YunStreamPool_62K::release_memory();
        YunStreamPool_64K::release_memory();

        Singleton_YunStreamPool::release_memory();
    }

#ifdef YUN_TRACE_POOL
#define TRACE_INFO(c) if (0 != c::numFree+c::numMalloc+c::numTotal) buf += c::traceInfo();
    //-----------------------------------------------
    hyc::String YunStreamPool::traceInfo()
    {
        String buf;
        
        TRACE_INFO(YunStreamPool_16);
        TRACE_INFO(YunStreamPool_32);
        TRACE_INFO(YunStreamPool_64);
        TRACE_INFO(YunStreamPool_128);
        TRACE_INFO(YunStreamPool_256);
        TRACE_INFO(YunStreamPool_512);
        TRACE_INFO(YunStreamPool_1K);
        TRACE_INFO(YunStreamPool_2K);
        TRACE_INFO(YunStreamPool_4K);
        TRACE_INFO(YunStreamPool_6K);
        TRACE_INFO(YunStreamPool_8K);
        TRACE_INFO(YunStreamPool_10K);
        TRACE_INFO(YunStreamPool_12K);
        TRACE_INFO(YunStreamPool_14K);
        TRACE_INFO(YunStreamPool_16K);
        TRACE_INFO(YunStreamPool_18K);
        TRACE_INFO(YunStreamPool_20K);
        TRACE_INFO(YunStreamPool_22K);
        TRACE_INFO(YunStreamPool_24K);
        TRACE_INFO(YunStreamPool_26K);
        TRACE_INFO(YunStreamPool_28K);
        TRACE_INFO(YunStreamPool_30K);
        TRACE_INFO(YunStreamPool_32K);
        TRACE_INFO(YunStreamPool_34K);
        TRACE_INFO(YunStreamPool_36K);
        TRACE_INFO(YunStreamPool_38K);
        TRACE_INFO(YunStreamPool_40K);
        TRACE_INFO(YunStreamPool_42K);
        TRACE_INFO(YunStreamPool_44K);
        TRACE_INFO(YunStreamPool_46K);
        TRACE_INFO(YunStreamPool_48K);
        TRACE_INFO(YunStreamPool_50K);
        TRACE_INFO(YunStreamPool_52K);
        TRACE_INFO(YunStreamPool_54K);
        TRACE_INFO(YunStreamPool_56K);
        TRACE_INFO(YunStreamPool_58K);
        TRACE_INFO(YunStreamPool_60K);
        TRACE_INFO(YunStreamPool_62K);
        TRACE_INFO(YunStreamPool_64K);

        return buf;
    }
#endif // YUN_TRACE_POOL

} // end namespace hyyun

