#ifndef INC_LGRAPHICS_DX11_ASYNCHRONOUSREF_H__
#define INC_LGRAPHICS_DX11_ASYNCHRONOUSREF_H__
/**
@file AsynchronousRef.h
@author t-sakai
@date 2014/10/10 create
*/
#include "../lgraphics.h"
#include "Enumerations.h"

struct ID3D11Asynchronous;

namespace lgfx
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

        AsynchronousRef()
            :asynchronous_(NULL)
        {}
        AsynchronousRef(const AsynchronousRef& rhs);
        AsynchronousRef(AsynchronousRef&& rhs);

        ~AsynchronousRef()
        {
            destroy();
        }

        void destroy();

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

        AsynchronousRef& operator=(const AsynchronousRef& rhs);
        AsynchronousRef& operator=(AsynchronousRef&& rhs);

        void swap(AsynchronousRef& rhs);
    protected:
        friend class GraphicsDeviceRef;
        friend class ContextRef;

        explicit AsynchronousRef(pointer_type asynchronous)
            :asynchronous_(asynchronous)
        {
        }

        pointer_type asynchronous_;
    };
}
#endif //INC_LGRAPHICS_DX11_ASYNCHRONOUSREF_H__
