//
// Copyright(c) 1999-2016 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#ifndef __BP_YUNSTREAMEMPTYANALYSER_HPP_20160215155504__
#define __BP_YUNSTREAMEMPTYANALYSER_HPP_20160215155504__

#include "yunStreamAnalyser.h"

namespace hyyun
{
    class YunStreamEmptyAnalyser;
    typedef ObjectPtr<YunStreamEmptyAnalyser> YunStreamEmptyAnalyserPtr;

    //----------------------------------------------------------------------------------------------------
    //* 空的消息处理流,什么都不处理.
    //----------------------------------------------------------------------------------------------------
    class HYYUN_API YunStreamEmptyAnalyser : public YunStreamAnalyser
    {
    public:
        static YunStreamAnalyser*   Instance()  { static YunStreamEmptyAnalyser* g = new YunStreamEmptyAnalyser(); return g; }

    public:
        virtual u8      version_b(const YunStream& ys, const Byte* beginPos) const  { return 0; }

        virtual bool    isLegalYun_b(
            const YunStream& ys,
            OUT s32& surplusSize,
            const Byte* beginPos, const Byte* endPos) const 
        {
            surplusSize = 0; 
            return true;
        }

        virtual bool    analyseSelf_b(
            const YunStream& ys,
            OUT ItemArray& arrCol,
            OUT s32& surplusSize,
            const Byte* beginPos, const Byte* endPos)
        {
            surplusSize = 0;
            return true;
        }

        virtual void    addColBegin(OUT YunStream& ys, OUT ItemArray& arrCol, bool crc32Check, bool contentSizeIn255) {}
        virtual void    addColEnd(OUT YunStream& ys, OUT ItemArray& arrCol) {}
    };


} // end namespace hyyun

#endif // end of define __BP_YUNSTREAMEMPTYANALYSER_HPP_20160215155504__
