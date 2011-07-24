#ifndef INC_EGDA_SCENE_H__
#define INC_EGDA_SCENE_H__
/**
@file Scene.h
@author t-sakai
@date 2011/02/06 create
*/
namespace lmath
{
    class Matrix44;
}

namespace pmm
{
    class Loader;
    class ModelPack;
    class CameraAnimPack;
    class LightAnimPack;
    class AccessoryPack;
}

namespace egda
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

        void initialize();

        void update();

        void setState(State state);
        void setCameraMode(s32 mode);
        s32 getCameraMode() const;

        void swap(Scene& rhs);

        void resetProjection();
    private:
        struct Impl;

        Scene(const Scene&);
        Scene& operator=(const Scene&);

        Impl *impl_;
    };
}
#endif //INC_EGDA_SCENE_H__
