#ifndef INC_LSCENE_LRENDER_MATERIAL_H__
#define INC_LSCENE_LRENDER_MATERIAL_H__
/**
@file Material.h
@author t-sakai
@date 2011/10/17 create
*/
#include <lmath/Vector4.h>

#include "render.h"

namespace lscene
{
namespace lrender
{
    class Material
    {
    public:
        enum Flag
        {
            Flag_CastShadow = 0x01U << 0,
            Flag_RecieveShadow = 0x01U << 1,
            Flag_RefractiveIndex = 0x01U << 2, //shadow_.w_ ‚ªrefractive index‚©

            Flag_Opaque = 0x01U<<16,
            Flag_Transparent = 0x01U<<17,
            Flag_TransparentAdd = 0x01U<<18,
        };

        Material();
        ~Material();

        inline bool checkFlag(Flag flag) const;
        inline void setFlag(Flag flag);
        inline void unsetFlag(Flag flag);
        inline bool isCastShadow() const;
        inline bool isRecieveShadow() const;
        inline bool isTransparent() const;

        inline bool hasTexture(Texture type) const;

        /// ƒXƒƒbƒv
        void swap(Material& rhs);

#ifdef LIME_NPBR
        lmath::Vector4 diffuse_; //rgba
        lmath::Vector4 specular_; //rgb shininess
        lmath::Vector4 ambient_; //rgb refractive index or fresnel
        lmath::Vector4 shadow_; //rgb roughness
#else
        lmath::Vector4 diffuse_; //rgba
        lmath::Vector4 specular_; //rgb roughness
        lmath::Vector4 ambient_; //rgb ambient shadow
        lmath::Vector4 shadow_; //rgb metalic
#endif

        u32 flags_;
        s16 textureIDs_[Tex_Num];
    };

    inline bool Material::checkFlag(Flag flag) const
    {
        return 0 != (flags_ & flag);
    }

    inline void Material::setFlag(Flag flag)
    {
        flags_ |= flag;
    }

    inline void Material::unsetFlag(Flag flag)
    {
        flags_ &= ~flag;
    }

    inline bool Material::isCastShadow() const
    {
        return 0 != (flags_ & Flag_CastShadow);
    }

    inline bool Material::isRecieveShadow() const
    {
        return 0 != (flags_ & Flag_RecieveShadow);
    }

    inline bool Material::isTransparent() const
    {
        return (flags_&Flag_Transparent) || (flags_&Flag_TransparentAdd);
    }

    inline bool Material::hasTexture(Texture type) const
    {
        return 0<=textureIDs_[type];
    }
}
}
#endif //INC_LSCENE_LRENDER_MATERIAL_H__
