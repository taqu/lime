/**
@file VolumeParticle01.cpp
@author t-sakai
@date 2016/12/23 create
*/
#include "VolumeParticle01.h"
#include <lcore/Random.h>
#include <lframework/resource/Resources.h>
#include <lframework/resource/ResourceTexture3D.h>
#include "lframework/System.h"
#include "lframework/Application.h"

namespace debug
{
    VolumeParticle01::VolumeParticle01()
    {
        hullConstant_.tesselationFactor_ = 16.0f;
        hullConstant_.minDistance_ = 0.1f;
        hullConstant_.invRange_ = 1.0f/(10.0f-0.1f);
        hullConstant_.tesselationMinFactor_ = 8.0f;
    }

    VolumeParticle01::~VolumeParticle01()
    {

    }

    void VolumeParticle01::onCreate()
    {
        ComponentVolumeParticleRenderer::onCreate();

        lcore::RandXorshift128Plus32 random(lcore::getDefaultSeed64());

        static const lcore::s32 capacity = 64;
        resize(capacity);
        lfw::VolumeParticleVertex vertex;
        vertex.position_ = lmath::Vector3::construct(1.0f, 0.5f, 0.25f);
        vertex.time_ = 0.0f;
        vertex.scale_ =  1.0f;
        vertex.radius_ = lcore::toFloat16(1.0f);
        vertex.frequencyScale_ = lcore::toFloat16(0.091f);
        add(vertex, lfw::Nothing());
        time_ = 0.0f;
    }

    void VolumeParticle01::onStart()
    {
    }

    void VolumeParticle01::update()
    {
        lfw::Timer& timer = lfw::System::getTimer();

        time_ += timer.getDeltaTime();
        if(1.0f<time_){
            time_ -= 1.0f;
        }
        for(lfw::s32 i=0; i<size(); ++i){
            lfw::VolumeParticleVertex& vertex = getFirst(i);
            vertex.time_ = time_;
            //vertex.scale_ = lmath::sinf_fast(time_);
        }
    }

    void VolumeParticle01::postUpdate()
    {
    }

    void VolumeParticle01::onDestroy()
    {
    }

    void VolumeParticle01::setShaders()
    {
        lfw::Resources& resources = lfw::System::getResources();
        ds_ = resources.getShaderManager().getDS(lfw::ShaderDS_VolumeParticle);
        hs_ = resources.getShaderManager().getHS(lfw::ShaderHS_VolumeParticle);
        vs_ = resources.getShaderManager().getVS(lfw::ShaderVS_VolumeParticle);
        ps_ = resources.getShaderManager().getPS(lfw::ShaderPS_VolumeSphere);
    }

    void VolumeParticle01::createResources()
    {
        lfw::s32 setID = 0;
        lfw::ResourceTexture3D* texNoise = lfw::System::getResources().load(setID, "LDR_RGBA_3D.dds", lfw::ResourceType::ResourceType_Texture3D, lfw::TextureParameter::NoSRGB_)->cast<lfw::ResourceTexture3D>();
        srvNoise_ = texNoise->getShaderResourceView();
    }

    void VolumeParticle01::setupResources(lfw::RenderContext& renderContext)
    {
        renderContext.getContext().setPSResources(0, 1, srvNoise_);
    }
}
