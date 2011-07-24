#ifndef INC_VIEWER_SCENE_H__
#define INC_VIEWER_SCENE_H__
/**
@file Scene.h
@author t-sakai
@date 2011/02/06 create
*/
namespace pmm
{
    class Loader;
    class ModelPack;
    class CameraAnimPack;
    class LightAnimPack;
    class AccessoryPack;
}

namespace viewer
{
    class Scene
    {
    public:
        enum State
        {
            State_Stop =0,
            State_Play,
            State_Num,
        };

        Scene();
        Scene(pmm::Loader& loader);
        ~Scene();

        void release();

        void initialize(f32 aspect);

        void update();

        void setState(State state);

        void swap(Scene& rhs);
    private:
        struct Impl;

        Scene(const Scene&);
        Scene& operator=(const Scene&);

        Impl *impl_;
    };
}
#endif //INC_VIEWER_SCENE_H__
