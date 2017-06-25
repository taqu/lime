/**
@file AsynchronousRef.cpp
@author t-sakai
@date 2017/06/26 create
*/
#include "AsynchronousRef.h"

namespace lgfx
{
    AsynchronousRef::AsynchronousRef(const AsynchronousRef& rhs)
        :asynchronous_(rhs.asynchronous_)
    {
        if(asynchronous_){
            asynchronous_->AddRef();
        }
    }

    AsynchronousRef::AsynchronousRef(AsynchronousRef&& rhs)
        :asynchronous_(rhs.asynchronous_)
    {
        rhs.asynchronous_ = NULL;
    }

    void AsynchronousRef::destroy()
    {
        LDXSAFE_RELEASE(asynchronous_);
    }

    AsynchronousRef& AsynchronousRef::operator=(const AsynchronousRef& rhs)
    {
        AsynchronousRef(rhs).swap(*this);
        return *this;
    }

    AsynchronousRef& AsynchronousRef::operator=(AsynchronousRef&& rhs)
    {
        if(this != &rhs){
            destroy();
            asynchronous_ = rhs.asynchronous_;
            rhs.asynchronous_ = NULL;
        }
        return *this;
    }

    void AsynchronousRef::swap(AsynchronousRef& rhs)
    {
        lcore::swap(asynchronous_, rhs.asynchronous_);
    }
}
