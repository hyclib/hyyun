
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#ifndef __BP_YUNSESSION_H_20140911141419__
#define __BP_YUNSESSION_H_20140911141419__


namespace hyyun
{
    /**
     * @brief 表示一个会话. 在response-request服务器中使用.
     */
    class HYYUNS_API YunSession : public RefCountSafe
    {
    public:
        /**
         * @brief 设置私密信息生成函数.
         */
        typedef void (*GeneratePrivacyHandle)( OUT u64& rnd, OUT u64& token );
        static void SetGeneratePrivacyHandle( GeneratePrivacyHandle handle );
        /**
         * @brief 设置私密信息验证函数.
         */
        typedef bool (*VerifyPrivacyHandle)( const u64& rnd, const u64& token );
        static void SetVerifyPrivacyHandle( VerifyPrivacyHandle handle );

    public:
        typedef u64 KEY;
        typedef u64 SID;

        /**
         * @brief 生成一对私密信息;
         * @param rnd 64位的随机数.
         * @param token 64位的令牌.
         */
        void    generatePrivacy( OUT u64& rnd, OUT u64& token );

        /**
         * @brief 验证私密信息是否正确.
         */
        bool    verifyPrivacy( const u64& rnd, const u64& token );

        /**
         * @brief sid
         */
        SID     sid;

        /**
         * @brief 最后一次的消息时间.
         */
        u32     timestamp;

        /**
         * @brief key
         */
        KEY     key_c;
        KEY     key_s;

        /**
         * @brief 长连接.
         */
        ConnectionPtr pConn;
        ConnectionPtr pConnRecent;    // 最近一次通信的连接.

        /**
         * @brief bundle communicate object.
         */
        Array<YunBundleObjectPtr> arrBundleObjs;

        /**
         * @brief userdata.
         */
        void*   userdata;
    };
    typedef ObjectPtr<YunSession> YunSessionPtr;

} // end namespace hyyun

#endif // end of define __BP_YUNSESSION_H_20140911141419__
