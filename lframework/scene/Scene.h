#ifndef INC_LFRAMEWORK_SCENE_H__
#define INC_LFRAMEWORK_SCENE_H__
/**
@file Scene.h
@author t-sakai
@date 2009/10/06 create
*/

#include <lmath/lmath.h>
#include <lgraphics/api/SamplerState.h>
#include <lgraphics/api/RenderStateRef.h>
#include <lgraphics/api/TextureRef.h>
#include <lgraphics/api/SurfaceRef.h>

#include "LightEnvironment.h"
#include "Camera.h"

namespace lscene
{
    class Scene
    {
    public:
        Scene();
        ~Scene();

        const lmath::Matrix44& getViewMatrix() const
        {
            return camera_.getViewMatrix();
        }

        void setViewMatrix(const lmath::Matrix44& view)
        {
            camera_.setViewMatrix(view);
        }

        const lmath::Matrix44& getProjMatrix() const
        {
            return camera_.getProjMatrix();
        }

        void setProjMatrix(const lmath::Matrix44& proj)
        {
            camera_.setProjMatrix(proj);
        }

        const lmath::Matrix44& getViewProjMatrix() const
        {
            return camera_.getViewProjMatrix();
        }

        Camera& getCamera()
        {
            return camera_;
        }

        LightEnvironment& getLightEnv()
        {
            return lightEnv_;
        }

        const LightEnvironment& getLightEnv() const
        {
            return lightEnv_;
        }

        const lmath::Matrix44& getLightViewProjection() const
        {
            return lightViewProjection_;
        }


        void setSceneAABB(const lmath::Vector4& aabbMin, const lmath::Vector4& aabbMax)
        {
            sceneAABBMin_ = aabbMin;
            sceneAABBMax_ = aabbMax;
        }

        const lmath::Vector4& getAABBMin() const
        {
            return sceneAABBMin_;
        }

        const lmath::Vector4& getAABBMax() const
        {
            return sceneAABBMax_;
        }

        u32 getShadowMapSize() const
        {
            return shadowMapSize_;
        }

        void setShadowMapSize(u32 size)
        {
            LASSERT(size>2);
            shadowMapSize_ = size;
            shadowTextureOffset_ = 0.5f + (0.5f / shadowMapSize_);
        }

        f32 getShadowMapBias() const
        {
            return shadowMapBias_;
        }

        void setShadowMapBias(f32 bias)
        {
            shadowMapBias_ = bias;
        }

        f32 getShadowMapZFar() const
        {
            return shadowMapZFar_;
        }

        void setShadowMapZFar(f32 zfar)
        {
            shadowMapZFar_ = zfar;
        }

        //const lgraphics::SamplerState& getSamplerState() const
        //{
        //    return depthSamplerState_;
        //}

        //lgraphics::RenderStateRef& getDepthRenderState()
        //{
        //    return depthState_;
        //}

        //const lgraphics::TextureRef& getDepthTexture() const
        //{
        //    return depthTexture_;
        //}

        //lgraphics::SurfaceRef& getDepthSurface()
        //{
        //    return depthSurface_;
        //}

        //void resetDepthTexture();

        void calcLightViewProjection();
        void mulShadowTextureOffset(lmath::Matrix44& mat) const;
    private:
        Camera camera_;
        LightEnvironment lightEnv_;

        lmath::Matrix44 lightViewProjection_;

        lmath::Vector4 sceneAABBMin_;
        lmath::Vector4 sceneAABBMax_;
        
        //lgraphics::TextureRef depthTexture_;
        //lgraphics::SurfaceRef depthSurface_;

        //lgraphics::SamplerState depthSamplerState_;
        //lgraphics::RenderStateRef depthState_;

        u32 shadowMapSize_;
        f32 shadowMapBias_;
        f32 shadowMapZFar_;
        f32 shadowTextureOffset_;
    };
}

#endif //INC_LFRAMEWORK_SCENE_H__
