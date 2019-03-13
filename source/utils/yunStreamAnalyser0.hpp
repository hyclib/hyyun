//
//  yunStreamAnalyser.hpp
//  yun network libaray
//
//  Created by brainpoint on 12-9-15.
//  Copyright (c) 2012年 华耀信息科技有限公司. All rights reserved.
//

#ifndef hyc_yunStreamAnalyser0_hpp
#define hyc_yunStreamAnalyser0_hpp

#include "../../include/utils/yunStreamAnalyser.h"

namespace hyyun
{
    // 0 版本解析器.
    extern "C" YunStreamAnalyserPtr g_yunStreamAnalyser_0;
    
    //----------------------------------------------------------------------------------------------------
    //* 默认的解析器.
    //----------------------------------------------------------------------------------------------------
    class __YunStreamAnalyser_0 : public YunStreamAnalyser
    {
    public:
        static const u8 Version = 0;
    public:
        
        virtual bool    isLegalYun_b( 
                                     const YunStream& ys, OUT s32& surplusSize,
                                     const Byte* beginPos, const Byte* endPos ) const;
        virtual u8      version_b(
                                  const YunStream& ys, const Byte* beginPos ) const;

        virtual bool    analyseSelf_b(
                                      const YunStream& ys, OUT YunStream::ItemArray& arrCol,
                                      OUT s32& surplusSize, 
                                      const Byte* beginPos, const Byte* endPos );
        

        virtual void addColBegin( OUT YunStream& ys, OUT YunStream::ItemArray& arrCol, bool crc32Check, bool contentSizeIn255 );
        virtual void addColEnd( OUT YunStream& ys, OUT YunStream::ItemArray& arrCol );
        
        // get version
        u8 _getPackVersion( const Byte* beginPos, const Byte* endPos ) const;
        
        // get crc32
        //  返回0表示没有crc32
        u32 _getPackCRC32( const Byte* beginPos, const Byte* endPos ) const;

        // 获得包头长度.
        // 返回==-1表示流不够长.
        s32 _getPackHeaderLength( const Byte* beginPos, const Byte* endPos ) const;
        
        // get content length; 可以用来验证报头
        // 足一个包返回内容长度, 不足返回负数.
        s32 _getPackLength( OUT s32& headerSize, const Byte* beginPos, const Byte* endPos ) const;
    };
}

#endif
