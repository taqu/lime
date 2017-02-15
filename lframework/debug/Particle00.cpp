/**
@file Particle00.cpp
@author t-sakai
@date 2016/12/23 create
*/
#include "Particle00.h"
#include <lcore/Random.h>
#include <lframework/resource/Resources.h>
#include <lframework/resource/ResourceTexture2D.h>

namespace debug
{
    Particle00::Particle00()
    {

    }

    Particle00::~Particle00()
    {

    }

    void Particle00::onCreate()
    {
        ComponentParticleRenderer::onCreate();

        lcore::RandXorshift128Plus32 random(lcore::getDefaultSeed64());

        resize(capacity);

        lfw::ParticleVertex vertex0;
        Particle00Vertex vertex1;
        for(lcore::s32 i=0; i<capacity; ++i){
            vertex1.index_ = i;
            vertex1.time_ = 0.5f;

            vertex0.position_.x_ = static_cast<lfw::f32>(i)/capacity*2.0f - 1.0f;
            vertex0.position_.y_ = 0.0f;
            vertex0.position_.z_ = -0.5f;

            vertex0.texcoord_[0] = lcore::toBinary16Float(0.0f);
            vertex0.texcoord_[1] = lcore::toBinary16Float(0.0f);
            vertex0.texcoord_[2] = lcore::toBinary16Float(1.0f);
            vertex0.texcoord_[3] = lcore::toBinary16Float(1.0f);
            vertex0.abgr_ = lcore::getABGR(128, 0, 0, 255);
            vertex0.size_[0] = lcore::toBinary16Float(0.2f);
            vertex0.size_[1] = lcore::toBinary16Float(0.2f);
            vertex0.rotation_[0] = lcore::floatTo16SNORM(1.0f);
            vertex0.rotation_[1] = lcore::floatTo16SNORM(0.0f);
            add(vertex0, vertex1);
        }

        lfw::ResourceTexture2D* texWhite = lfw::Resources::getInstance().getEmptyTextureWhite()->cast<lfw::ResourceTexture2D>();
        setTexture(texWhite->get(), texWhite->getShaderResourceView());
    }

    void Particle00::onStart()
    {
    }

    void Particle00::update()
    {
    }

    void Particle00::postUpdate()
    {
    }

    void Particle00::onDestroy()
    {
    }
}
