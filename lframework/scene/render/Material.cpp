/**
@file Material.cpp
@author t-sakai
@date 2011/10/17 create
*/
#include "Material.h"

namespace lscene
{
namespace lrender
{
    Material::Material()
        :flags_(0)
    {
        for(u32 i=0; i<Tex_Num; ++i){
            textureIDs_[i] = -1;
        }
    }

    Material::~Material()
    {
    }

    void Material::swap(Material& rhs)
    {
        lcore::swap(flags_, rhs.flags_);
        diffuse_.swap(rhs.diffuse_);
        specular_.swap(rhs.specular_);
        ambient_.swap(rhs.ambient_);
        shadow_.swap(rhs.shadow_);

        for(u32 i=0; i<Tex_Num; ++i){
            lcore::swap(textureIDs_[i], rhs.textureIDs_[i]);
        }
    }
}
}
