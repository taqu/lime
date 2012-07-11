/**
@file Light.cpp
@author t-sakai
@date 2011/02/26
@date 2011/04/07 フレーム検索方法を汎用アルゴリズムから一時変更
*/
#include "stdafx.h"
#include "Light.h"

#include "System.h"
#include "render/RenderingSystem.h"
#include <lframework/scene/Scene.h>

#include "libConverter/PmmDef.h"

using namespace lmath;

namespace viewer
{
    namespace
    {
        inline void set(lmath::Vector4& dst, const u8* c0)
        {
            static const f32 ratio = (1.0f/255.0f);

            dst.x_ = ratio * c0[0];
            dst.y_ = ratio * c0[1];
            dst.z_ = ratio * c0[2];
        }

        inline void lerp(lmath::Vector4& dst, const u8* c0, const u8* c1, f32 blend)
        {
            blend *= (1.0f/255.0f);
            f32 b2= (1.0f/255.0f) - blend;
            dst.x_ = b2 * c0[0] + blend * c1[0];
            dst.y_ = b2 * c0[1] + blend * c1[1];
            dst.z_ = b2 * c0[2] + blend * c1[2];
        }
    }

    Light::Light()
        :lightAnimPack_(NULL)
    {
    }

    Light::~Light()
    {
    }

    void Light::initialize()
    {
        lightAnimPack_->initialize();
    }

    void Light::update(u32 frame)
    {
        LASSERT(lightAnimPack_ != NULL);

        lscene::Scene& scene = lframework::System::getRenderSys().getScene();
        lscene::LightEnvironment& lightEnv = scene.getLightEnv();
        lscene::DirectionalLight& dlight = lightEnv.getDirectionalLight();

        u32 animIndex = lightAnimPack_->binarySearchIndex(frame);
        //u32 animIndex = lightAnimPack_->getNextIndex(frame);
        const pmm::LightPose& lightPose = lightAnimPack_->getPose(animIndex);

        if(animIndex == lightAnimPack_->getNumPoses() - 1){
            //最後のフレーム
            lmath::Vector4 dir(lightPose.direction_);
            dlight.setDirection(dir);
            set(dlight.getColor(), lightPose.rgbx_);

        }else{
            //補間する

            //次のフレームのポーズ
            const pmm::LightPose& nextPose = lightAnimPack_->getPose(animIndex + 1);

            f32 blend0 = static_cast<f32>(frame - lightPose.frameNo_);
            blend0 /= static_cast<f32>(nextPose.frameNo_ - lightPose.frameNo_);
            f32 blend1 = 1.0f - blend0;

            lmath::Vector3 blendDir;
            blendDir.lerp(lightPose.direction_, nextPose.direction_, blend0, blend1);
            lmath::Vector4 dir(blendDir);
            dlight.setDirection(dir);

            lerp(dlight.getColor(), lightPose.rgbx_, nextPose.rgbx_, blend0);
        } //if(animIndex ==
    }

    void Light::setLightAnim(pmm::LightAnimPack* pack)
    {
        LASSERT(pack != NULL);
        lightAnimPack_ = pack;
        reset();
    }

    void Light::reset()
    {
        LASSERT(lightAnimPack_ != NULL);

        initialize();

        lscene::Scene& scene = lframework::System::getRenderSys().getScene();
        lscene::LightEnvironment& lightEnv = scene.getLightEnv();
        lscene::DirectionalLight& dlight = lightEnv.getDirectionalLight();

        const pmm::LightPose& pose = lightAnimPack_->getPose(0);
        lmath::Vector4 dir(pose.direction_);
        dlight.setDirection( dir );
        set(dlight.getColor(), pose.rgbx_);
    }
}
