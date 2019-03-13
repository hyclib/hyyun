//
//  yunStreamAnalyser.h
//  yun network libaray
//
//  Created by brainpoint on 12-9-14.
//  Copyright (c) 2012年 华耀信息科技有限公司. All rights reserved.
//

#ifndef hyc_yunStreamAnalyser_h
#define hyc_yunStreamAnalyser_h

#include "yunStream.h"

#define YUNSTREAM_ANALYSER_CHECK(typ, value)   if (arrCol.size() <= index || typ != arrCol[index].type){ v = value; return false; }

namespace hyyun
{
    class YunStreamAnalyser;
    typedef ObjectPtr<YunStreamAnalyser> YunStreamAnalyserPtr;
    
    //----------------------------------------------------------------------------------------------------
    //* the analyser of yun stream;
    //* use to implement user define net protocol.
    //-----------------------------------------------
    //* !!!Important!!!:
    //*     有效的流是从 beginPos~endPos 结束.
    //*     YunStream::ItemArray 是columns 的信息
    //*         Item.Type  表示的是此列的类型
    //*         Item.Byte* 表示的是此列除Type外的数据地址.
    //----------------------------------------------------------------------------------------------------
    class HYYUN_API YunStreamAnalyser : public RefCount
    {
    protected:
        void*   getUserData( const YunStream& ys )              { return ys.analyserUserdata; }
        void    setUserData( const YunStream& ys, void* data )  { const_cast<YunStream&>(ys).analyserUserdata = data; }

    public:
        typedef YunStream::ItemArray    ItemArray;
    public:
        virtual ~YunStreamAnalyser()   {}
        
        //
        //! 合法性验证.
        //! 指示流 [beginPos, endPos) 是否是合法的yunstream
        //! \param surplusSize: 见YunStream::analyseSelf.
        //
        virtual bool    isLegalYun_b( 
                                     const YunStream& ys, 
                                     OUT s32& surplusSize,
                                     const Byte* beginPos, const Byte* endPos ) const = 0;
        
        //
        //! 获得以beginPos开头的流的版本号.
        //! \param beginPos:    指向ys中的buf
        //
        virtual u8      version_b( const YunStream& ys, const Byte* beginPos ) const = 0;
        
        //
        //! 解析流.
        //! 参见 YunStream::analyseSelf_b
        //
        virtual bool    analyseSelf_b(
                                      const YunStream& ys, 
                                      OUT ItemArray& arrCol,
                                      OUT s32& surplusSize, 
                                      const Byte* beginPos, const Byte* endPos ) = 0;

        //
        //! 这两个函数用于数据包头的封装.
        //! 在addColBegin被调用之前,流的内容会先被清空.
        //! \param crc32Check   是否使用crc32来验证数据
        //! \param contentSizeIn255 内容的长度是否在 255 bytes内
        //
        virtual void    addColBegin( OUT YunStream& ys, OUT ItemArray& arrCol, bool crc32Check, bool contentSizeIn255 ) = 0;
        virtual void    addColEnd( OUT YunStream& ys, OUT ItemArray& arrCol ) = 0;

        //
        //! 取列中的数值. 其他类型的数据将使用arrCol中的指针位置来取值.
        //! 默认的实现: 先加入u16代表长度,再加入数据. string末位的0也写入.
        //
        virtual bool getCol( const YunStream& ys, const ItemArray& arrCol, u32 index, OUT bool& v ) const;
        virtual bool getCol( const YunStream& ys, const ItemArray& arrCol, u32 index, OUT u8& v ) const;
        virtual bool getCol( const YunStream& ys, const ItemArray& arrCol, u32 index, OUT u16& v ) const;
        virtual bool getCol( const YunStream& ys, const ItemArray& arrCol, u32 index, OUT u32& v ) const;
        virtual bool getCol( const YunStream& ys, const ItemArray& arrCol, u32 index, OUT u64& v ) const;
        virtual bool getCol( const YunStream& ys, const ItemArray& arrCol, u32 index, OUT s8& v ) const;
        virtual bool getCol( const YunStream& ys, const ItemArray& arrCol, u32 index, OUT s16& v ) const;
        virtual bool getCol( const YunStream& ys, const ItemArray& arrCol, u32 index, OUT s32& v ) const;
        virtual bool getCol( const YunStream& ys, const ItemArray& arrCol, u32 index, OUT s64& v ) const;
        virtual bool getCol( const YunStream& ys, const ItemArray& arrCol, u32 index, OUT f32& v ) const;
        virtual bool getCol( const YunStream& ys, const ItemArray& arrCol, u32 index, OUT f64& v ) const;
        virtual bool getCol( const YunStream& ys, const ItemArray& arrCol, u32 index, OUT String& v ) const;
        virtual bool getCol_stream_info( const YunStream& ys, const ItemArray& arrCol, u32 index, OUT Byte*& pData, OUT u16& len ) const; // 仅读取ByteStream的信息.
       
        //
        //! 添加数据.
        //
        virtual bool addCol( OUT YunStream& ys, OUT ItemArray& arrCol, const bool& v );
        virtual bool addCol( OUT YunStream& ys, OUT ItemArray& arrCol, const u8& v );
        virtual bool addCol( OUT YunStream& ys, OUT ItemArray& arrCol, const u16& v );
        virtual bool addCol( OUT YunStream& ys, OUT ItemArray& arrCol, const u32& v );
        virtual bool addCol( OUT YunStream& ys, OUT ItemArray& arrCol, const u64& v );
        virtual bool addCol( OUT YunStream& ys, OUT ItemArray& arrCol, const s8& v );
        virtual bool addCol( OUT YunStream& ys, OUT ItemArray& arrCol, const s16& v );
        virtual bool addCol( OUT YunStream& ys, OUT ItemArray& arrCol, const s32& v );
        virtual bool addCol( OUT YunStream& ys, OUT ItemArray& arrCol, const s64& v );
        virtual bool addCol( OUT YunStream& ys, OUT ItemArray& arrCol, const f32& v );
        virtual bool addCol( OUT YunStream& ys, OUT ItemArray& arrCol, const f64& v );
        virtual bool addCol( OUT YunStream& ys, OUT ItemArray& arrCol, const Byte* pData, u32 length );    // 类型为 ByteStream
        virtual bool addCol( OUT YunStream& ys, OUT ItemArray& arrCol, const tchar* v, const u32& vsize );
    };
    

    //-----------------------------------------------
    inline bool YunStreamAnalyser::getCol_stream_info( const YunStream& ys, const YunStream::ItemArray& arrCol, u32 index, OUT Byte*& pData, OUT u16& v ) const
    {
        YUNSTREAM_ANALYSER_CHECK(YunStream::Type_ByteStream, 0);
        memcpy(&v, arrCol[index].data, sizeof(u16));
        v = ntohs(v);

        //assert(len > 0);
        if (0 == v)
            return true;

        pData = arrCol[index].data+sizeof(u16);

        if (pData+v > ys.end())
            return false;

        return true;
    }

    //-----------------------------------------------
    inline bool YunStreamAnalyser::getCol( const YunStream& ys, const YunStream::ItemArray& arrCol, u32 index, OUT String& v ) const
    {
        v.clear();
        YUNSTREAM_ANALYSER_CHECK(YunStream::Type_String, _T(""));

        u32 offpos = (u32)(arrCol[index].data-ys.begin());
        return YunStreamHelper::get(ys, ys.size(), offpos, v);
    }

    //-----------------------------------------------
    inline bool YunStreamAnalyser::getCol( const YunStream& ys, const YunStream::ItemArray& arrCol, u32 index, OUT bool& v ) const
    {
        YUNSTREAM_ANALYSER_CHECK(YunStream::Type_bool, false);
        memcpy(&v, arrCol[index].data, sizeof(bool));
        return true;
    }
    //-----------------------------------------------
    inline bool YunStreamAnalyser::getCol( const YunStream& ys, const YunStream::ItemArray& arrCol, u32 index, OUT u8& v ) const
    {
        YUNSTREAM_ANALYSER_CHECK(YunStream::Type_u8, 0);
        memcpy(&v, arrCol[index].data, sizeof(u8));
        return true;
    }
    //-----------------------------------------------
    inline bool YunStreamAnalyser::getCol( const YunStream& ys, const YunStream::ItemArray& arrCol, u32 index, OUT u16& v ) const
    {
        YUNSTREAM_ANALYSER_CHECK(YunStream::Type_u16, 0);
        memcpy(&v, arrCol[index].data, sizeof(u16));
        v = ntohs(v);
        return true;
    }
    //-----------------------------------------------
    inline bool YunStreamAnalyser::getCol( const YunStream& ys, const YunStream::ItemArray& arrCol, u32 index, OUT u32& v ) const
    {
        YUNSTREAM_ANALYSER_CHECK(YunStream::Type_u32, 0);
        memcpy(&v, arrCol[index].data, sizeof(u32));
        v = ntohl(v);
        return true;
    }
    //-----------------------------------------------
    inline bool YunStreamAnalyser::getCol( const YunStream& ys, const YunStream::ItemArray& arrCol, u32 index, OUT u64& v ) const
    {
        YUNSTREAM_ANALYSER_CHECK(YunStream::Type_u64, 0);
        memcpy(&v, arrCol[index].data, sizeof(u64));
        v = ByteUtils::Ntoh(v);
        return true;
    }
    //-----------------------------------------------
    inline bool YunStreamAnalyser::getCol( const YunStream& ys, const YunStream::ItemArray& arrCol, u32 index, OUT s8& v ) const
    {
        YUNSTREAM_ANALYSER_CHECK(YunStream::Type_s8, 0);
        memcpy(&v, arrCol[index].data, sizeof(s8));
        return true;
    }
    //-----------------------------------------------
    inline bool YunStreamAnalyser::getCol( const YunStream& ys, const YunStream::ItemArray& arrCol, u32 index, OUT s16& v ) const
    {
        YUNSTREAM_ANALYSER_CHECK(YunStream::Type_s16, 0);
        memcpy(&v, arrCol[index].data, sizeof(s16));
        v = ByteUtils::Ntoh((u16)v);
        return true;
    }
    //-----------------------------------------------
    inline bool YunStreamAnalyser::getCol( const YunStream& ys, const YunStream::ItemArray& arrCol, u32 index, OUT s32& v ) const
    {
        YUNSTREAM_ANALYSER_CHECK(YunStream::Type_s32, 0);
        memcpy(&v, arrCol[index].data, sizeof(s32));
        v = ByteUtils::Ntoh((u32)v);
        return true;
    }
    //-----------------------------------------------
    inline bool YunStreamAnalyser::getCol( const YunStream& ys, const YunStream::ItemArray& arrCol, u32 index, OUT s64& v ) const
    {
        YUNSTREAM_ANALYSER_CHECK(YunStream::Type_s64, 0);
        memcpy(&v, arrCol[index].data, sizeof(s64));
        v = ByteUtils::Ntoh((u64)v);
        return true;
    }
    //-----------------------------------------------
    inline bool YunStreamAnalyser::getCol( const YunStream& ys, const YunStream::ItemArray& arrCol, u32 index, OUT f32& v ) const
    {
        YUNSTREAM_ANALYSER_CHECK(YunStream::Type_f32, 0);
        memcpy(&v, arrCol[index].data, sizeof(f32));
        v = ByteUtils::Ntoh(v);
        return true;
    }
    //-----------------------------------------------
    inline bool YunStreamAnalyser::getCol( const YunStream& ys, const YunStream::ItemArray& arrCol, u32 index, OUT f64& v ) const
    {
        YUNSTREAM_ANALYSER_CHECK(YunStream::Type_f64, 0);
        memcpy(&v, arrCol[index].data, sizeof(f64));
        v = ByteUtils::Ntoh(v);
        return true;
    }

    //-----------------------------------------------
    inline bool YunStreamAnalyser::addCol( OUT YunStream& ys, OUT YunStream::ItemArray& arrCol, const bool& v )
    {
        // type
        ys.add( &YunStream::_type_bool, sizeof(Byte) );
        arrCol.push_back( YunStream::Item(YunStream::Type_bool, ys.end()) );
        // v
        ys.add( &v, sizeof(v) );

        return true;
    }
    
    //-----------------------------------------------
    inline bool YunStreamAnalyser::addCol( OUT YunStream& ys, OUT YunStream::ItemArray& arrCol, const u8& v )
    {
        // type
        ys.add( &YunStream::_type_u8, sizeof(Byte) );
        arrCol.push_back( YunStream::Item(YunStream::Type_u8, ys.end()) );
        // v
        ys.add( &v, sizeof(v) );
        
        return true;
    }
    
    //-----------------------------------------------
    inline bool YunStreamAnalyser::addCol( OUT YunStream& ys, OUT YunStream::ItemArray& arrCol, const u16& v )
    {
        // type
        ys.add( &YunStream::_type_u16, sizeof(Byte) );
        arrCol.push_back( YunStream::Item(YunStream::Type_u16, ys.end()) );
        // v
        u16 vv = ByteUtils::Hton(v);
        ys.add( &vv, sizeof(vv) );
        
        return true;
    }
    
    //-----------------------------------------------
    inline bool YunStreamAnalyser::addCol( OUT YunStream& ys, OUT YunStream::ItemArray& arrCol, const u32& v )
    {
        // type
        ys.add( &YunStream::_type_u32, sizeof(Byte) );
        arrCol.push_back( YunStream::Item(YunStream::Type_u32, ys.end()) );
        // v
        u32 vv = ByteUtils::Hton(v);
        ys.add( &vv, sizeof(vv) );
        
        return true;
    }
    
    //-----------------------------------------------
    inline bool YunStreamAnalyser::addCol( OUT YunStream& ys, OUT YunStream::ItemArray& arrCol, const u64& v )
    {
        // type
        ys.add( &YunStream::_type_u64, sizeof(Byte) );
        arrCol.push_back( YunStream::Item(YunStream::Type_u64, ys.end()) );
        // v
        u64 vv = ByteUtils::Hton(v);
        ys.add( &vv, sizeof(vv) );
        
        return true;    
    }
    
    //-----------------------------------------------
    inline bool YunStreamAnalyser::addCol( OUT YunStream& ys, OUT YunStream::ItemArray& arrCol, const s8& v )
    {
        // type
        ys.add( &YunStream::_type_s8, sizeof(Byte) );
        arrCol.push_back( YunStream::Item(YunStream::Type_s8, ys.end()) );
        // v
        ys.add( &v, sizeof(v) );
        
        return true;    
    }
    
    //-----------------------------------------------
    inline bool YunStreamAnalyser::addCol( OUT YunStream& ys, OUT YunStream::ItemArray& arrCol, const s16& v )
    {
        // type
        ys.add( &YunStream::_type_s16, sizeof(Byte) );
        arrCol.push_back( YunStream::Item(YunStream::Type_s16, ys.end()) );
        // v
        s16 vv = ByteUtils::Hton((u16)v);
        ys.add( &vv, sizeof(vv) );
        
        return true;    
    }
    
    //-----------------------------------------------
    inline bool YunStreamAnalyser::addCol( OUT YunStream& ys, OUT YunStream::ItemArray& arrCol, const s32& v )
    {
        // type
        ys.add( &YunStream::_type_s32, sizeof(Byte) );
        arrCol.push_back( YunStream::Item(YunStream::Type_s32, ys.end()) );
        // v
        s32 vv = ByteUtils::Hton((u32)v);
        ys.add( &vv, sizeof(vv) );
        
        return true;    
    }
    
    //-----------------------------------------------
    inline bool YunStreamAnalyser::addCol( OUT YunStream& ys, OUT YunStream::ItemArray& arrCol, const s64& v )
    {
        // type
        ys.add( &YunStream::_type_s64, sizeof(Byte) );
        arrCol.push_back( YunStream::Item(YunStream::Type_s64, ys.end()) );
        // v
        s64 vv = ByteUtils::Hton((u64)v);
        ys.add( &vv, sizeof(vv) );
        
        return true;    
    }
    
    //-----------------------------------------------
    inline bool YunStreamAnalyser::addCol( OUT YunStream& ys, OUT YunStream::ItemArray& arrCol, const f32& v )
    {
        // type
        ys.add( &YunStream::_type_f32, sizeof(Byte) );
        arrCol.push_back( YunStream::Item(YunStream::Type_f32, ys.end()) );
        // v
        f32 vv = ByteUtils::Hton(v);
        ys.add( &vv, sizeof(vv) );
        
        return true;
    }
    
    //-----------------------------------------------
    inline bool YunStreamAnalyser::addCol( OUT YunStream& ys, OUT YunStream::ItemArray& arrCol, const f64& v )
    {
        // type
        ys.add( &YunStream::_type_f64, sizeof(Byte) );
        arrCol.push_back( YunStream::Item(YunStream::Type_f64, ys.end()) );
        // v
        f64 vv = ByteUtils::Hton(v);
        ys.add( &vv, sizeof(vv) );
        
        return true;    
    }
    
    //-----------------------------------------------
    inline bool YunStreamAnalyser::addCol( OUT YunStream& ys, OUT YunStream::ItemArray& arrCol, const Byte* pData, u32 length )
    {
        // type
        ys.add( &YunStream::_type_byteStream, sizeof(Byte) );
        arrCol.push_back( YunStream::Item(YunStream::Type_ByteStream, ys.end()) );

        return YunStreamHelper::set(ys, pData, length);
    }

    //-----------------------------------------------
    inline bool YunStreamAnalyser::addCol( OUT YunStream& ys, OUT YunStream::ItemArray& arrCol, const tchar* v, const u32& vsize )
    {
        // type
        ys.add( &YunStream::_type_string, sizeof(Byte) );
        arrCol.push_back( YunStream::Item(YunStream::Type_String, ys.end()) );

        return YunStreamHelper::set(ys, v, vsize);
    }

        
} // end namespace hyyun

#endif
