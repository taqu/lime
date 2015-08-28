#ifndef INC_LGRAPHICS_DX11_ASYNCHRONOUSREF_H__
#define INC_LGRAPHICS_DX11_ASYNCHRONOUSREF_H__
/**
@file AsynchronousRef.h
@author t-sakai
@date 2014/10/10 create
*/
#include "../../lgraphics.h"
#include "Enumerations.h"

struct ID3D11Asynchronous;

namespace lgraphics
{
    //--------------------------------------------------------
    //---
    //--- AsynchronousRef
    //---
    //--------------------------------------------------------
    class AsynchronousRef
    {
    public:
        typedef ID3D11Asynchronous element_type;
        typedef ID3D11Asynchronous* pointer_type;
        typedef pointer_type const* pointer_array_type;

        void destroy()
        {
            SAFE_RELEASE(asynchronous_);
        }

        pointer_type get(){ return asynchronous_;}
        operator pointer_type(){ return asynchronous_;}
        operator pointer_array_type(){ return &asynchronous_;}

        bool valid() const
        {
            return (NULL != asynchronous_);
        }

        u32 getDataSize()
        {
            return asynchronous_->GetDataSize();
        }

        AsynchronousRef& operator=(const AsynchronousRef& rhs)
        {
            AsynchronousRef(rhs).swap(*this);
            return *this;
        }

        void swap(AsynchronousRef& rhs)
        {
            lcore::swap(asynchronous_, rhs.asynchronous_);
        }
    protected:
        friend class GraphicsDeviceRef;
        friend class ContextRef;

        AsynchronousRef()
            :asynchronous_(NULL)
        {}

        AsynchronousRef(const AsynchronousRef& rhs)
            :asynchronous_(rhs.asynchronous_)
        {
            if(asynchronous_){
                asynchronous_->AddRef();
            }
        }

        explicit AsynchronousRef(pointer_type asynchronous)
            :asynchronous_(asynchronous)
        {
        }

        ~AsynchronousRef()
        {
            destroy();
        }

        pointer_type asynchronous_;
    };
}
#endif //INC_LGRAPHICS_DX11_ASYNCHRONOUSREF_H__
