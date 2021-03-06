#ifndef INC_LFRAMEWORK_RESOURCE_H_
#define INC_LFRAMEWORK_RESOURCE_H_
/**
@file Resource.h
@author t-sakai
@date 2016/11/16 create
*/
#include "../lframework.h"
#include <lcore/intrusive_ptr.h>

namespace lfw
{
    struct TextureParameter
    {
        static const u8 Flag_sRGB = 0x01<<0;
        static const u8 Flag_Sampler = 0x01<<1;

        inline bool isSRGB() const
        {
            return 0 != (flags_&Flag_sRGB);
        }

        inline bool isSampler() const
        {
            return 0 != (flags_&Flag_Sampler);
        }

        static const TextureParameter SRGB_;
        static const TextureParameter NoSRGB_;
        static const TextureParameter SRGBSampler;
        static const TextureParameter NoSRGBSampler;

        u8 flags_;
        u8 filterType_;
        u8 addressUVW_;
        u8 compFunc_;
        f32 borderColor_;
    };

    class Resource
    {
    public:
        typedef lcore::intrusive_ptr<Resource> pointer;

        virtual s32 getType() const =0;
        inline s32 getReferenceCount() const;
        inline u64 getID() const;
        inline void setID(u64 id);

        template<class T>
        inline T* cast()
        {
            return (T::Type == getType())? reinterpret_cast<T*>(this) : NULL;
        }
    protected:
        Resource(const Resource&);
        Resource& operator=(const Resource&);

        friend void intrusive_ptr_addref(Resource* resource);
        friend void intrusive_ptr_release(Resource* resource);

        Resource()
            :referenceCount_(0)
            ,id_(0)
        {}

        virtual ~Resource()
        {
            referenceCount_ = 0;
        }

        s32 referenceCount_;
        u64 id_;
    };

    inline s32 Resource::getReferenceCount() const
    {
        return referenceCount_;
    }

    inline u64 Resource::getID() const
    {
        return id_;
    }

    inline void Resource::setID(u64 id)
    {
        id_ = id;
    }

    inline void intrusive_ptr_addref(Resource* resource)
    {
        ++resource->referenceCount_;
    }

    inline void intrusive_ptr_release(Resource* resource)
    {
        --resource->referenceCount_;
        if(0 == resource->referenceCount_){
            LDELETE_RAW(resource);
        }
    }
}
#endif //INC_LFRAMEWORK_RESOURCE_H_
