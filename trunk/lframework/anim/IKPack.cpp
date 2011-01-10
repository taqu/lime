/**
@file IKPack.cpp
@author t-sakai
@date 2010/12/30
*/
#include "IKPack.h"

namespace lanim
{
    IKPack::IKPack()
        :refCount_(0)
        ,numIKs_(0)
        ,iks_(NULL)
    {
    }

    IKPack::IKPack(u32 numIKs)
        :refCount_(0)
        ,numIKs_(numIKs)
    {
        if(numIKs_>0){
            iks_ = LIME_NEW IKEntry[numIKs_];
        }
    }

    IKPack::~IKPack()
    {
        LIME_DELETE_ARRAY(iks_);
    }

    void IKPack::swap(IKPack& rhs)
    {
        lcore::swap(refCount_, rhs.refCount_);
        lcore::swap(numIKs_, rhs.numIKs_);
        lcore::swap(iks_, rhs.iks_);
    }
}
