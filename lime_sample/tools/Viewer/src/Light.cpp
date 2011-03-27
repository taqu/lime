/**
@file Light.cpp
@author t-sakai
@date 2011/02/26
*/
#include "stdafx.h"
#include "Light.h"

#include <lframework/System.h>
#include <lframework/render/RenderingSystem.h>
#include <lframework/scene/Scene.h>

#include <PmmDef.h>

using namespace lmath;

namespace viewer
{
    namespace
    {
        inline void set(lmath::Vector4& dst, const u8* c0)
        {
            static const f32 ratio = (1.0f/255.0f);

            dst._x = ratio * c0[0];
            dst._y = ratio * c0[1];
            dst._z = ratio * c0[2];
        }

        inline void lerp(lmath::Vector4& dst, const u8* c0, const u8* c1, f32 blend)
        {
            blend *= (1.0f/255.0f);
            f32 b2= (1.0f/255.0f) - blend;
            dst._x = b2 * c0[0] + blend * c1[0];
            dst._y = b2 * c0[1] + blend * c1[1];
            dst._z = b2 * c0[2] + blend * c1[2];
        }
    }

    Light::Light()
        :frame_(0)
        ,lastFrame_(0)
        ,lightAnimPack_(NULL)
    {
    }

    Light::~Light()
    {
    }

    void Light::update()
    {
        LASSERT(lightAnimPack_ != NULL);

        if(++frame_ > lastFrame_){
            frame_ = 0;
        }

        lscene::Scene& scene = lframework::System::getRenderSys().getScene();
        lscene::LightEnvironment& lightEnv = scene.getLightEnv();
        lscene::DirectionalLight& dlight = lightEnv.getDirectionalLight();

        u32 animIndex = lightAnimPack_->binarySearchIndex(frame_);
        const pmm::LightPose& lightPose = lightAnimPack_->getPose(animIndex);

        if(animIndex == lightAnimPack_->getNumPoses() - 1){
            //最後のフレーム
            dlight.setDirection(lightPose.direction_);
            set(dlight.getColor(), lightPose.rgbx_);

        }else{
            //補間する

            //次のフレームのポーズ
            const pmm::LightPose& nextPose = lightAnimPack_->getPose(animIndex + 1);

            f32 blend = (frame_ - static_cast<f32>(lightPose.frameNo_));
            blend /= static_cast<f32>(nextPose.frameNo_ - lightPose.frameNo_);

            dlight.getDirection().lerp(lightPose.direction_, nextPose.direction_, blend);

            lerp(dlight.getColor(), lightPose.rgbx_, nextPose.rgbx_, blend);
        } //if(animIndex ==
    }

    void Light::setLightAnim(pmm::LightAnimPack* pack, u32 lastFrame)
    {
        LASSERT(pack != NULL);
        lastFrame_ = lastFrame;
        lightAnimPack_ = pack;
        reset();
    }

    void Light::reset()
    {
        LASSERT(lightAnimPack_ != NULL);

        frame_ = 0;

        lscene::Scene& scene = lframework::System::getRenderSys().getScene();
        lscene::LightEnvironment& lightEnv = scene.getLightEnv();
        lscene::DirectionalLight& dlight = lightEnv.getDirectionalLight();

        const pmm::LightPose& pose = lightAnimPack_->getPose(frame_);
        dlight.setDirection( pose.direction_ );
        set(dlight.getColor(), pose.rgbx_);
    }
}
