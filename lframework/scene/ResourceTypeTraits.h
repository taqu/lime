#ifndef INC_LSCENE_RESOURCETYPETRAITS_H__
#define INC_LSCENE_RESOURCETYPETRAITS_H__
/**
@file ResourceTypeTraits.h
@author t-sakai
@date 2014/12/10 create
*/
#include "lscene.h"

namespace lgraphics
{
    class Texture2D;
}

namespace lscene
{
    class ResourceTypeTraitsUnknown : public ResourceTypeTraits
    {
    public:
        virtual s32 getType() const
        {
            return ResourceType_Unknown;
        }

        virtual void destroy(void* resource);
    };

    class ResourceTypeTraitsUnknown : public ResourceTypeTraits
    {
    public:
        virtual s32 getType() const
        {
            return ResourceType_Unknown;
        }

        virtual void destroy(void* resource);
    };

    class ResourceTypeTraitsUnknown : public ResourceTypeTraits
    {
    public:
        virtual s32 getType() const
        {
            return ResourceType_Unknown;
        }

        virtual void destroy(void* resource);
    };

    class ResourceTypeTraitsUnknown : public ResourceTypeTraits
    {
    public:
        virtual s32 getType() const
        {
            return ResourceType_Unknown;
        }

        virtual void destroy(void* resource);
    };
}
#endif //INC_LSCENE_RESOURCETYPETRAITS_H__
