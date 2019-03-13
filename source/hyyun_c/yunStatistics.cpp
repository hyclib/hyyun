//
// Copyright(c) 1999-2012 hyrj. All rights reserved.
// base network libaray
// author:       lipengxiang
// description:  
//


#include "preheader.h"

namespace hyyun
{   
    //----------------------------------------------------------------------------------------------------
    //!	YunStatistics
    //----------------------------------------------------------------------------------------------------
    
    //-----------------------------------------------
    YunStatistics::YunStatistics()
    {
        _timer.start();
        _bps = 0;
        _bpsDataSize = 0;
        _recvSize = 0;
        _sendSize = 0;
        _elpase = 5000;
    }
    
    //-----------------------------------------------
    YunStatistics::~YunStatistics()
    {
    }
    
    //-----------------------------------------------
    const u32& YunStatistics::getSendSize() const
    { 
        
        return _sendSize;
    }

    //-----------------------------------------------
    const u32& YunStatistics::getRecvSize() const
    { 
        
        return _recvSize; 
    }

    //-----------------------------------------------
    void YunStatistics::resetSendSize()
    {
        
        _sendSize = 0;
    }

    //-----------------------------------------------
    void YunStatistics::resetRecvSize()
    {
        
        _recvSize = 0;
    }

    //-----------------------------------------------
    void YunStatistics::resetSendAndRecvSize()
    {
        
        _recvSize = _sendSize = 0;
    }

    //-----------------------------------------------
    void YunStatistics::_addSendSize( const u32& sz )
    {
        
        _sendSize += sz;
        _bpsDataSize += sz;
        if (_timer.getMilliseconds() > _elpase && _timer.getMilliseconds() > 0)
        {
            _bps = _bpsDataSize * 1000 / _timer.getMilliseconds();
            _timer.start();
            _bpsDataSize = 0;
        }
    }

    //-----------------------------------------------
    void YunStatistics::_addRecvSize( const u32& sz )
    {
        
        _recvSize += sz;
        _bpsDataSize += sz;
        if (_timer.getMilliseconds() > _elpase && _timer.getMilliseconds() > 0)
        {
            _bps = _bpsDataSize * 1000 / _timer.getMilliseconds();
            _timer.start();
            _bpsDataSize = 0;
        }
    }

    //-----------------------------------------------
    const u32& YunStatistics::getBps() const
    {
        
        return _bps;
    }

    //-----------------------------------------------
    hyc::s32 YunStatistics::getElapse() const
    {
        
        return _elpase;
    }

    //-----------------------------------------------
    void YunStatistics::setElapse( s32 elapse )
    {
        
        _elpase = elapse;
    }

} // end namespace hyyun

