#ifndef INC_LSCENE_RESOURCE_H__
#define INC_LSCENE_RESOURCE_H__
/**
@file Resource.h
@author t-sakai
@date 2014/12/10 create
*/
#include "lscene.h"
#include "./mixin/ReferenceCounted.h"
#include <lgraphics/api/TextureRef.h>

namespace lscene
{
    class Resource : public mixin::ReferenceCounted<ResourceAllocator>
    {
    public:
        virtual s32 getType() const =0;

    protected:
        Resource(const Resource&);
        Resource& operator=(const Resource&);

        Resource()
        {}

        virtual ~Resource()
        {}
    };

    template<s32 Type>
    class ResourceTyped : public Resource
    {
    public:
        static const s32 type_id = Type;
        virtual s32 getType() const{ return type_id;}
    };

    class ResourceUnknown : public ResourceTyped<ResourceType_Unknown>
    {
    public:
        ResourceUnknown()
            :size_(0)
            ,data_(NULL)
        {
        }

        ResourceUnknown(s32 size, u8* data)
            :size_(size)
            ,data_(data)
        {
        }

        s32 getSize() const{ return size_;}
        const u8* getData() const{ return data_;}
    private:
        virtual ~ResourceUnknown()
        {
            LSCENE_FREE(data_);
        }

        s32 size_;
        u8* data_;
    };

    class ResourceTexture2D : public ResourceTyped<ResourceType_Texture2D>
    {
    public:
        ResourceTexture2D()
        {
        }

        ResourceTexture2D(lgraphics::Texture2DRef& texture)
            :texture_(texture)
        {
        }

        lgraphics::Texture2DRef& get() { return texture_;}
    private:
        virtual ~ResourceTexture2D()
        {
        }

        lgraphics::Texture2DRef texture_;
    };

    class ResourceTextureCube : public ResourceTyped<ResourceType_TextureCube>
    {
    public:
        ResourceTextureCube()
        {
        }

        ResourceTextureCube(lgraphics::Texture2DRef& texture)
            :texture_(texture)
        {
        }

        lgraphics::Texture2DRef& get() { return texture_;}
    private:
        virtual ~ResourceTextureCube()
        {
        }

        lgraphics::Texture2DRef texture_;
    };
}
#endif //INC_LSCENE_RESOURCE_H__
