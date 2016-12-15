#ifndef INC_LFRAMEWORK_LOADMATERIAL_H__
#define INC_LFRAMEWORK_LOADMATERIAL_H__
/**
@file load_material.h
@author t-sakai
@date 2016/11/23 create
*/
#include "load.h"
#include <lmath/Vector4.h>

namespace lfw
{
    class LoadMaterial
    {
    public:
#ifdef LNPBR
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
        u8 blendEnable_;
        u8 srcBlend_;
        u8 dstBlend_;
        u8 blendOp_;
        s8 textureIDs_[TextureType_Num];
    };
}
#endif //INC_LFRAMEWORK_LOADMATERIAL_H__
