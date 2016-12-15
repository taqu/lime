/**
@file BlobRef.cpp
@author t-sakai
@date 2012/07/21 create
*/
#include "BlobRef.h"
#include <lz4/lz4.h>

namespace lgfx
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

    BlobRef::BlobRef(pointer_type blob)
        :blob_(blob)
    {
    }

    BlobRef::~BlobRef()
    {
        LDXSAFE_RELEASE(blob_);
    }

    void* BlobRef::getPointer()
    {
        return blob_->GetBufferPointer();
    }

    lsize_t BlobRef::getSize()
    {
        return static_cast<lsize_t>(blob_->GetBufferSize());
    }

    s32 packBlob(s32 dstSize, u8* dst, BlobRef& blob, bool compress)
    {
        LASSERT(0<=dstSize);
        LASSERT(NULL != dst);
        s32 size = static_cast<s32>(blob.getSize());
        if(dstSize<(size+sizeof(BlobPack))){
            return 0;
        }
        BlobPack header = {0,0,0};
        u8* buffer = dst + sizeof(BlobPack);
        if(compress){
            header.signature_ = Signature_BlobCompressed;
            header.uncompressedSize_ = size;
            header.compressedSize_ = LZ4_compress_default(
                static_cast<const Char*>(blob.getPointer()),
                reinterpret_cast<Char*>(buffer),
                size,
                dstSize);
        } else{
            header.signature_ = Signature_BlobUncompressed;
            header.uncompressedSize_ = size;
            header.compressedSize_ = size;
            lcore::memcpy(buffer, blob.getPointer(), size);
        }
        lcore::memcpy(dst, &header, sizeof(BlobPack));
        return (header.compressedSize_<=0)? 0 : header.compressedSize_+sizeof(BlobPack);
    }

    s32 unpackBlob(s32 dstSize, u8* dst, s32 srcSize, const u8* src)
    {
        LASSERT(0<=dstSize);
        LASSERT(NULL != dst);
        LASSERT(0<=srcSize);
        LASSERT(NULL != src);
        LASSERT(sizeof(BlobPack)<=srcSize);
        if(dstSize<srcSize){
            return 0;
        }
        BlobPack header;
        lcore::memcpy(&header, src, sizeof(BlobPack));
        src += sizeof(BlobPack);
        srcSize -= sizeof(BlobPack);
        s32 size = 0;
        switch(header.signature_)
        {
        case Signature_BlobUncompressed:
            if(srcSize<header.uncompressedSize_){
                return 0;
            }
            lcore::memcpy(dst, src, header.uncompressedSize_);
            size = header.uncompressedSize_;
            break;
        case Signature_BlobCompressed:
            if(srcSize<header.compressedSize_){
                return 0;
            }
            size = LZ4_decompress_safe(
                reinterpret_cast<const Char*>(src),
                reinterpret_cast<Char*>(dst),
                header.compressedSize_,
                dstSize);
            if(size<header.uncompressedSize_){
                return 0;
            }
            break;
        default:
            break;
        }
        return size;
    }

    bool getPackBlobStatus(BlobPackInfo& info, s32 srcSize, const u8* src)
    {
        LASSERT(0<=srcSize);
        LASSERT(NULL != src);
        if(srcSize<sizeof(BlobPack)){
            return false;
        }
        const BlobPack* blobPack = reinterpret_cast<const BlobPack*>(src);
        info.uncompressedSize_ = blobPack->uncompressedSize_;
        info.compressedSize_ = blobPack->compressedSize_;
        switch(blobPack->signature_)
        {
        case Signature_BlobUncompressed:
            info.compressed_ = false;
            break;
        case Signature_BlobCompressed:
            info.compressed_ = true;
            break;
        default:
            return false;
        }
        return true;
    }
}
