/**
@file IOMaterial.cpp
@author t-sakai
@date 2010/05/06 create

*/
#include "IOMaterial.h"

#include <lgraphics/api/ShaderKey.h>
#include <lgraphics/io/IOShaderKey.h>
#include <lgraphics/io/IORenderState.h>
#include <lgraphics/io/IOTexture.h>
#include <lgraphics/io/IOSamplerState.h>

#include "../IOUtil.h"
#include "../SectionID.h"
#include "Material.h"
#include "ShaderBase.h"

using namespace lframework;

namespace lscene
{
    namespace
    {
        struct MaterialHeader
        {
            lmath::Vector4 diffuse_;
            lmath::Vector4 specular_;
            lmath::Vector3 ambient_;
            lmath::Vector3 emissive_;
            //lmath::Vector4 transparent_;
            //f32 opacity_;
            //f32 refraction_;
            f32 reflectance_;
            u32 flags_;
            u32 shaderID_;
            u32 numTextures_;
        };
    }

    bool IOMaterial::read(lcore::istream& is, Material& material)
    {
        u32 materialID = 0;

        lcore::io::read(is, materialID);

        MaterialHeader header;
        lcore::io::read(is, header);

        material.diffuse_ = header.diffuse_;
        material.specular_ = header.specular_;
        material.ambient_ = header.ambient_;
        material.emissive_ = header.emissive_;
        //material.transparent_ = header.transparent_;
        //material.opacity_ = header.opacity_;
        //material.refraction_ = header.refraction_;
        material.reflectance_ = header.reflectance_;
        material.getFlags().set(header.flags_);
        material.setShaderID( header.shaderID_ );


        lgraphics::RenderStateRef& state = material.getRenderState();
        lgraphics::io::IORenderState::read(is, state);

        material.setTextureNum(header.numTextures_);
        for(u32 i=0; i<header.numTextures_; ++i){
            lgraphics::SamplerState state;
            lgraphics::io::IOSamplerState::read(is, state );
            material.setSamplerState(i, state);
        }

        return true;
    }

    bool IOMaterial::write(lcore::ostream& os, Material& material)
    {
        u32 materialID = MaterialID;

        lcore::io::write(os, materialID);

        MaterialHeader header;
        header.diffuse_ = material.diffuse_;
        header.specular_ = material.specular_;
        header.ambient_ = material.ambient_;
        header.emissive_ = material.emissive_;
        //header.transparent_ = material.transparent_;
        //header.opacity_ = material.opacity_;
        //header.refraction_ = material.refraction_;
        header.reflectance_ = material.reflectance_;
        header.flags_ = material.getFlags().get();
        header.shaderID_ = material.getShaderID();
        header.numTextures_ = material.getTextureNum();

        lcore::io::write(os, header);

        lgraphics::RenderStateRef& state = material.getRenderState();
        lgraphics::io::IORenderState::write(os, state);

        for(u32 i=0; i<header.numTextures_; ++i){
            lgraphics::io::IOSamplerState::write(os, material.getSamplerState(i) );
        }

        return true;
    }
}
