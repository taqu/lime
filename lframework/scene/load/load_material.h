#ifndef INC_LSCENE_LLOAD_MATERIAL_H__
#define INC_LSCENE_LLOAD_MATERIAL_H__
/**
@file load_material.h
@author t-sakai
@date 2012/04/01 create
*/
#include "load.h"
#include <lmath/Vector4.h>

namespace lscene
{
namespace lload
{
    class Material
    {
    public:
        enum Flag
        {
            Flag_CastShadow = 0x01U << 0,
            Flag_RecieveShadow = 0x01U << 1,
            Flag_RefractiveIndex = 0x01U << 2, //shadow_.w_ ‚ªrefractive index‚©
        };

#ifdef LIME_NPBR
        lmath::Vector4 diffuse_; //rgba
        lmath::Vector4 specular_; //rgb shininess
        lmath::Vector4 ambient_; //rgb refractive refractive index of fresnel
        lmath::Vector4 shadow_; //rgb roughness
#else
        lmath::Vector4 diffuse_; //rgba
        lmath::Vector4 specular_; //rgb roughness
        lmath::Vector4 ambient_; //rgb ambient refractive index of fresnel
        lmath::Vector4 shadow_; //rgb metalic
#endif
        u32 flags_;

        s16 texColor_;
        s16 texNormal_;
    };
}
}
#endif //INC_LSCENE_LLOAD_MATERIAL_H__
