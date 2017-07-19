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
    enum Format
    {
        Format_Unknown,
        Format_LM,
        Format_OBJ,
        Format_FBX,
    };

    //enum ECSCategory
    //{
    //    ECSCategory_Max = ECSCategory_User,
    //};

    enum ECSType
    {
        ECSType_Viewer = lfw::ECSType_User,
    };

    class Viewer : public lfw::ComponentMeshRenderer
    {
    public:
        static lfw::u8 category(){ return lfw::ECSCategory_Renderer;}
        static lfw::u32 type(){ return ECSType_Viewer;}

        Viewer();
        virtual ~Viewer();

        virtual lfw::u32 getType() const;
        virtual void onCreate();
        virtual void update();

        void setModelPath(const lfw::Char* filepath);
        static Format getFormat(const lfw::Char* filepath);
    private:
        void resetCamera();
        void loadModel(Format format);

        lfw::ComponentLight* light_;
        lfw::ComponentCamera* camera_;

        lfw::ModelLoader modelLoader_;
        lfw::Model::pointer model_;
        lfw::Skeleton::pointer skeleton_;
        lfw::LoadTexture* textures_;

        bool menuOpen_;
        Format loadModel_;
        static const lfw::s32 BufferSize = 256;
        lfw::Char filepath_[BufferSize];

        lfw::f32 rotationX_;
        lfw::f32 rotationY_;
        lfw::f32 zoom_;
        lfw::f32 zoomMin_;
        lfw::f32 zoomMax_;
        lfw::f32 zoomRate_;


        //Load Options
        lfw::f32 scale_; //only for OBJ
        bool convertToDDS_;
    };
}
#endif //INC_VIEWER_VIEWER_H_
