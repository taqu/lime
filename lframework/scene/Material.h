#ifndef INC_LSCENE_MATERIAL_H__
#define INC_LSCENE_MATERIAL_H__
/**
@file Material.h
@author t-sakai
@date 2009/08/11 create
*/
#include "lscene.h"
#include <lmath/lmath.h>
#include <lcore/FixedArray.h>
#include <lcore/String.h>

#include <lgraphics/api/TextureRef.h>
#include <lgraphics/api/SamplerState.h>
#include <lgraphics/api/RenderStateRef.h>
#include "../Flags.h"

namespace lscene
{
    class Scene;

    class Material
    {
    public:
        static const u32 MAX_NAME_BUFFER_SIZE = 32;
        typedef lcore::String<MAX_NAME_BUFFER_SIZE> NameString;

        typedef lframework::Flags Flags;

        /// マテリアル機能フラグ
        enum MaterialFlag
        {
            MatFlag_TexAlbedo = (0x01U<<0),
            MatFlag_Texture1 = (0x01U<<1),
            MatFlag_TextureN = (0x01U<<2),
            MatFlag_TextureS = (0x01U<<3),
            MatFlag_DiffuseVS = (0x01U<<4),
            MatFlag_Fresnel = (0x01U<<5),
            MatFlag_TexGrad = (0x01U<<6),
            MatFlag_Emissive = (0x01U<<7),
            MatFlag_AlphaTest = (0x01U<<8),
            MatFlag_LightingVS = (0x01U<<9),
            MatFlag_Valid = (0x01U<<31),
            MatFlag_Num = 5,
        };

        Material();
        ~Material();

        void setTextureNum(u32 num);
        inline u32 getTextureNum() const;


        inline lgraphics::TextureRef& getTexture(u32 index);
        inline const lgraphics::TextureRef& getTexture(u32 index) const;
        inline void setTexture(u32 index, const lgraphics::TextureRef& texture);

        inline const lgraphics::SamplerState& getSamplerState(u32 index) const;
        inline lgraphics::SamplerState& getSamplerState(u32 index);
        inline void setSamplerState(u32 index, const lgraphics::SamplerState& state);

        inline lgraphics::RenderStateRef& getRenderState();
        inline const lgraphics::RenderStateRef& getRenderState() const;
        inline void setRenderState(const lgraphics::RenderStateRef& state);

        inline void applyRenderState();

        void swap(Material& rhs);

        inline const Flags& getFlags() const;
        inline Flags& getFlags();

        inline u32 getShaderID() const;
        inline void setShaderID(u32 shaderID);

        Material& operator=(const Material& rhs);

    //private:
        lmath::Vector4 diffuse_;
        lmath::Vector4 specular_;
        lmath::Vector3 ambient_;
        lmath::Vector3 emissive_;
        //lmath::Vector4 transparent_;
        //f32 opacity_;
        //f32 refraction_;
        f32 reflectance_;
        Flags materialFlags_;
        u32 shaderID_;
    private:
        void releaseTextures();

        struct TextureHandleSet
        {
            TextureHandleSet()
            {
            }

            lgraphics::TextureRef texture_;
            lgraphics::SamplerState state_;
        };

        typedef lcore::FixedArray<TextureHandleSet> TextureArray;
        TextureArray textures_;
        
        lgraphics::RenderStateRef renderState_;
    };


    //--------------------------------------------------------
    //---
    //--- inline実装
    //---
    //--------------------------------------------------------
    inline u32 Material::getTextureNum() const
    {
        return textures_.size();
    }

    inline lgraphics::TextureRef& Material::getTexture(u32 index)
    {
        return textures_[index].texture_;
    }

    inline const lgraphics::TextureRef& Material::getTexture(u32 index) const
    {
        return textures_[index].texture_;
    }

    inline void Material::setTexture(u32 index, const lgraphics::TextureRef& texture)
    {
        textures_[index].texture_ = texture;
    }

    inline const lgraphics::SamplerState& Material::getSamplerState(u32 index) const
    {
        return textures_[index].state_;
    }

    inline lgraphics::SamplerState& Material::getSamplerState(u32 index)
    {
        return textures_[index].state_;
    }


    inline void Material::setSamplerState(u32 index, const lgraphics::SamplerState& state)
    {
        textures_[index].state_ = state;
    }


    inline lgraphics::RenderStateRef& Material::getRenderState()
    {
        return renderState_;
    }

    inline const lgraphics::RenderStateRef& Material::getRenderState() const
    {
        return renderState_;
    }

    inline void Material::setRenderState(const lgraphics::RenderStateRef& state)
    {
        renderState_ = state;
    }

    inline void Material::applyRenderState()
    {
        renderState_.apply();
    }


    inline const Material::Flags& Material::getFlags() const
    {
        return materialFlags_;
    }

    inline Material::Flags& Material::getFlags()
    {
        return materialFlags_;
    }

    inline u32 Material::getShaderID() const
    {
        return shaderID_;
    }

    inline void Material::setShaderID(u32 shaderID)
    {
        shaderID_ = shaderID;
    }
}

#endif //INC_LSCENE_MATERIAL_H__
