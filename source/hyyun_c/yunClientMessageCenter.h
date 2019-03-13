//
// Copyright(c) 1999-2013 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#ifndef __BP_CLIENTMESSAGECENTER_H_2013032595747__
#define __BP_CLIENTMESSAGECENTER_H_2013032595747__

#include <map>

namespace hyyun
{
    //----------------------------------------------------------------------------------------------------
    //!	ClientMessageCenter; 消息中心, 用来适配各种消息包的;
    //! 其要求被适配的消息的第一个字段为u16(用来标识消息包)
    //----------------------------------------------------------------------------------------------------
    class ClientMessageCenter : public INoncopyable
    {
    public:
        ClientMessageCenter()   {}
        ~ClientMessageCenter()  {}

        //! \brief
        //!     添加消息处理器
        //! \return
        //!     返回false表示拥有相同的消息包标识符的处理器已经存在or msgProcessor 为空.
        bool                     addProcessor( YunClientHandlerPtr msgProcessor );
        YunClientHandlerPtr&      getProcessor( u16 mark );

    private:
        typedef std::map<u16, YunClientHandlerPtr>  MsgProMap;
        MsgProMap   _map;
    };

} // end namespace hyyun

#endif // end of define __BP_CLIENTMESSAGECENTER_H_2013032595747__