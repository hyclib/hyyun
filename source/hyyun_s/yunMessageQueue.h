//
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy yun network framework
// author:       lipengxiang
// create at:    2014-04-02 14:21:13
// description:  
//

#ifndef __BP_HYMESSAGEQUEUE_H_20140402142113__
#define __BP_HYMESSAGEQUEUE_H_20140402142113__

namespace hyyun
{
	class MessageQueue
    {
    public:
        MessageQueue( u32 len, const Array<hymq::MessageConsumer*> arrMsgConsumer );
        ~MessageQueue();

        void    start();
        void    stop();

        // mask表示线程掩码. -1则按顺序使用线程.
        void    push( hymq::MessageData data, uinteger mask );

        void    getThreadIds( OUT Array<integer>& arrIds );

    private:
        AtomicBase<u32>                _curIndex;
//         u32                            _curIndex;
        u32                            _size;
        FixArray<hymq::MessageQueue*>  _arrMsgQueue;
    };


    //-----------------------------------------------
    inline MessageQueue::MessageQueue( u32 len, const Array<hymq::MessageConsumer*> arrMsgConsumer )
        : _curIndex(0), _size(arrMsgConsumer.size()), _arrMsgQueue(arrMsgConsumer.size())
    {
            for (u32 i = 0; i < arrMsgConsumer.size(); i++ )
            {
                _arrMsgQueue[i] = new hymq::MessageQueue(arrMsgConsumer[i], len, true, false);
            }
    }

    //-----------------------------------------------
    inline MessageQueue::~MessageQueue()
    {
        stop();
        for (u32 i = 0; i < _arrMsgQueue.size(); i++)
        {
            delete _arrMsgQueue[i];
        }
    }

    //-----------------------------------------------
    inline void MessageQueue::push( hymq::MessageData data, uinteger mask )
    {
    	uinteger step = 0;
    	uinteger i;

        if (mask >= 0)
            i = mask%_size;
        else
        {
            i = ++_curIndex;
            i %= _size;
        }
        assert(i < _size && i >= 0);

//         while (true) {
//         print_all_line(_T("index %d"), mask%_size);
//            i = (mask==-1) ? ((++_curIndex)%_size) : (mask%_size);
//             if (_arrMsgQueue[i]->isFull())
//             {
//                 step++;
//                 if (step > _size)
//                 {
//                     trace_lite("message queue is full!");
//                     Thread::Sleep(1);
//                 }
// 
//                 continue;
//             }

            _arrMsgQueue[i]->push(data);
//             break;
//         }
    }

    //-----------------------------------------------
    inline void MessageQueue::start()
    {
        for (u32 i = 0; i < _arrMsgQueue.size(); i++)
        {
            _arrMsgQueue[i]->start();
        }
    }

    //-----------------------------------------------
    inline void MessageQueue::stop()
    {
        for (u32 i = 0; i < _arrMsgQueue.size(); i++)
        {
            _arrMsgQueue[i]->stopImmediately();
        }
    }

    //-----------------------------------------------
    inline void MessageQueue::getThreadIds( OUT Array<integer>& arrIds )
    {
        arrIds.clear();
        for (u32 i = 0; i < _arrMsgQueue.size(); i++)
        {
            arrIds.push_back(_arrMsgQueue[i]->getThreadId());
        }
    }

} // end namespace hyyun

#endif // end of define __BP_HYMESSAGEQUEUE_H_20140402142113__
