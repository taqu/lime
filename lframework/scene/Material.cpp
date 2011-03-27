/**
@file Material.cpp
@author t-sakai
@date 2009/08/11
*/
#include "Material.h"
#include "ShaderBase.h"

namespace lscene
{
    Material::Material()
        :materialFlags_(0)
        ,textures_(0)
    {
    }

    Material::~Material()
    {
    }

    void Material::setTextureNum(u32 num)
    {
        releaseTextures();

        TextureArray texs(num);
        textures_.swap(texs);
    }

    void Material::swap(Material& rhs)
    {
        shading_.swap(rhs.shading_);

        diffuse_.swap(rhs.diffuse_);
        specular_.swap(rhs.specular_);
        ambient_.swap(rhs.ambient_);
        emissive_.swap(rhs.emissive_);
        transparent_.swap(rhs.transparent_);

        lcore::swap(shininess_, rhs.shininess_);
        lcore::swap(shininessStrength_, rhs.shininessStrength_);
        lcore::swap(opacity_, rhs.opacity_);
        lcore::swap(refraction_, rhs.refraction_);
        materialFlags_.swap(rhs.materialFlags_);

        textures_.swap(rhs.textures_);
        
        renderState_.swap(rhs.renderState_);
    }

    void Material::releaseTextures()
    {
        for(TextureArray::iterator itr = textures_.begin();
            itr != textures_.end();
            ++itr)
        {
            (*itr).texture_.destroy();
        }
    }

    Material& Material::operator=(const Material& rhs)
    {
        shading_ = rhs.shading_;

        diffuse_ = rhs.diffuse_;
        specular_ = rhs.specular_;
        ambient_ = rhs.ambient_;
        emissive_ = rhs.emissive_;
        transparent_ = rhs.transparent_;

        shininess_ = rhs.shininess_;
        shininessStrength_ = rhs.shininessStrength_;
        opacity_ = rhs.opacity_;
        refraction_ = rhs.refraction_;
        materialFlags_ = rhs.materialFlags_;

        setTextureNum(rhs.getTextureNum());

        for(u32 i=0; i<textures_.size(); ++i){
            textures_[i].state_ = rhs.textures_[i].state_;
            textures_[i].texture_ = rhs.textures_[i].texture_;
        }

        renderState_ = rhs.renderState_;

        return *this;
    }
}