/**
@file ARCFOUR.cpp
@author t-sakai
@date 2013/08/09 create
*/
#include "ARCFOUR.h"
#include "CLibrary.h"

namespace lcore
{
    ARCFOURContext::ARCFOURContext()
    {
    }

    ARCFOURContext::ARCFOURContext(u8* key, u32 length)
    {
        initialize(key, length);
    }

    ARCFOURContext::~ARCFOURContext()
    {
    }

    void ARCFOURContext::initialize(u8* key, u32 length)
    {
        LASSERT(NULL != key);

        for(u32 i=0; i<ARCFOUR::StateSize; ++i){
            s_[i] = i;
        }

        u32 j=0;
        for(u32 i=0; i<ARCFOUR::StateSize; ++i){
            j = (j + s_[i] + key[i % length]) & ARCFOUR::StateMask;
            lcore::swap(s_[i], s_[j]);
        }
    }

    s32 ARCFOUR::encrypt(const ARCFOURContext& context, u8* data, s32 dataLength)
    {
        LASSERT(NULL != data);

        s32 i=0;
        s32 j=0;
        u8 s[StateSize];
        lcore::memcpy(s, context.s_, sizeof(u8)*StateSize);

        for(s32 k=0; k<dataLength; ++k){
            i = (i+1) & StateMask;
            j = (j+s[i]) & StateMask;
            lcore::swap(s[i], s[j]);
            data[k] ^= s[ (s[i] + s[j]) & StateMask ];
        }
        return dataLength;
    }

    s32 ARCFOUR::decrypt(const ARCFOURContext& context, u8* cipher, s32 cipherLength)
    {
        return encrypt(context, cipher, cipherLength);
    }
}
