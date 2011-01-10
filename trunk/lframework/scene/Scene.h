#ifndef INC_SCENE_H__
#define INC_SCENE_H__
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
        Scene(){}
        ~Scene(){}

        void updateMatrix();

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

    private:
        Camera camera_;
        LightEnvironment lightEnv_;
    };
}

#endif //INC_SCENE_H__
