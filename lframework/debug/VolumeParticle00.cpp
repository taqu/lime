/**
@file VolumeParticle00.cpp
@author t-sakai
@date 2016/12/23 create
*/
#include "VolumeParticle00.h"
#include <lcore/Random.h>
#include <lframework/resource/Resources.h>
#include <lframework/resource/ResourceTexture3D.h>
#include "lframework/System.h"
#include "lframework/Application.h"

namespace debug
{
    VolumeParticle00::VolumeParticle00()
    {

    }

    VolumeParticle00::~VolumeParticle00()
    {

    }

    void VolumeParticle00::onCreate()
    {
        ComponentVolumeParticleRenderer::onCreate();

        lcore::RandXorshift128Plus32 random(lcore::getDefaultSeed64());

        static const lcore::s32 capacity = 64;
        resize(capacity);
        lfw::VolumeParticleVertex vertex;
        vertex.position_ = lmath::Vector3::construct(0.0f, 2.0f, -0.5f);
        vertex.time_ = 0.0f;
        vertex.scale_ =  0.0f;
        vertex.radius_ = lcore::toBinary16Float(1.0f);
        vertex.frequencyScale_ = lcore::toBinary16Float(0.091f);
        add(vertex, lfw::Nothing());
        time_ = 0.0f;
    }

    void VolumeParticle00::onStart()
    {
    }

    void VolumeParticle00::update()
    {
        lfw::Timer& timer = lfw::System::getTimer();

        time_ += timer.getDeltaTime();
        if(1.0f<time_){
            time_ -= 1.0f;
        }
        for(lfw::s32 i=0; i<size(); ++i){
            lfw::VolumeParticleVertex& vertex = getFirst(i);
            vertex.time_ = time_;
            vertex.scale_ = lmath::sinf_fast(time_);
        }
    }

    void VolumeParticle00::postUpdate()
    {
    }

    void VolumeParticle00::onDestroy()
    {
    }

    void VolumeParticle00::createResources()
    {
        lfw::s32 setID = 0;
        lfw::ResourceTexture3D* texNoise = lfw::System::getResources().load(setID, "LDR_RGBA_3D.dds", lfw::ResourceType::ResourceType_Texture3D, lfw::TextureParameter::NoSRGB_)->cast<lfw::ResourceTexture3D>();
        srvNoise_ = texNoise->getShaderResourceView();
    }

    void VolumeParticle00::setupResources(lfw::RenderContext& renderContext)
    {
        renderContext.getContext().setPSResources(0, 1, srvNoise_);
    }
}
