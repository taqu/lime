#ifndef INC_LFRAMEWORK_RESOURCEANIMATIONCLIP_H_
#define INC_LFRAMEWORK_RESOURCEANIMATIONCLIP_H_
/**
@file ResourceAnimationClip.h
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
    class ResourceAnimationClip : public Resource
    {
    public:
        static ResourceAnimationClip* load(const Char* path, s64 size, const u8* memory);

        virtual s32 getType() const
        {
            return ResourceType_Animation;
        }

    protected:
        ResourceAnimationClip(const ResourceAnimationClip&);
        ResourceAnimationClip& operator=(const ResourceAnimationClip&);

        virtual ~ResourceAnimationClip()
        {
        }

    };
}
#endif //INC_LFRAMEWORK_RESOURCEANIMATIONCLIP_H_
