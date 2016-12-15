#ifndef INC_LFRAMEWORK_RESOURCEBYTES_H__
#define INC_LFRAMEWORK_RESOURCEBYTES_H__
/**
@file ResourceBytes.h
@author t-sakai
@date 2016/11/21 create
*/
#include "Resource.h"

namespace lfw
{
    class ResourceBytes : public Resource
    {
    public:
        typedef lcore::intrusive_ptr<ResourceBytes> pointer;
        static const s32 Type = ResourceType_Bytes;

        inline static ResourceBytes* load(s64 size, const u8* memory);

        virtual s32 getType() const
        {
            return ResourceType_Bytes;
        }

        inline s32 getSize() const;
        inline const u8* getBytes() const;
    protected:
        ResourceBytes(const ResourceBytes&);
        ResourceBytes& operator=(const ResourceBytes&);

        ResourceBytes(s32 size, const u8* bytes)
            :size_(size)
            ,bytes_(bytes)
        {
            LASSERT(0<=size_);
            LASSERT(NULL != bytes_);
        }

        virtual ~ResourceBytes()
        {
            LDELETE_ARRAY(bytes_);
        }

        s32 size_;
        const u8* bytes_;
    };

    inline ResourceBytes* ResourceBytes::load(s64 size, const u8* memory)
    {
        LASSERT(0<=size);
        LASSERT(NULL != memory);
        return LNEW ResourceBytes(static_cast<s32>(size), memory);
    }

    inline s32 ResourceBytes::getSize() const
    {
        return size_;
    }

    inline const u8* ResourceBytes::getBytes() const
    {
        return bytes_;
    }
}
#endif //INC_LFRAMEWORK_RESOURCEBYTES_H__
