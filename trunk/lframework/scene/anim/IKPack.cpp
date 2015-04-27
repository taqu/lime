/**
@file IKPack.cpp
@author t-sakai
@date 2010/12/30
*/
#include "IKPack.h"

namespace lscene
{
namespace lanim
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
            iks_ = reinterpret_cast<IKEntry*>(LSCENE_MALLOC(sizeof(IKEntry)*numIKs_));
            for(s32 i=0; i<numIKs_; ++i){
                LSCENE_PLACEMENT_NEW(&iks_[i]) IKEntry;
            }
        }
    }

    IKPack::~IKPack()
    {
        for(s32 i=0; i<numIKs_; ++i){
            iks_[i].~IKEntry();
        }
        numIKs_ = 0;
        LSCENE_FREE(iks_);
    }

    void IKPack::swap(IKPack& rhs)
    {
        lcore::swap(refCount_, rhs.refCount_);
        lcore::swap(numIKs_, rhs.numIKs_);
        lcore::swap(iks_, rhs.iks_);
    }
}
}
