/**
@file IKPack.cpp
@author t-sakai
@date 2016/11/24 create
*/
#include "animation/IKPack.h"

namespace lfw
{
    IKPack::IKPack()
        :refCount_(0)
        ,numIKs_(0)
        ,iks_(NULL)
    {
    }

    IKPack::IKPack(s32 numIKs)
        :refCount_(0)
        ,numIKs_(numIKs)
    {
        if(numIKs_>0){
            iks_ = static_cast<IKEntry*>(LMALLOC(sizeof(IKEntry)*numIKs_));
            for(s32 i=0; i<numIKs_; ++i){
                LPLACEMENT_NEW(&iks_[i]) IKEntry;
            }
        }
    }

    IKPack::~IKPack()
    {
        for(s32 i=0; i<numIKs_; ++i){
            iks_[i].~IKEntry();
        }
        numIKs_ = 0;
        LFREE(iks_);
    }

    void IKPack::swap(IKPack& rhs)
    {
        lcore::swap(refCount_, rhs.refCount_);
        lcore::swap(numIKs_, rhs.numIKs_);
        lcore::swap(iks_, rhs.iks_);
    }
}
