
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy yun network framework
// author:       lipengxiang
// create at:    2014-04-03 17:55:33
// description:  
//

#ifndef __BP_POOLTEMPLATE_HPP_20140403175533__
#define __BP_POOLTEMPLATE_HPP_20140403175533__

// boost pool
#include <boost/pool/poolfwd.hpp>
#include <boost/pool/singleton_pool.hpp>

#include "../preheader.h"

namespace hyyun
{
    template<class T>
    class ObjectPool 
    {
        typedef boost::singleton_pool<T, sizeof(T)>   Singleton_pool;
    public:
        static T*          malloc();
        static void        free(T* p);

        // release unuse memory
        static void        release_memory();

        // release all memory
        static void        purge_memory();

#ifdef YUN_TRACE_POOL
        static AtomicBase<u32> s_tick;
        static AtomicBase<u32> s_tick2;
        static String  TraceInfo() {
            String buf;
            buf.format(_T("Count: all(%d) now(%d)\n"), (u32)s_tick2, (u32)s_tick);
            return buf;
        }
#else
        static String  TraceInfo() { return _T(""); }
#endif // _DEBUG
    };

#ifdef YUN_TRACE_POOL
#define OBJECT_POOL_STATIC(c) template<> AtomicBase<u32> ObjectPool<c>::s_tick=0; template<> AtomicBase<u32> ObjectPool<c>::s_tick2=0;
#else
#define OBJECT_POOL_STATIC(c)
#endif


    //-----------------------------------------------
    template<class T>
    inline void hyyun::ObjectPool<T>::purge_memory()
    {
        Singleton_pool::purge_memory();
    }
    //-----------------------------------------------
    template<class T>
    inline void hyyun::ObjectPool<T>::release_memory()
    {
        Singleton_pool::release_memory();
    }
    //-----------------------------------------------
    template<class T>
    inline void hyyun::ObjectPool<T>::free( T* p )
    {
#ifdef YUN_TRACE_POOL
        --s_tick;
#endif // _DEBUG
        if (p)
        {
            p->T::~T();
            Singleton_pool::free(p);
        }
    }

    //-----------------------------------------------
    template<class T>
    inline T* hyyun::ObjectPool<T>::malloc()
    {
#ifdef YUN_TRACE_POOL
        ++s_tick;
        ++s_tick2;
#endif // _DEBUG

        void* p = Singleton_pool::malloc();
        if (!p)
        {
            release_memory();
            trace(_T("整理了一次内存"));
            p = Singleton_pool::malloc();
        }
        if ( p )
        {
            T* pp = new (p)T;
            return pp;
        }

        trace_all(_T("ObjectPool::malloc 内存不足."));
        assert(0);

        return NULL;
    }

} // end namespace hyyun

#endif // end of define __BP_POOLTEMPLATE_HPP_20140403175533__
