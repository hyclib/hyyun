//
//  yunStream.h
//  yun network libaray
//
//  Created by brainpoint on 12-9-14.
//  Copyright (c) 2012年 华耀信息科技有限公司. All rights reserved.
//

#ifndef hyc_yunStream_h
#define hyc_yunStream_h

namespace hyyun
{
    class YunStreamAnalyser;

    //----------------------------------------------------------------------------------------------------
    //*	yun stream; 一个yun包最大为65535-20(header of udp)
    //* 流中传输的字符串使用utf8格式.
    //----------------------------------------------------------------------------------------------------
    class HYYUN_API YunStream : public ByteStream
    {
    public:
        //! 支持的类型.
        typedef enum { Type_bool,Type_u8,Type_u16,Type_u32,Type_u64,Type_s8,Type_s16,Type_s32,Type_s64,Type_f32,Type_f64,Type_ByteStream,Type_String,
        } Type;

    public:
        YunStream();
        YunStream( Byte* buffer, u32 size );
        virtual ~YunStream();

        /**
         * @brief 智能指针析构方法 (只有使用智能指针的对象才会使用这个来析构).
         */
        virtual void onDestroy();
        
        /**
         * @brief 设置解析器, 为NULL时系统自动根据版本选择解析器, 否则使用选择的解析器.
         *        对象析构时不会释放pAnalyser.
         */
        void    setAnalyser( YunStreamAnalyser* pAnalyser );

        //-----------------------------------------------
        //  override
    public:
        virtual Byte&       operator[]( u32 n );
        virtual const Byte& operator[]( u32 n ) const;
        virtual void        operator+=( const char* pStr );
        virtual void        operator+=( const cString& str );
        
        virtual void        add( const void* pAppendBuf, u32 size );
        
        virtual void        swapBuffer( ByteStream& other );
        virtual Byte*       begin();
        virtual const Byte* begin() const;
        virtual Byte*       end();
        virtual const Byte* end() const;
        virtual u32         size() const;
        virtual u32         capacity() const;
        virtual bool        empty() const;
        virtual void        clear();
        virtual void        resize( u32 n );
        virtual void        reserve( u32 n );   // 如果使用外部流, 则此操作是阻塞的, 保证分配内存成功.
        virtual void        push_back( Byte v );
        
        //-----------------------------------------------
        //  method
    public:
        
        //! 获得数据.
        u32         getColNum() const;
        const Type& getColType( u32 index ) const;
        bool        getColValue( u32 index, OUT bool& v ) const;
        bool        getColValue( u32 index, OUT u8& v ) const;
        bool        getColValue( u32 index, OUT u16& v ) const;
        bool        getColValue( u32 index, OUT u32& v ) const;
        bool        getColValue( u32 index, OUT u64& v ) const;
        bool        getColValue( u32 index, OUT s8& v ) const;
        bool        getColValue( u32 index, OUT s16& v ) const;
        bool        getColValue( u32 index, OUT s32& v ) const;
        bool        getColValue( u32 index, OUT s64& v ) const;
        bool        getColValue( u32 index, OUT f32& v ) const;
        bool        getColValue( u32 index, OUT f64& v ) const;
        bool        getColValue( u32 index, OUT String& v ) const;
        bool        getColValue( u32 index, OUT ByteStream& v ) const;                          // 不清空v,继续添加到v中.
        bool        getColValue( u32 index, OUT Byte* pBuf, u16 bufLength ) const;              // 使用buf获取字节流, 当长度不为bufLength长度时返回false
        bool        getColValue( u32 index, OUT YunStreamType* v ) const;
        bool        getColValue_stream_info( u32 index, OUT Byte*& pData, OUT u16& len ) const; // 读取ByteStream的信息.
        
        bool        getColValueArrString( u32 index, OUT ArrayFind<String>& v ) const;  //! string数组获取.
        template<class T>
        bool        getColValueArrYunStreamType( u32 index, OUT Array<T>& v ) const;    //! 自定类型的获取.
        template<class T>
        bool        getColValueArr( u32 index, OUT ArrayFind<T>& v ) const;             //! 基础类型的数组化获取.

        /**
         * @brief 清空流, 并开始添加数据至流中; (添加前后调用addColBegin/addColEnd)
         * @param crc32Check   是否使用crc32来验证数据.
         * @param contentSizeIn255 内容的长度是否在 255 bytes内 (出于性能考虑不自动计算这个条件).
         */
        void        addColBegin( bool crc32Check = true, bool contentSizeIn255 = false );
        void        addColEnd();
        
        bool        addCol( const bool& v );
        bool        addCol( const u8& v );
        bool        addCol( const u16& v );
        bool        addCol( const u32& v );
        bool        addCol( const u64& v );
        bool        addCol( const s8& v );
        bool        addCol( const s16& v );
        bool        addCol( const s32& v );
        bool        addCol( const s64& v );
        bool        addCol( const f32& v );
        bool        addCol( const f64& v );
        bool        addCol( const ByteStream& v );
        bool        addCol( const Byte* pData, u32 length );
        bool        addCol( const String& v );
        bool        addCol( const tchar* v );
        bool        addCol( const YunStreamType* v );

        bool        addColArrString( const ArrayFind<String>& v );  //! string数组化的添加.
        template<class T>
        bool        addColArrYunStreamType( const Array<T>& v );    //! 自定类型数组化的添加.
        template<class T>
        bool        addColArr( const ArrayFind<T>& v );             //! 基础类型数组化的添加.
        
        /**
         * @brief 判断指定流 [beginPos, endPos) 是否为合法.
         * @param surplusSize
         *            见函数analyseSelf.
         * @return     返回true表明一定包含一个完整包.
         */
        bool        isLegalYun() const  { s32 s; return isLegalYun_b(begin(), end(), s); }
        bool        isLegalYun_b( const Byte* beginPos, const Byte* endPos, OUT s32& surplusSize ) const;
        
        /**
         * @brief 获得以beginPos开头的流的版本号.
         */
        u8          version() const { return version_b(begin()); }
        u8          version_b( const Byte* beginPos ) const;
        
        //! 是否对内容压缩
        //void        setCompress( bool bCompress );
        //bool        isCompressed() const;
        
        /**
         * @brief 使用指定的流来构造yunStream, 自身的内容必须可以构造一个包(内容可以大于一个包), 否则返回false
         *        解析的有效流为: [begin, _curPos)
         * @param surplusSize
         *          构造完整包后剩余的流大小; >0 表明构造完一个包后还剩余的字节数, <0 表明构造一个完整包还需要的字节数
         * @return
         *          (false && 0==surplusSize), 则表明流是非法的, 可抛弃.
         *          (false && 0> surplusSize), 则表明流不足以构造一个完整包;
         *          (true &&  0==surplusSize,  则表明流刚好为一个包.
         *          (true &&  0< surplusSize), 则表明流存在一个包,还剩余一些buf.
         */
        bool        analyseSelf( OUT s32& surplusSize )     { return analyseSelf_b(surplusSize, begin()); }
        bool        analyseSelf_b( OUT s32& surplusSize, Byte* beginPos );

        /**
         * @brief 解析器的用户数据, 用来在解析器与流间传递需要的信息.
         */
        void*       analyserUserdata;
        
    public:
        /**
         * @brief 外部析构对象本身的回调. 将在onDestroy中调用这个方法.
         */
        typedef void (*DeallocCallback)( YunStream* pObj );
        /**
         * @brief 使用外部buf的析构回调.
         */
        typedef void (*MemoryDeallocCallback)( hyc::Byte* pBuffer, u32 uSize );
        /**
         * @brief 使用外部buf的重新申请内存回调.
         * @param pBuffer
         *        外部buf的指针, 返回重新分配后的指针.
         * @param uSrcSize
         *        buf的原长度.
         * @param uNewSize
         *        期望的新长度, 返回重新分配后的长度.
         */
        typedef void (*MemoryReallocCallback)( hyc::Byte*& pBuffer, u32 uSrcSize, OUT u32& uNewSize );

        /**
         * @brief 使用外部析构对象的方法.
         */
        void    setExternalDealloc( DeallocCallback callback )  { assert(!m_deallocCallback); m_deallocCallback = callback; }

        /**
         * @brief 使用外部的buffer, 不会对buffer进行释放
         *        使用了外部的buf之后, 就必须保证添加数据不会超出csize, 否则不会自动分配内存.
         */
        void    setExternalBuffer( void* pBuffer, u32 uSize, MemoryDeallocCallback cbDealloc, MemoryReallocCallback cbRealloc );

    public:
        //! 列数值的数组.
        struct Item {
            Item()  {}
            Item(Type t, Byte* d) : type(t), data(d) {}

            Type type; Byte* data; 
        };
        typedef Array< Item > ItemArray;

        static const Byte _type_bool;
        static const Byte _type_u8;
        static const Byte _type_u16;
        static const Byte _type_u32;
        static const Byte _type_u64;
        static const Byte _type_s8;
        static const Byte _type_s16;
        static const Byte _type_s32;
        static const Byte _type_s64;
        static const Byte _type_f32;
        static const Byte _type_f64;
        static const Byte _type_byteStream;
        static const Byte _type_string;

    protected:
        ItemArray       m_types;
        bool            m_beginAdd;
        mutable u16     m_headerSize;
        YunStreamAnalyser*    m_pAnalyser;
        DeallocCallback m_deallocCallback;
        
        struct HYYUN_API ExternalBuffer {
            ExternalBuffer()
            : m_useExternalBuffer(false)
            , m_size(0)
            , m_capacity(0)
            , m_buffer(0), m_cnDealloc(0), m_cnRealloc(0)   {}
            
            bool        m_useExternalBuffer;
            u32         m_size;
            u32         m_capacity;
            Byte*       m_buffer;
            MemoryDeallocCallback  m_cnDealloc;
            MemoryReallocCallback  m_cnRealloc;
        } _externalBuffer;  // 区别于 ByteStream._extBuffer.
        
        friend class YunStreamPrivateClass;
        friend class YunStreamAnalyser;
    };

    typedef ObjectPtr<YunStream>   YunStreamPtr;


    //-----------------------------------------------
    inline bool YunStream::getColValue( u32 index, OUT ByteStream& v ) const
    {
        Byte* pByte;
        u16 len;
        if (!getColValue_stream_info(index, pByte, len))
            return false;
        else
        {
            v.reserve(v.size()+len);
            v.add(pByte, len);
            return true;
        }
    }

    //-----------------------------------------------
    inline bool YunStream::getColValue( u32 index, OUT Byte* pBuf, u16 bufLength ) const
    {
        Byte* pByte;
        u16 len;
        if (!getColValue_stream_info(index, pByte, len) || len != bufLength)
            return false;
        else
        {
            memcpy(pByte, pByte, len);
            return true;
        }
    }

    //-----------------------------------------------
    inline bool YunStream::getColValue( u32 index, OUT YunStreamType* v ) const
    {
        Byte* pByte;
        u16 len;
        if (!getColValue_stream_info(index, pByte, len))
            return false;
        else
        {
            u32 pos = (u32)(pByte-begin());
            return v->deserialize(*this, pos, len);
        }
    }

    //-----------------------------------------------
    template<class T>
    inline bool YunStream::getColValueArrYunStreamType( u32 index, OUT Array<T>& v ) const
    {
        v.clear();
        if (m_types.size() <= index || YunStream::Type_ByteStream != m_types[index].type)
        {
            return false;
        }
        else
        {
            //u16 len = *reinterpret_cast<u16*>(m_types[index].second);
            u16 len;
            memcpy(&len, m_types[index].data, sizeof(u16));
            len = ByteUtils::Ntoh(len);


            //assert(len > 0);
            if (0 == len)
                return true;

            hyc::Byte* pBuf = m_types[index].data+sizeof(u16);
            hyc::Byte* pEndBuf = pBuf + len;
            if (pEndBuf > end())    return false;

            //u16 vSize = *(u16*)pBuf; pBuf += sizeof(u16);
            u16 vSize;
            memcpy(&vSize, pBuf, sizeof(u16)); pBuf += sizeof(u16);
            vSize = ByteUtils::Ntoh(vSize);

            if (0 == vSize)
                return true;

            T t;
            u32 posSrc = 0;
            assert(dynamic_cast<YunStreamType*>(&t));
            while (pBuf < pEndBuf) {
                if ( pBuf+2 > pEndBuf )   { v.clear(); return false; }
                //len = *(u16*)pBuf;  pBuf += sizeof(u16);
                memcpy(&len, pBuf, sizeof(u16));
                len = ByteUtils::Ntoh(len);
                if ( pBuf+len > pEndBuf ){ v.clear(); return false; }

                pBuf += sizeof(u16);
                posSrc = pBuf - begin();
                if (!t.deserialize(*this, posSrc, pEndBuf-pBuf))    { v.clear(); return false; }
                pBuf -= sizeof(u16);
                pBuf += len;

                v.push_back(t);
            }

            if (vSize != v.size())
            {
                v.clear();
                return false;
            }

            return true;
        }
    }


    //-----------------------------------------------
    template<class T>
    inline bool YunStream::getColValueArr( u32 index, OUT ArrayFind<T>& v ) const
    {
        v.clear();
        if (m_types.size() <= index || YunStream::Type_ByteStream != m_types[index].type)
        {
            return false;
        }
        else
        {
            //u16 len = *reinterpret_cast<u16*>(m_types[index].second);
            u16 len;
            memcpy(&len, m_types[index].data, sizeof(u16));
            len = ByteUtils::Ntoh(len);

            //assert(len > 0);
            if (2 >= len)
                return true;

            hyc::Byte* pBuf = m_types[index].data+sizeof(u16);
            hyc::Byte* pEndBuf = pBuf + len;
            u32 endpos = (u32)(pEndBuf-begin());
            u32 offpos = (u32)(pBuf-begin());
            return YunStreamHelper::get(*this, endpos, offpos, v);
        }
    }

    //-----------------------------------------------
    template<class T>
    inline bool YunStream::addColArrYunStreamType( const Array<T>& v )
    {

            if (!m_beginAdd || size()+3+2 > YUN_CONTENT_MAX_LENGTH ) return false;
            add( &YunStream::_type_byteStream, sizeof(Byte) );
            m_types.push_back( Item(YunStream::Type_ByteStream, end()) );
            u32 bufLenPos = (u32)(end()-begin());
            u16 bufLen = 2;
            u16 len = 0;    len = ByteUtils::Hton(len); add( &len, sizeof(len) );

            if (v.size() == 0)
                return true;

            len = v.size(); len = ByteUtils::Hton(len); add( &len, sizeof(len) );

            u16 lenn;
            u32 lenPos;
            for (u32 i = 0; i < v.size(); i++) {
                assert( dynamic_cast<const YunStreamType*>(&v[i]) );
                //len = v[i].size();
                //if (size()+len+2 > YUN_CONTENT_MAX_LENGTH)   return false;
                lenn = 0;
                lenPos = size();
                add(&lenn, sizeof(u16));
                if (!v[i].serialize(*this))
                    return false;

                len = size()-lenPos;
                lenn = ByteUtils::Hton(len);
                memcpy(begin()+lenPos, &lenn, sizeof(u16));

                bufLen += len;
            }

            //*(u16*)(begin()+bufLenPos) = bufLen;
            bufLen = ByteUtils::Hton(bufLen); 
            memcpy(begin()+bufLenPos, &bufLen, sizeof(u16));
            return true;
    }

    //-----------------------------------------------
    template<class T>
    inline bool YunStream::addColArr( const ArrayFind<T>& v )
    {
        if (!m_beginAdd || size()+3+sizeof(T)*v.size() > YUN_CONTENT_MAX_LENGTH ) return false;
        add( &YunStream::_type_byteStream, sizeof(Byte) );
        m_types.push_back( Item(YunStream::Type_ByteStream, end()) );

        u16 len = sizeof(T)*v.size() + 2;    len = ByteUtils::Hton(len); add( &len, sizeof(len) );

        ByteStream& buf = *this;
        YunStreamHelper::set(buf, v);

        return true;
    }

} // end namespace hyyun

#endif
