//
// Copyright(c) 1999-2013 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#include "../preheader.h"
#include "tcpHeartBeat.h"
#include <algorithm>

#define LOCK_TRY_TIME 3

namespace hyyun
{
    TcpHeartBeat::TcpHeartBeat() : _heartBeatNum(0) 
    {

    }

    TcpHeartBeat::~TcpHeartBeat()
    {
        eraseAll();
    }

    //-----------------------------------------------
    void TcpHeartBeat::addConn( TcpAgent* ptr )
    {
        // add 必须成功,外部需考虑死锁情况.
        _list.lock();
        {
            _list.push_back( ptr );
            ++_heartBeatNum;
        }
        _list.unlock();
    }

    //-----------------------------------------------
    bool TcpHeartBeat::erase( TcpAgent* ptr )
    {
        assert(ptr);
        bool r = false;

        s32 step = LOCK_TRY_TIME;
        while (step-- > 0)
        {
            if (_list.tryLock())
            {
                List::iterator itr = std::find( _list.begin(), _list.end(), ptr );
                if (itr != _list.end())
                {
                    r = true;
                    //(*itr)->finished();
                    ptr->_setHeartBeatFlag(TCP_HEARTBEAT_REMOVE);
                    ptr->_onDisconnectCallback();
                    ptr->close();

                    _list.erase(itr);
                    --_heartBeatNum;

                    // 唤醒全部的等待.
                    if (0 == _heartBeatNum)
                        _cv.broadcast();
                }
                _list.unlock();
                return r;
            }
        }

        ptr->_setHeartBeatFlag(TCP_HEARTBEAT_REMOVE);
        return r;
    }

    //-----------------------------------------------
    void TcpHeartBeat::eraseDead()
    {
        s32 step = LOCK_TRY_TIME;
        while (step-- > 0)
        {
            if (_list.tryLock())
            {
                _list.remove_if( __remove_dead );
                _heartBeatNum = _list.size();

                // 唤醒全部的等待.
                if (0 == _heartBeatNum)
                    _cv.broadcast();
                _list.unlock();
                return;
            }
        }
    }

    //-----------------------------------------------
    void TcpHeartBeat::eraseAll()
    {
        s32 step = LOCK_TRY_TIME;
        while (step-- > 0)
        {
            if (_list.tryLock())
            {
                for (List::iterator itr = _list.begin(); itr != _list.end(); itr++)
                {
                    (*itr)->_onDisconnectCallback();
                    (*itr)->close();
                }

                _list.clear();
                _heartBeatNum = _list.size();

                // 唤醒全部的等待.
                if (0 == _heartBeatNum)
                    _cv.broadcast();
                _list.unlock();
                return;
            }
        }
    }

    //-----------------------------------------------
    bool TcpHeartBeat::__remove_dead( HeartBeatData& data )
    {
        if (!data.get())    return true;
        if (TCP_HEARTBEAT_REMOVE != data->_getHeartBeatFlag())
        {
            data->_setHeartBeatFlag(TCP_HEARTBEAT_REMOVE);
            return false;
        }

        data->_onDisconnectCallback();
        data->close();

        return true;
    }

    //-----------------------------------------------
    void TcpHeartBeat::waitForEmpty()
    {
        if (0 != _heartBeatNum)
        {
            _cvSync.lock();
            if (0 != _heartBeatNum)
                _cv.wait(_cvSync);
            _cvSync.unlock();
        }
    }

} // end namespace hyyun
