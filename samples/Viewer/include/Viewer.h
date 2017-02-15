#ifndef INC_VIEWER_VIEWER_H_
#define INC_VIEWER_VIEWER_H_
/**
@file Viewer.h
@author t-sakai
@date 2017/01/18 create
*/
#include <lframework/ecs/ComponentMeshRenderer.h>
#include <lframework/resource/ModelLoader.h>
#include <lframework/render/Model.h>
#include <lframework/animation/Skeleton.h>

namespace lfw
{
    class ComponentLight;
    class ComponentCamera;
    class Model;
    class Skeleton;

    class LoadTexture;
}

namespace viewer
{
    class Viewer : public lfw::ComponentMeshRenderer
    {
    public:
        Viewer();
        virtual ~Viewer();

        virtual void onCreate();
        virtual void update();

    private:
        void resetCamera();

        lfw::ComponentLight* light_;
        lfw::ComponentCamera* camera_;

        lfw::ModelLoader modelLoader_;
        lfw::Model::pointer model_;
        lfw::Skeleton::pointer skeleton_;
        lfw::LoadTexture* textures_;

        static const lfw::s32 BufferSize = 256;
        lfw::Char filepath_[BufferSize];

        lfw::f32 rotationX_;
        lfw::f32 rotationY_;
        lfw::f32 zoom_;
        lfw::f32 zoomMin_;
        lfw::f32 zoomMax_;
        lfw::f32 zoomRate_;
    };
}
#endif //INC_VIEWER_VIEWER_H_
