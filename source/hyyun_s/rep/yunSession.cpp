//
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy yun network.
// author:       lipengxiang
// description:  
//

#include "../preheader.h"
#include "../../include/hyyun_s/yunSession.h"

//-----------------------------------------------
static void _GeneratePrivacyHandle( OUT u64& rnd, OUT u64& token )
{
    do  {
#if CN_CPU_ARCH == CN_CPU_ARCHITECTURE_64
        rnd = Math::Rand();
#else
        rnd = 0;
        u32 r = Math::Rand();
        rnd |= r;
        rnd <<= 32;
        r = Math::Rand();
        rnd |= r;
#endif
    } while (rnd == 0);

    token = rnd;
    CryptUtils::RC4_Crypt((Byte*)&token, sizeof(token), (Byte*)&key_s, sizeof(key_s));
}

//-----------------------------------------------
static bool _VerifyPrivacyHandle( const u64& rnd, const u64& token )
{
    // 验证正确性.
    CryptUtils::RC4_Decrypt((Byte*)&token, sizeof(token), (Byte*)&(key_c), sizeof(key_c));
    if (rnd == token)
    {
        return true;
    }

    return false;
}

static hyyun::YunSession::GeneratePrivacyHandle g_handle_generatePrivacy = _GeneratePrivacyHandle;
static hyyun::YunSession::VerifyPrivacyHandle   g_handle_verifyPrivacy = _VerifyPrivacyHandle;

namespace hyyun
{

    //-----------------------------------------------
    void YunSession::SetGeneratePrivacyHandle( GeneratePrivacyHandle handle )
    {
        g_handle_generatePrivacy = handle;
    }

    //-----------------------------------------------
    void YunSession::SetVerifyPrivacyHandle( VerifyPrivacyHandle handle )
    {
        g_handle_verifyPrivacy = handle;
    }

    //-----------------------------------------------
    void YunSession::generatePrivacy( OUT u64& rnd, OUT u64& token )
    {
        g_handle_generatePrivacy(rnd, token);
    }

    //-----------------------------------------------
    bool YunSession::verifyPrivacy( const u64& rnd, const u64& token )
    {
        return g_handle_verifyPrivacy(rnd, token);
    }

} // end namespace hyyun

