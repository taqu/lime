#ifndef INC_LFRAMEWORK_RESOURCETEXTURE2D_H__
#define INC_LFRAMEWORK_RESOURCETEXTURE2D_H__
/**
@file ResourceTexture2D.h
@author t-sakai
@date 2016/11/21 create
*/
#include "Resource.h"
#include <lgraphics/TextureRef.h>

namespace lfw
{
    class ResourceTexture2D : public Resource
    {
    public:
        typedef lcore::intrusive_ptr<ResourceTexture2D> pointer;
        static const s32 Type = ResourceType_Texture2D;

        static ResourceTexture2D* load(const Char* path, s64 size, const u8* memory, const TextureParameter& param);

        virtual s32 getType() const
        {
            return ResourceType_Texture2D;
        }

        inline lgfx::Texture2DRef& get() ;
        inline lgfx::SamplerStateRef& getSampler();
        inline lgfx::ShaderResourceViewRef& getShaderResourceView();

    protected:
        ResourceTexture2D(const ResourceTexture2D&);
        ResourceTexture2D& operator=(const ResourceTexture2D&);

        ResourceTexture2D(lgfx::Texture2DRef& texture, lgfx::SamplerStateRef& sampler, lgfx::ShaderResourceViewRef& srv)
            :texture_(texture)
            ,sampler_(sampler)
            ,srv_(srv)
        {
        }

        virtual ~ResourceTexture2D()
        {
        }

        lgfx::Texture2DRef texture_;
        lgfx::SamplerStateRef sampler_;
        lgfx::ShaderResourceViewRef srv_;
    };

    inline lgfx::Texture2DRef& ResourceTexture2D::get()
    {
        return texture_;
    }

    inline lgfx::SamplerStateRef& ResourceTexture2D::getSampler()
    {
        return sampler_;
    }

    inline lgfx::ShaderResourceViewRef& ResourceTexture2D::getShaderResourceView()
    {
        return srv_;
    }

}
#endif //INC_LFRAMEWORK_RESOURCETEXTURE2D_H__
