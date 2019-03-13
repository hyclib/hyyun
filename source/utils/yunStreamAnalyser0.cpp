//
//  yunStreamAnalyser.cpp
//  yun network libaray
//
//  Created by brainpoint on 12-9-15.
//  Copyright (c) 2012年 华耀信息科技有限公司. All rights reserved.
//

#include "preheader.h"
#include "yunStreamAnalyser0.hpp"

namespace hyyun
{
    YunStreamAnalyserPtr    g_yunStreamAnalyser_0 = new __YunStreamAnalyser_0;
    

    //-----------------------------------------------
    s32 __YunStreamAnalyser_0::_getPackHeaderLength( const Byte* beginPos, const Byte* endPos ) const
    {
        u32 bufLen = (u32)(endPos - beginPos);

        if (!beginPos || bufLen<=0)
        {
            return -1;
        }
        else
        {
            u8 v1 = ByteUtils::ReadBit(*beginPos, 7); // 判读crc位是否存在.
            u8 v2 = ByteUtils::ReadBit(*beginPos, 6); // 判读length==1/2

            
            //s32 bufLen = m_pStream->size();
            s32 headerSize;

            switch (v1)
            {
            case 0: // 不存在crc
                {
                    headerSize = 1;
                    switch (v2)
                    {
                    case 0: // 长度为1
                        headerSize += 1;
                        break;
                    case 1: // 长度为2
                        headerSize += 2;

                        break;
                    default:
                        assert(false);
                        headerSize = -1;
                        break;
                    }
                } break;
            case 1: // 存在crc
                {
                    headerSize = 1+4;
                    switch (v2)
                    {
                    case 0:
                        headerSize += 1;
                        break;
                    case 1:
                        headerSize += 2;
                        break;
                    default:
                        assert(false);
                        return -1;
                    }

                } break;
            default:
                assert(false);
                return -1;
            }

            return headerSize;
        } // if..else
    }

    //-----------------------------------------------
    s32 __YunStreamAnalyser_0::_getPackLength( OUT s32& headerSize1, const Byte* beginPos, const Byte* endPos ) const
    {        
        u32 bufLen = (u32)(endPos - beginPos);
        u32 headerSize = _getPackHeaderLength(beginPos, endPos);
        headerSize1 = headerSize;
        if (headerSize == -1)
            return -1;
        else
        {
            s32 contentLen = 0;
            u16 len;

            switch (headerSize)
            {
            case 2:
                if (bufLen < headerSize)  return bufLen-headerSize;
                memcpy(&contentLen, (beginPos+1), sizeof(u8));
                //contentLen = *(u8*)(beginPos+1);
                return contentLen;

                break;
            case 3:
                if (bufLen < headerSize)  return bufLen-headerSize;
                memcpy(&len, (beginPos+1), sizeof(u16));
                len = ByteUtils::Ntoh(len);
                //contentLen = *(u16*)(beginPos+1);
                return (s32)len;

                break;
            case 6:
                if (bufLen < headerSize)  return bufLen-headerSize;
                memcpy(&contentLen, (beginPos+5), sizeof(u8));
                //contentLen = *(u8*)(beginPos+5);
                return contentLen;

                break;
            case 7:
                if (bufLen < headerSize)  return bufLen-headerSize;
                memcpy(&len, (beginPos+5), sizeof(u16));
                len = ByteUtils::Ntoh(len);
                //contentLen = *(u16*)(beginPos+5);
                return (s32)len;
                break;
            default:
                return -1;
            }

        } // if..else
    }
    
    //-----------------------------------------------    
    u32 __YunStreamAnalyser_0::_getPackCRC32( const Byte* beginPos, const Byte* endPos ) const
    {        
        u32 bufLen = (u32)(endPos-beginPos);

        if (!beginPos || bufLen<=0)
        {
            return 0;
        }
        else
        {
            u8 v1 = ByteUtils::ReadBit(*beginPos, 7); // 判读crc位是否存在.
            //s32 bufLen = m_pStream->size();
            
            switch (v1)
            {
                case 0: // 不存在crc
                {
                    return 0;
                } break;
                case 1: // 存在crc
                {
                    if (bufLen < 5)  return -1;

                    u32 crc;
                    memcpy(&crc, (beginPos+1), sizeof(u32));
                    crc = ByteUtils::Ntoh(crc);
                    //return *(u32*)(beginPos+1);
                    return crc;
                } break;
                default:
                    assert(false);
                    return 0;
            }
        } // if..else
    }
    
    //         // 判断包是否压缩
    //         static bool _getPackCompress( Byte* beginPos, const YunStream* const& m_pStream )
    //         {
    //             assert(beginPos >= m_pStream->begin());
    //             if (!m_pStream || m_pStream->size()<=0)
    //             {
    //                 return false;
    //             }
    //             else
    //             {
    //                 return 1 == ByteUtils::ReadBit(*beginPos, 5);
    //             } // if..else
    //         }
    
    //-----------------------------------------------    
    u8 __YunStreamAnalyser_0::_getPackVersion( const Byte* beginPos, const Byte* endPos ) const
    {
        if (!beginPos)
        {
            return 0;
        }
        else
        {
            u8 v;// = *beginPos;
            memcpy(&v, beginPos, sizeof(u8));
            v <<= 4;
            v >>= 4;
            return v;
        } // if..else
    }

    
    //-----------------------------------------------
    void __YunStreamAnalyser_0::addColBegin( OUT YunStream& ys, OUT YunStream::ItemArray& arrCol, bool crc32Check, bool contentSizeIn255 )
    {
        // 报头大小.
        u16 headerSize = 1;
        if (crc32Check)
            headerSize += 4;
        
        if (contentSizeIn255)
            headerSize += 1;
        else
            headerSize += 2;
        
        ys.resize(headerSize);
        ZeroMemory(ys.begin(), ys.size());
        
        // 构造第一个字节.
        //Byte& headerFirst = *ys.begin();
        //headerFirst = 0x00;
        Byte headerFirst = 0x00;

        if (crc32Check)
            ByteUtils::WriteBit( headerFirst, 7, 1 );
        if (!contentSizeIn255)
            ByteUtils::WriteBit( headerFirst, 6, 1 );

        memcpy(ys.begin(), &headerFirst, sizeof(Byte));
    }
    
    //-----------------------------------------------
    void __YunStreamAnalyser_0::addColEnd( OUT YunStream& ys, OUT YunStream::ItemArray& arrCol )
    {
        //Byte& headerFirst = *ys.begin();
        Byte headerFirst;
        memcpy(&headerFirst, ys.begin(), sizeof(Byte));

        u32 headerSize = _getPackHeaderLength(ys.begin(), ys.end());
        assert(headerSize > 0);

        if (ys.size() > headerSize)  // 存在内容.
        {
            if (1 == ByteUtils::ReadBit( headerFirst, 7 ))  // crc32
            {
                //u32* crc32 = (u32*)(ys.begin()+1);
                //*crc32 = CryptUtils::CRC32( ys.begin()+headerSize, ys.size()-headerSize );
                u32 crc32;
                crc32 = CryptUtils::CRC32( ys.begin()+headerSize, ys.size()-headerSize );
                crc32 = ByteUtils::Hton(crc32);
                memcpy(ys.begin()+1, &crc32, sizeof(u32));
                
                // length
                if (1 == ByteUtils::ReadBit( headerFirst, 6 ))  // 2
                {
                    //u16* len = (u16*)(ys.begin()+5);
                    //*len = ys.size()-headerSize;
                    u16 len = ys.size()-headerSize;
                    len = ByteUtils::Hton(len);
                    memcpy(ys.begin()+5, &len, sizeof(u16));
                }
                else    // 1
                {
                    assert(ys.size()-headerSize <= 255);
                    //u8* len = (u8*)(ys.begin()+5);
                    //*len = ys.size()-headerSize;
                    u8 len = ys.size()-headerSize;
                    memcpy(ys.begin()+5, &len, sizeof(u8));
                }
            }
            else
            {
                // length
                if (1 == ByteUtils::ReadBit( headerFirst, 6 ))  // 2
                {
                    //u16* len = (u16*)(ys.begin()+1);
                    //*len = ys.size()-headerSize;
                    u16 len = ys.size()-headerSize;
                    len = ByteUtils::Hton(len);
                    memcpy(ys.begin()+1, &len, sizeof(u16));
                }
                else    // 1
                {
                    assert(ys.size()-headerSize <= 255);
                    //u8* len = (u8*)(ys.begin()+1);
                    //*len = ys.size()-headerSize;
                    u8 len = ys.size()-headerSize;
                    memcpy(ys.begin()+1, &len, sizeof(u8));
                }
            } // if..else
        }
    }
    
    //-----------------------------------------------
    bool __YunStreamAnalyser_0::isLegalYun_b( 
                                     const YunStream& ys, OUT s32& surplusSize,
                                     const Byte* beginPos, const Byte* endPos ) const
    {
        if (!beginPos || !endPos)
        {
            surplusSize = 0;
            return false;
        }
        
        /* 1.0 版本不处理压缩 */
        s32 size = (s32)(endPos - beginPos);

        if (size < 2) 
        {
            surplusSize = -2;
            return false;
        }
        
        s32 headerSize;
        s32 contentLen = _getPackLength( headerSize, beginPos, endPos );
        
        if (0 > contentLen && size < headerSize)    
        {
            surplusSize = -headerSize;
            return false;
        }
        else
        {
            s32 packLen = headerSize+contentLen;
            surplusSize = size - packLen;

            if (Version != _getPackVersion((Byte*)beginPos, endPos))
                return false;
            

            u32 crc32 = _getPackCRC32((Byte*)beginPos, endPos);
            if (0 == crc32)
            {
                if (headerSize+contentLen <= size)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                if (headerSize+contentLen <= size && crc32 == CryptUtils::CRC32( beginPos+headerSize, contentLen))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            
        } // if..else
    }
    
    //-----------------------------------------------
    u8 __YunStreamAnalyser_0::version_b(
                                  const YunStream& ys, const Byte* beginPos ) const
    {
        if (!beginPos)
            return false;
        
        return _getPackVersion((Byte*)beginPos, beginPos+1);
    }
    
    //-----------------------------------------------
    bool __YunStreamAnalyser_0::analyseSelf_b(
                                                   const YunStream& ys, OUT YunStream::ItemArray& arrCol,
                                      OUT s32& surplusSize, 
                                      const Byte* beginPos, const Byte* endPos )
    {
        Byte* pStream = (Byte*)beginPos;
        s32 csize = (s32)(endPos-pStream);
        
        arrCol.clear();
        s32 m_headerSize = 0;
        
        // 判断包长度.
        s32 contentLen = 0;
        
        contentLen = _getPackLength(m_headerSize, beginPos, endPos);
        if (contentLen < 0)
        {
            surplusSize = contentLen;
            return false;
        }
        else    // 头部构造完成.
        {
            if (Version != version_b(ys, beginPos))
            {
                surplusSize = 0;
                return false;
            }
            
            if (contentLen+m_headerSize > csize)
            {
                surplusSize = csize-(contentLen+m_headerSize);
                return false;                    
            }
            else
            {
                assert(m_headerSize <= csize);
                surplusSize = csize-(contentLen+m_headerSize);
                
                // 构造类型表.
                if (0 == contentLen)
                {
                    if (csize >= /*contentLen+*/m_headerSize)  
                    {
                        return true;
                    }
                    else
                    {
                        return false; 
                    }  
                }
                else
                {
                    Byte* ptr = const_cast<Byte*>(beginPos+m_headerSize);
                    u16 temp;
                    s32 pos = 0;
                    while (ptr < endPos && pos < contentLen)
                    {
                        switch ( (*ptr) )
                        {
                            case YunStream::Type_bool:
                                arrCol.push_back( YunStream::Item(YunStream::Type_bool, ptr+1) );
                                ptr += 1 + sizeof(bool);
                                pos += 1 + sizeof(bool);
                                break;
                            case YunStream::Type_u8:
                                arrCol.push_back( YunStream::Item(YunStream::Type_u8, ptr+1) );
                                ptr += 1 + sizeof(u8);
                                pos += 1 + sizeof(u8);
                                break;
                            case YunStream::Type_u16:
                                arrCol.push_back( YunStream::Item(YunStream::Type_u16, ptr+1) );
                                ptr += 1 + sizeof(u16);
                                pos += 1 + sizeof(u16);
                                break;
                            case YunStream::Type_u32:
                                arrCol.push_back( YunStream::Item(YunStream::Type_u32, ptr+1) );
                                ptr += 1 + sizeof(u32);
                                pos += 1 + sizeof(u32);
                                break;
                            case YunStream::Type_u64:
                                arrCol.push_back( YunStream::Item(YunStream::Type_u64, ptr+1) );
                                ptr += 1 + sizeof(u64);
                                pos += 1 + sizeof(u64);
                                break;
                            case YunStream::Type_s8:
                                arrCol.push_back( YunStream::Item(YunStream::Type_s8, ptr+1) );
                                ptr += 1 + sizeof(s8);
                                pos += 1 + sizeof(s8);
                                break;
                            case YunStream::Type_s16:
                                arrCol.push_back( YunStream::Item(YunStream::Type_s16, ptr+1) );
                                ptr += 1 + sizeof(s16);
                                pos += 1 + sizeof(s16);
                                break;
                            case YunStream::Type_s32:
                                arrCol.push_back( YunStream::Item(YunStream::Type_s32, ptr+1) );
                                ptr += 1 + sizeof(s32);
                                pos += 1 + sizeof(s32);
                                break;
                            case YunStream::Type_s64:
                                arrCol.push_back( YunStream::Item(YunStream::Type_s64, ptr+1) );
                                ptr += 1 + sizeof(s64);
                                pos += 1 + sizeof(s64);
                                break;
                            case YunStream::Type_f32:
                                arrCol.push_back( YunStream::Item(YunStream::Type_f32, ptr+1) );
                                ptr += 1 + sizeof(f32);
                                pos += 1 + sizeof(f32);
                                break;
                            case YunStream::Type_f64:
                                arrCol.push_back( YunStream::Item(YunStream::Type_f64, ptr+1) );
                                ptr += 1 + sizeof(f64);
                                pos += 1 + sizeof(f64);
                                break;
                            case YunStream::Type_ByteStream:
                                arrCol.push_back( YunStream::Item(YunStream::Type_ByteStream, ptr+1) );
                                memcpy(&temp, ptr+1, sizeof(u16));
                                temp = ByteUtils::Ntoh(temp);
                                pos += 3 + temp;//*(u16*)(ptr+1);
                                ptr += 3 + temp;//*(u16*)(ptr+1);
                                break;
                            case YunStream::Type_String:
                                arrCol.push_back( YunStream::Item(YunStream::Type_String, ptr+1) );
                                memcpy(&temp, ptr+1, sizeof(u16));
                                temp = ByteUtils::Ntoh(temp);
                                pos += 3 + temp;//*(u16*)(ptr+1);
                                ptr += 3 + temp;//*(u16*)(ptr+1);
                                break;
                            default:
                                surplusSize = 0;
                                return false;
                        }
                    }
                    
                } // if..else
                
                return true;
            }
        } // if..else
        
        return false;
    }

}
