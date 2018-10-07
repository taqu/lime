#ifndef INC_LFRAMEWORK_RESOURCETEXTURE3D_H_
#define INC_LFRAMEWORK_RESOURCETEXTURE3D_H_
/**
@file ResourceTexture3D.h
@author t-sakai
@date 2017/07/29 create
*/
#include "Resource.h"
#include <lgraphics/TextureRef.h>

namespace lfw
{
    class ResourceTexture3D : public Resource
    {
    public:
        typedef lcore::intrusive_ptr<ResourceTexture3D> pointer;
        static const s32 Type = ResourceType_Texture3D;

        static ResourceTexture3D* load(const Char* path, s64 size, const u8* memory, const TextureParameter& param);
        static ResourceTexture3D* create(u32 color, const TextureParameter& param);
        static ResourceTexture3D* create(u32 width, u32 height, u32 depth, const u32* colors, const TextureParameter& param);

        virtual s32 getType() const
        {
            return ResourceType_Texture3D;
        }

        inline lgfx::Texture3DRef& get() ;
        inline lgfx::SamplerStateRef& getSampler();
        inline lgfx::ShaderResourceViewRef& getShaderResourceView();

    protected:
        ResourceTexture3D(const ResourceTexture3D&);
        ResourceTexture3D& operator=(const ResourceTexture3D&);

        ResourceTexture3D(lgfx::Texture3DRef& texture, lgfx::SamplerStateRef& sampler, lgfx::ShaderResourceViewRef& srv)
            :texture_(texture)
            ,sampler_(sampler)
            ,srv_(srv)
        {
        }

        virtual ~ResourceTexture3D()
        {
        }

        lgfx::Texture3DRef texture_;
        lgfx::SamplerStateRef sampler_;
        lgfx::ShaderResourceViewRef srv_;
    };

    inline lgfx::Texture3DRef& ResourceTexture3D::get()
    {
        return texture_;
    }

    inline lgfx::SamplerStateRef& ResourceTexture3D::getSampler()
    {
        return sampler_;
    }

    inline lgfx::ShaderResourceViewRef& ResourceTexture3D::getShaderResourceView()
    {
        return srv_;
    }

}
#endif //INC_LFRAMEWORK_RESOURCETEXTURE3D_H_
