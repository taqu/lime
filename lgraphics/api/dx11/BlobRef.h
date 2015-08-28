#ifndef INC_LGRAPHICS_DX11_BLOBREF_H__
#define INC_LGRAPHICS_DX11_BLOBREF_H__
/**
@file BlobRef.h
@author t-sakai
@date 2012/07/21 create
*/
#include "../../lgraphicscore.h"
struct ID3D10Blob;

namespace lgraphics
{
    class BlobRef
    {
    public:
        typedef ID3D10Blob element_type;
        typedef ID3D10Blob* pointer_type;
        typedef pointer_type const* pointer_array_type;

        BlobRef();
        BlobRef(const BlobRef& rhs);
        explicit BlobRef(pointer_type blob);
        ~BlobRef();

        pointer_type get(){ return blob_;}
        operator pointer_type(){ return blob_; }
        operator pointer_array_type(){ return &blob_;}

        bool valid() const{ return (blob_!=NULL);}

        void* getPointer();
        u32 getSize();

        BlobRef& operator=(const BlobRef& rhs)
        {
            BlobRef(rhs).swap(*this);
            return *this;
        }

        void swap(BlobRef& rhs)
        {
            lcore::swap(blob_, rhs.blob_);
        }
    private:
        pointer_type blob_;
    };
}
#endif //INC_LGRAPHICS_DX11_BLOBREF_H__
