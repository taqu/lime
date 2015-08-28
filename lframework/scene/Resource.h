#ifndef INC_LSCENE_RESOURCE_H__
#define INC_LSCENE_RESOURCE_H__
/**
@file Resource.h
@author t-sakai
@date 2014/12/10 create
*/
#include "lscene.h"
#include "./mixin/ReferenceCounted.h"
#include "./render/Texture.h"

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

        ResourceTexture2D(lrender::Texture2D& texture)
            :texture_(texture)
        {
        }

        lrender::Texture2D& get() { return texture_;}
    private:
        virtual ~ResourceTexture2D()
        {
        }

        lrender::Texture2D texture_;
    };

    class ResourceTexture3D : public ResourceTyped<ResourceType_Texture3D>
    {
    public:
        ResourceTexture3D()
        {
        }

        ResourceTexture3D(lrender::Texture3D& texture)
            :texture_(texture)
        {
        }

        lrender::Texture3D& get() { return texture_;}
    private:
        virtual ~ResourceTexture3D()
        {
        }

        lrender::Texture3D texture_;
    };

    class ResourceTextureCube : public ResourceTyped<ResourceType_TextureCube>
    {
    public:
        ResourceTextureCube()
        {
        }

        ResourceTextureCube(lrender::Texture2D& texture)
            :texture_(texture)
        {
        }

        lrender::Texture2D& get() { return texture_;}
    private:
        virtual ~ResourceTextureCube()
        {
        }

        lrender::Texture2D texture_;
    };
}
#endif //INC_LSCENE_RESOURCE_H__
