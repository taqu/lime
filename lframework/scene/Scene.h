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

        inline const lmath::Matrix44& getViewMatrix() const;

        inline void setViewMatrix(const lmath::Matrix44& view);

        inline const lmath::Matrix44& getProjMatrix() const;

        inline void setProjMatrix(const lmath::Matrix44& proj);

        inline const lmath::Matrix44& getViewProjMatrix() const;

        inline const lmath::Matrix44& getPrevViewProjMatrix() const;

        inline const Camera& getCamera() const;

        inline Camera& getCamera();

        inline LightEnvironment& getLightEnv();

        inline const LightEnvironment& getLightEnv() const;

        inline void updateSceneMatrix();

        inline void pushSceneMatrix();

    private:
        Camera* camera_;
        LightEnvironment* lightEnv_;
        u8 buffer_[sizeof(Camera)+sizeof(LightEnvironment)+16];
    };

    inline const lmath::Matrix44& Scene::getViewMatrix() const
    {
        return camera_->getViewMatrix();
    }

    inline void Scene::setViewMatrix(const lmath::Matrix44& view)
    {
        camera_->setViewMatrix(view);
    }

    inline const lmath::Matrix44& Scene::getProjMatrix() const
    {
        return camera_->getProjMatrix();
    }

    inline void Scene::setProjMatrix(const lmath::Matrix44& proj)
    {
        camera_->setProjMatrix(proj);
    }

    inline const lmath::Matrix44& Scene::getViewProjMatrix() const
    {
        return camera_->getViewProjMatrix();
    }

    inline const lmath::Matrix44& Scene::getPrevViewProjMatrix() const
    {
        return camera_->getPrevViewProjMatrix();
    }

    inline const Camera& Scene::getCamera() const
    {
        return *camera_;
    }

    inline Camera& Scene::getCamera()
    {
        return *camera_;
    }

    inline LightEnvironment& Scene::getLightEnv()
    {
        return *lightEnv_;
    }

    inline const LightEnvironment& Scene::getLightEnv() const
    {
        return *lightEnv_;
    }

    inline void Scene::updateSceneMatrix()
    {
        camera_->updateMatrix();
    }

    inline void Scene::pushSceneMatrix()
    {
        camera_->pushMatrix();
    }
}

#endif //INC_LFRAMEWORK_SCENE_H__
