//
//  yunStream.cpp
//  yun network libaray
//
//  Created by brainpoint on 12-9-14.
//  Copyright (c) 2012年 华耀信息科技有限公司. All rights reserved.
//

#include "preheader.h"

#include "../../include/utils/yunStream.h"
#include "yunStreamAnalyser0.hpp"

#if CN_PLATFORM == CN_PLATFORM_WINDOWS
#pragma warning(disable: 4018)
#endif

#define YUN_CUR_VERSION 0

namespace hyyun
{

    const Byte YunStream::_type_bool = (Byte)YunStream::Type_bool;
    const Byte YunStream::_type_u8 = (Byte)YunStream::Type_u8;
    const Byte YunStream::_type_u16 = (Byte)YunStream::Type_u16;
    const Byte YunStream::_type_u32 = (Byte)YunStream::Type_u32;
    const Byte YunStream::_type_u64 = (Byte)YunStream::Type_u64;
    const Byte YunStream::_type_s8 = (Byte)YunStream::Type_s8;
    const Byte YunStream::_type_s16 = (Byte)YunStream::Type_s16;
    const Byte YunStream::_type_s32 = (Byte)YunStream::Type_s32;
    const Byte YunStream::_type_s64 = (Byte)YunStream::Type_s64;
    const Byte YunStream::_type_f32 = (Byte)YunStream::Type_f32;
    const Byte YunStream::_type_f64 = (Byte)YunStream::Type_f64;
    const Byte YunStream::_type_byteStream = (Byte)YunStream::Type_ByteStream;
    const Byte YunStream::_type_string = (Byte)YunStream::Type_String;
    
    //----------------------------------------------------------------------------------------------------
    //*	YunStream
    //----------------------------------------------------------------------------------------------------
    
    //-----------------------------------------------
    YunStream::YunStream()
        : ByteStream()
    	, analyserUserdata(NULL)
        , m_beginAdd(false)
        , m_headerSize(0)
        , m_pAnalyser(NULL)
        , m_deallocCallback(NULL)
    {
        setAnalyser(NULL);
    }

    //-----------------------------------------------
    YunStream::YunStream( Byte* buffer, u32 size )
        : ByteStream(buffer, size)
    	, analyserUserdata(NULL)
        , m_beginAdd(false)
        , m_headerSize(0)
        , m_pAnalyser(NULL)
        , m_deallocCallback(NULL)
    {
        setAnalyser(NULL);
    }

    //-----------------------------------------------
    YunStream::~YunStream()
    {
        //assert(!m_beginAdd);

        //_TRACE_LOG_DEBUG(_T("error addColEnd"));
        if (_externalBuffer.m_useExternalBuffer && _externalBuffer.m_cnDealloc)
        {
            _externalBuffer.m_cnDealloc(_externalBuffer.m_buffer, _externalBuffer.m_capacity);
        }
    }

    //-----------------------------------------------
    void YunStream::onDestroy()
    {
        if (m_deallocCallback)
            m_deallocCallback(this);
        else
            RefCountSafe::onDestroy();
    }

    //-----------------------------------------------
    void YunStream::setAnalyser( YunStreamAnalyser* pAnalyser )
    {
        m_pAnalyser = pAnalyser;
        if (!m_pAnalyser)
            m_pAnalyser = g_yunStreamAnalyser_0.get();
    }

    //-----------------------------------------------
    void YunStream::setExternalBuffer( void* pBuffer, u32 uSize, MemoryDeallocCallback cbDealloc, MemoryReallocCallback cbRealloc )
    {
        if (_extBuffer) {
            error_all(_T("external buffer can't be Externalled"));
            return;
        }

        assert(cbDealloc && cbRealloc);
        if (_externalBuffer.m_useExternalBuffer && _externalBuffer.m_cnDealloc)
        {
            _externalBuffer.m_cnDealloc(_externalBuffer.m_buffer, _externalBuffer.m_capacity);
        }
        
        _externalBuffer.m_useExternalBuffer = true;
        _externalBuffer.m_buffer = (Byte*)pBuffer;
        _externalBuffer.m_size = 0;
        _externalBuffer.m_capacity = (NULL==pBuffer) ? 0 : uSize;
        _externalBuffer.m_cnDealloc = cbDealloc;
        _externalBuffer.m_cnRealloc = cbRealloc;
        
        _curPos = 0;
    }
    
    //-----------------------------------------------
    bool YunStream::isLegalYun_b( const Byte* beginPos, const Byte* endPos, OUT s32& surplusSize ) const
    {
        assert(m_pAnalyser);
        return m_pAnalyser->isLegalYun_b(*this, surplusSize, beginPos, endPos);
    }
    
    //-----------------------------------------------
    hyc::u32 YunStream::getColNum() const
    {
        return (u32)m_types.size();
    }
    
    //-----------------------------------------------
    const YunStream::Type& YunStream::getColType( u32 index ) const
    {
        return m_types[index].type;
    }
    

    //----------------------------------------
    bool YunStream::getColValueArrString( u32 index, OUT ArrayFind<String>& v ) const
    {
        v.clear();

        if (m_types.size() <= index || YunStream::Type_ByteStream != m_types[index].type)
        {
            return false;
        }
        else
        {
            u16 len;
            memcpy(&len, m_types[index].data, sizeof(u16));
            len = ByteUtils::Ntoh(len);

            //assert(len > 0);
            if (2 >= len)
                return true;

            u32 endpos = (u32)(m_types[index].data-begin()+2+len);
            u32 offpos = (u32)(m_types[index].data-begin()+2);
            return YunStreamHelper::get(*this, endpos, offpos, v);
        }
    }
    
    //-----------------------------------------------
    void YunStream::addColBegin( bool crc32Check, bool contentSizeIn255 )
    {
        assert(!m_beginAdd);
        clear();
        assert(m_pAnalyser);
        m_pAnalyser->addColBegin(*this, m_types, crc32Check, contentSizeIn255);
        m_beginAdd = true;
    }
    
    //-----------------------------------------------
    void YunStream::addColEnd()
    {
        assert( m_beginAdd );
        assert (size() >= m_headerSize);
        
        assert(m_pAnalyser);
        m_pAnalyser->addColEnd(*this, m_types);
        _curPos = end();
        m_beginAdd = false;
    }
    
    //-----------------------------------------------
    bool YunStream::analyseSelf_b( OUT s32& surplusSize, Byte* beginPos )
    {
        assert(beginPos >= begin());
        
        surplusSize = 0;
        if (_curPos == NULL || _curPos == beginPos)
        {
            return false;
        }
        m_beginAdd = false;
        
        assert(m_pAnalyser);
        return m_pAnalyser->analyseSelf_b(*this, m_types, surplusSize, beginPos, _curPos);
    }
    
    //-----------------------------------------------
    hyc::u8 YunStream::version_b( const Byte* beginPos ) const
    {
        assert(m_pAnalyser);
        return m_pAnalyser->version_b(*this, beginPos);
    }
    
    //----------------------------------------
    bool YunStream::addColArrString( const ArrayFind<String>& v )
    {
        if (!m_beginAdd
            || size()+3+2 > YUN_CONTENT_MAX_LENGTH )
            return false;

        add( &YunStream::_type_byteStream, sizeof(Byte) );
        m_types.push_back( Item(YunStream::Type_ByteStream, end()) );

        u32 bufSizePos = size();

        u16 len = 0;    add( &len, sizeof(u16) );
        u16 bufSize = size();

        ByteStream& buf = *this;
        if (!YunStreamHelper::set(buf, v))
            return false;

        bufSize = size()-bufSize;
        bufSize = ByteUtils::Hton(bufSize);
        memcpy(begin()+bufSizePos, &bufSize, sizeof(u16));

        return true;
    }


    //-----------------------------------------------
    Byte& YunStream::operator[]( u32 n )
    {
        if (_externalBuffer.m_useExternalBuffer)
        {
            return _externalBuffer.m_buffer[n];
        }
        else
        {
            return ByteStream::operator[](n);
        }
    }
    
    //-----------------------------------------------
    const Byte& YunStream::operator[]( u32 n ) const
    {
        if (_externalBuffer.m_useExternalBuffer)
        {
            return _externalBuffer.m_buffer[n];
        }
        else
        {
            return ByteStream::operator[](n);
        }
    }
    
    //-----------------------------------------------
    void YunStream::operator+=( const char* pStr )
    {
        if (pStr)
        {
            if (_externalBuffer.m_useExternalBuffer)
            {
                u32 len = (u32)StringUtils::GetLength( pStr );
                
                if (_externalBuffer.m_buffer
                    && _externalBuffer.m_size + len > _externalBuffer.m_capacity)
                {
                    reserve( len + _externalBuffer.m_size );
                }
                
                if (_externalBuffer.m_capacity - _externalBuffer.m_size >= len)
                {
                    memcpy( 
                           _externalBuffer.m_buffer+_externalBuffer.m_size,
                           pStr, 
                           len
                           );
                    _externalBuffer.m_size += len;
                    return;
                }
                
                assert(false);  // error external buf realloc
            }
            else
            {
                ByteStream::operator+=(pStr);
            }
        }
    }
    
    //-----------------------------------------------
    void YunStream::operator+=( const cString& str )
    {
        if (!str.empty())
        {
            if (_externalBuffer.m_useExternalBuffer)
            {
                u32 len = (u32)str.size();
                
                if (_externalBuffer.m_buffer
                    && _externalBuffer.m_size + len > _externalBuffer.m_capacity)
                {
                    
                    reserve( len + _externalBuffer.m_size );
                }
                
                if (_externalBuffer.m_capacity - _externalBuffer.m_size >= len)
                {
                    memcpy( 
                           _externalBuffer.m_buffer+_externalBuffer.m_size,
                           str.c_str(), 
                           len
                           );
                    _externalBuffer.m_size += len;
                    return;
                }
                
                assert(false);  // error external buf realloc
            }
            else
            {
                ByteStream::operator+=(str);
            }
        }
    }
    
    //-----------------------------------------------
    void YunStream::add( const void* pAppendBuf, u32 csize )
    {
        if (0 == csize || NULL == pAppendBuf)
            return;
        
        if (_externalBuffer.m_useExternalBuffer)
        {
            reserve(csize + _externalBuffer.m_size);
            
            if (_externalBuffer.m_capacity - _externalBuffer.m_size >= csize)
            {
                memcpy( 
                       _externalBuffer.m_buffer+_externalBuffer.m_size,
                       pAppendBuf, 
                       csize
                       );
                _externalBuffer.m_size += csize;
                return;
            }
            
            assert(false);  // error external buf realloc
        }
        else
        {
            ByteStream::add(pAppendBuf, csize);
        }
    }
    
    //-----------------------------------------------
    void YunStream::swapBuffer( ByteStream& other )
    {
        assert(false);
    }
    
    //-----------------------------------------------
    Byte* YunStream::begin()
    {
        if (_externalBuffer.m_useExternalBuffer)
        {
            return _externalBuffer.m_buffer;
        }
        else
        {
            return ByteStream::begin();
        }
    }
    
    //-----------------------------------------------
    const Byte* YunStream::begin() const
    {
        if (_externalBuffer.m_useExternalBuffer)
        {
            return _externalBuffer.m_buffer;
        }
        else
        {
            return ByteStream::begin();
        }
    }
    
    //-----------------------------------------------
    Byte* YunStream::end()
    {
        if (_externalBuffer.m_useExternalBuffer)
        {
            return _externalBuffer.m_buffer+_externalBuffer.m_size;
        }
        else
        {
            return ByteStream::end();
        }
    }
    
    //-----------------------------------------------
    const Byte* YunStream::end() const
    {
        if (_externalBuffer.m_useExternalBuffer)
        {
            return _externalBuffer.m_buffer+_externalBuffer.m_size;
        }
        else
        {
            return ByteStream::end();
        }
    }
    
    //-----------------------------------------------
    hyc::u32 YunStream::size() const
    {
        if (_externalBuffer.m_useExternalBuffer)
        {
            return _externalBuffer.m_size;
        }
        else
        {
            return ByteStream::size();
        }
    }

    //-----------------------------------------------
    hyc::u32 YunStream::capacity() const
    {
        if (_externalBuffer.m_useExternalBuffer)
        {
            return _externalBuffer.m_capacity;
        }
        else
        {
            return ByteStream::capacity();
        }
    }

    //-----------------------------------------------
    bool YunStream::empty() const
    {
        if (_externalBuffer.m_useExternalBuffer)
        {
            return 0 == _externalBuffer.m_size;
        }
        else
        {
            return ByteStream::empty();
        }
    }
    
    //-----------------------------------------------
    void YunStream::clear()
    {
        if (_externalBuffer.m_useExternalBuffer)
        {
            _externalBuffer.m_size = 0;
            _curPos = NULL;
            _position = 0;
        }
        else
        {
            ByteStream::clear();
        }
        m_types.clear();
        m_beginAdd = false;
        m_headerSize = 0;
    }
    
    //-----------------------------------------------
    void YunStream::resize( u32 n )
    {
        if (_externalBuffer.m_useExternalBuffer)
        {
            if (n > YUN_CONTENT_MAX_LENGTH) n = YUN_CONTENT_MAX_LENGTH;
            
            if(n == 0)
            {
                clear();
            }
            else if(n > _externalBuffer.m_capacity)
            {
                reserve(n);
            }
            _externalBuffer.m_size = n;
        }
        else
        {
            return ByteStream::resize(n);
        }
    }
    
    //-----------------------------------------------
    void YunStream::reserve( u32 n )
    {
        if (n > YUN_CONTENT_MAX_LENGTH) n = YUN_CONTENT_MAX_LENGTH;
        
        if (_externalBuffer.m_useExternalBuffer)
        {
            if (_externalBuffer.m_cnRealloc)
            {
                if (_externalBuffer.m_capacity < n)
                {
                    _externalBuffer.m_cnRealloc( _externalBuffer.m_buffer, _externalBuffer.m_capacity, n );
                    _externalBuffer.m_capacity = n;
                }
            }
            
            assert (_externalBuffer.m_capacity >= n);
        }
        else
        {
            return ByteStream::reserve(n);
        }
    }
    
    //-----------------------------------------------
    void YunStream::push_back( Byte v )
    {
        if (_externalBuffer.m_useExternalBuffer)
        {
            reserve( 1 + _externalBuffer.m_size );
            if (_externalBuffer.m_capacity - _externalBuffer.m_size >= 1)
            {
                memcpy( 
                       _externalBuffer.m_buffer+_externalBuffer.m_size,
                       &v, 
                       1
                       );
                _externalBuffer.m_size += 1;
                return;
            }
            assert(false);  // error external buf realloc
        }
        else
        {
            return ByteStream::push_back(v);
        }
    }

        bool YunStream::getColValue( u32 index, OUT bool& v ) const         { return m_pAnalyser->getCol(*this, m_types, index, v); }
        bool YunStream::getColValue( u32 index, OUT u8& v ) const           { return m_pAnalyser->getCol(*this, m_types, index, v); }
        bool YunStream::getColValue( u32 index, OUT u16& v ) const          { return m_pAnalyser->getCol(*this, m_types, index, v); }
        bool YunStream::getColValue( u32 index, OUT u32& v ) const          { return m_pAnalyser->getCol(*this, m_types, index, v); }
        bool YunStream::getColValue( u32 index, OUT u64& v ) const          { return m_pAnalyser->getCol(*this, m_types, index, v); }
        bool YunStream::getColValue( u32 index, OUT s8& v ) const           { return m_pAnalyser->getCol(*this, m_types, index, v); }
        bool YunStream::getColValue( u32 index, OUT s16& v ) const          { return m_pAnalyser->getCol(*this, m_types, index, v); }
        bool YunStream::getColValue( u32 index, OUT s32& v ) const          { return m_pAnalyser->getCol(*this, m_types, index, v); }
        bool YunStream::getColValue( u32 index, OUT s64& v ) const          { return m_pAnalyser->getCol(*this, m_types, index, v); }
        bool YunStream::getColValue( u32 index, OUT f32& v ) const          { return m_pAnalyser->getCol(*this, m_types, index, v); }
        bool YunStream::getColValue( u32 index, OUT f64& v ) const          { return m_pAnalyser->getCol(*this, m_types, index, v); }
        bool YunStream::getColValue( u32 index, OUT String& v ) const       { return m_pAnalyser->getCol(*this, m_types, index, v); }
        bool YunStream::getColValue_stream_info( u32 index, OUT Byte*& pData, OUT u16& len ) const { return m_pAnalyser->getCol_stream_info(*this, m_types, index, pData, len); } // 读取ByteStream的信息.
                
        bool YunStream::addCol( const bool& v )                     { return m_beginAdd && m_pAnalyser->addCol(*this, m_types, v); }
        bool YunStream::addCol( const u8& v )                       { return m_beginAdd && m_pAnalyser->addCol(*this, m_types, v); }
        bool YunStream::addCol( const u16& v )                      { return m_beginAdd && m_pAnalyser->addCol(*this, m_types, v); }
        bool YunStream::addCol( const u32& v )                      { return m_beginAdd && m_pAnalyser->addCol(*this, m_types, v); }
        bool YunStream::addCol( const u64& v )                      { return m_beginAdd && m_pAnalyser->addCol(*this, m_types, v); }
        bool YunStream::addCol( const s8& v )                       { return m_beginAdd && m_pAnalyser->addCol(*this, m_types, v); }
        bool YunStream::addCol( const s16& v )                      { return m_beginAdd && m_pAnalyser->addCol(*this, m_types, v); }
        bool YunStream::addCol( const s32& v )                      { return m_beginAdd && m_pAnalyser->addCol(*this, m_types, v); }
        bool YunStream::addCol( const s64& v )                      { return m_beginAdd && m_pAnalyser->addCol(*this, m_types, v); }
        bool YunStream::addCol( const f32& v )                      { return m_beginAdd && m_pAnalyser->addCol(*this, m_types, v); }
        bool YunStream::addCol( const f64& v )                      { return m_beginAdd && m_pAnalyser->addCol(*this, m_types, v); }
        bool YunStream::addCol( const ByteStream& v )               { return m_beginAdd && m_pAnalyser->addCol(*this, m_types, v.begin(), v.size()); }
        bool YunStream::addCol( const Byte* pData, u32 length )     { return m_beginAdd && m_pAnalyser->addCol(*this, m_types, pData, length); }
        bool YunStream::addCol( const String& v )                   { return m_beginAdd && m_pAnalyser->addCol(*this, m_types, v.c_str(), v.size()); }
        bool YunStream::addCol( const tchar* v )                    { return m_beginAdd && m_pAnalyser->addCol(*this, m_types, v, (u32)StringUtils::GetLength(v)); }
        bool YunStream::addCol( const YunStreamType* v )            { if (!m_beginAdd) return false; ByteStream bs; if (!v->serialize(bs)) return false; return addCol(bs); }

} // end namespace hyyun
