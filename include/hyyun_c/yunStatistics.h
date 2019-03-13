//
//  hyyunStatistics.h
//  hyyun_c
//
//  Created by brainpoint on 12-10-13.
//  Copyright (c) 2012骞?__MyCompanyName__. All rights reserved.
//

#ifndef hyyun_c_hyyunStatistics_h_20121013
#define hyyun_c_hyyunStatistics_h_20121013

namespace hyyun {
    
    //----------------------------------------------------------------------------------------------------
    //! 用于统计流量数据.
    //----------------------------------------------------------------------------------------------------
    class HYYUN_API YunStatistics : public RefCount
    {    
    public:
        YunStatistics();
        virtual ~YunStatistics();
        
        /**
         * @brief 统计间隔.(默认5000ms)
         */
        s32         getElapse() const;
        void        setElapse( s32 elapse );
        
        /**
         * @brief 获得到目前为止发送接收的数据量
         */
        const u32&  getSendSize() const;
        const u32&  getRecvSize() const;
        
        /**
         * @brief 获得当前的速度 byte/s
         */
        const u32&  getBps() const;
        
        /**
         * @brief 重置.
         */
        void        resetSendSize();
        void        resetRecvSize();
        void        resetSendAndRecvSize();

    protected:        
        void    _addSendSize(const u32& sz);
        void    _addRecvSize(const u32& sz);

    private:
        u32         _sendSize;
        u32         _recvSize;
        u32         _bpsDataSize;
        u32         _bps;
        Timer       _timer;
        s32         _elpase;
    };

    
} // end namespace hyyun

#endif // hyyun_c_hyyunStatistics_h_20121013
