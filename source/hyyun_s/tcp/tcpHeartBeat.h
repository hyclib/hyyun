//
// Copyright(c) 1999-2012 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#ifndef __BP_SERVERHEARTBEAT_H_20120223141635__
#define __BP_SERVERHEARTBEAT_H_20120223141635__

#include "../../../include/hyyun_s/yunTcpAgent.h"
#include <list>

namespace hyyun
{
    //----------------------------------------------------------------------------------------------------
    //!	HeartBeatList
    //! HeartBeatData::_onDisconnectCallback 函数中可能存在于List竞争锁的情形,故需对操作做tryLock操作, addConn/waitForEmpty不考虑.
    //----------------------------------------------------------------------------------------------------
    class TcpHeartBeat
    {
    public:
        typedef ObjectPtr<TcpAgent>   HeartBeatData;

    public:
        TcpHeartBeat();
        ~TcpHeartBeat();

        void addConn( TcpAgent* ptr );

        // 返回值指示是否存在.
        bool erase( TcpAgent* ptr );

        // 移除全部死亡的连接, 内部会调用finished.
        void eraseDead();

        // 移除全部.
        void eraseAll();

        // 等待直到连接数为空.
        void waitForEmpty();

        inline u32 size() const  { return _heartBeatNum; }

    protected:
        typedef AsyncList<HeartBeatData> List;  // u8: 0则为没有响应,1则为响应

        static bool __remove_dead( HeartBeatData& data );
        
    private:
        List                _list;
        AtomicBase<u32>     _heartBeatNum;

        SyncObject          _cvSync;
        ConditionVariable   _cv;
    };

	

} // end namespace hyyun

#endif // end of define __BP_SERVERHEARTBEAT_H_20120223141635__

