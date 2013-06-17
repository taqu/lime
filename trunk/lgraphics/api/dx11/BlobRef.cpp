/**
@file BlobRef.cpp
@author t-sakai
@date 2012/07/21 create
*/
#include "BlobRef.h"
#include <D3Dcompiler.h>

namespace lgraphics
{
    BlobRef::BlobRef()
        :blob_(NULL)
    {
    }

    BlobRef::BlobRef(const BlobRef& rhs)
        :blob_(rhs.blob_)
    {
        if(blob_){
            blob_->AddRef();
        }
    }

    BlobRef::BlobRef(ID3D10Blob* blob)
        :blob_(blob)
    {
    }

    BlobRef::~BlobRef()
    {
        SAFE_RELEASE(blob_);
    }

    void* BlobRef::getPointer()
    {
        return blob_->GetBufferPointer();
    }

    u32 BlobRef::getSize()
    {
        return blob_->GetBufferSize();
    }
}
