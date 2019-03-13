//
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// author:       lipengxiang
// description:  
//

#include "preheader.h"
#include "yunClientMessageCenter.h"

namespace hyyun
{


    //-----------------------------------------------
    bool ClientMessageCenter::addProcessor( YunClientHandlerPtr msgProcessor )
    {
        //             SyncTokenPtr token = getSync().getToken();
        if (!msgProcessor.get())    return false;
        MsgProMap::iterator itr = _map.find( msgProcessor->getMessageMark() );
        if ( itr != _map.end() )
        {
            return false;
        }
        else
        {
            _map.insert( std::make_pair(msgProcessor->getMessageMark(), msgProcessor) );
            return true;
        }
    }

    //-----------------------------------------------
    YunClientHandlerPtr& ClientMessageCenter::getProcessor( u16 mark )
    {
        static YunClientHandlerPtr processor;
        MsgProMap::iterator itr = _map.find(mark);
        if (itr != _map.end())
            return itr->second;

        return processor;
    }
} // end namespace hyyun
