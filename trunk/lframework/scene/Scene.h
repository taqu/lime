#ifndef INC_LFRAMEWORK_SCENE_H__
#define INC_LFRAMEWORK_SCENE_H__
/**
@file Scene.h
@author t-sakai
@date 2009/10/06 create
*/

#include <lmath/lmath.h>

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
            return camera_->getViewMatrix();
        }

        void setViewMatrix(const lmath::Matrix44& view)
        {
            camera_->setViewMatrix(view);
        }

        const lmath::Matrix44& getProjMatrix() const
        {
            return camera_->getProjMatrix();
        }

        void setProjMatrix(const lmath::Matrix44& proj)
        {
            camera_->setProjMatrix(proj);
        }

        const lmath::Matrix44& getViewProjMatrix() const
        {
            return camera_->getViewProjMatrix();
        }

        const lmath::Matrix44& getPrevViewProjMatrix() const
        {
            return camera_->getPrevViewProjMatrix();
        }

        const Camera& getCamera() const
        {
            return *camera_;
        }

        Camera& getCamera()
        {
            return *camera_;
        }

        LightEnvironment& getLightEnv()
        {
            return *lightEnv_;
        }

        const LightEnvironment& getLightEnv() const
        {
            return *lightEnv_;
        }

        void updateSceneMatrix()
        {
            camera_->updateMatrix();
        }

        void pushSceneMatrix()
        {
            camera_->pushMatrix();
        }
    private:
        Camera* camera_;
        LightEnvironment* lightEnv_;
        u8 buffer_[sizeof(Camera) + sizeof(LightEnvironment) + 16];
    };
}

#endif //INC_LFRAMEWORK_SCENE_H__
