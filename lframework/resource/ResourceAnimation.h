#ifndef INC_LFRAMEWORK_RESOURCEANIMATION_H__
#define INC_LFRAMEWORK_RESOURCEANIMATION_H__
/**
@file ResourceAnimation.h
@author t-sakai
@date 2016/11/22 create
*/
#include "Resource.h"

namespace lcore
{
    class FileProxy;
}

namespace lfw
{
    class ResourceAnimation : public Resource
    {
    public:
        static ResourceAnimation* load(const Char* path, s64 size, const u8* memory);

        virtual s32 getType() const
        {
            return ResourceType_Animation;
        }

    protected:
        ResourceAnimation(const ResourceAnimation&);
        ResourceAnimation& operator=(const ResourceAnimation&);

        friend class Loader;

        virtual ~ResourceAnimation()
        {
        }

    };
}
#endif //INC_LFRAMEWORK_RESOURCEANIMATION_H__
