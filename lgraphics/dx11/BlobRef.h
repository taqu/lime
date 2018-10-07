#ifndef INC_LGRAPHICS_DX11_BLOBREF_H_
#define INC_LGRAPHICS_DX11_BLOBREF_H_
/**
@file BlobRef.h
@author t-sakai
@date 2012/07/21 create
*/
#include "../lgraphics.h"
struct ID3D10Blob;

namespace lgfx
{
    class BlobRef
    {
    public:
        typedef ID3D10Blob element_type;
        typedef ID3D10Blob* pointer_type;
        typedef pointer_type const* pointer_array_type;

        BlobRef();
        BlobRef(const BlobRef& rhs);
        BlobRef(BlobRef&& rhs);
        explicit BlobRef(pointer_type blob);
        ~BlobRef();

        pointer_type get(){ return blob_;}
        operator pointer_type(){ return blob_; }
        operator pointer_array_type(){ return &blob_;}

        bool valid() const{ return (blob_!=NULL);}

        void* getPointer();
        size_t getSize();

        BlobRef& operator=(const BlobRef& rhs);
        BlobRef& operator=(BlobRef&& rhs);

        void swap(BlobRef& rhs)
        {
            lcore::swap(blob_, rhs.blob_);
        }
    private:
        pointer_type blob_;
    };

    static const u32 Signature_BlobUncompressed = 'LBPU';
    static const u32 Signature_BlobCompressed = 'LBPC';

    struct BlobPack
    {
        u32 signature_;
        s32 uncompressedSize_;
        s32 compressedSize_;
    };

    struct BlobPackInfo
    {
        bool compressed_;
        s32 uncompressedSize_;
        s32 compressedSize_;
    };

    s32 packBlob(s32 dstSize, u8* dst, BlobRef& blob, bool compress);
    s32 unpackBlob(s32 dstSize, u8* dst, s32 srcSize, const u8* src);
    bool getPackBlobStatus(BlobPackInfo& info, s32 srcSize, const u8* src);
}
#endif //INC_LGRAPHICS_DX11_BLOBREF_H_
