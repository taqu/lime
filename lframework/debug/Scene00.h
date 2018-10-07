#ifndef INC_SAMPLES_SCENE00_H_
#define INC_SAMPLES_SCENE00_H_
/**
@file Scene00.h
@author t-sakai
@date 2016/11/29 create
*/
#include <lframework/ecs/ComponentBehavior.h>
#include <lframework/ecs/Entity.h>

namespace lsound
{
    class UserPlayer;
}

namespace debug
{
    class Scene00 : public lfw::ComponentBehavior
    {
    public:
        Scene00();
        virtual ~Scene00();

        virtual void onCreate();
        virtual void onStart();
        virtual void update();
        virtual void onDestroy();

    private:
        lfw::Entity mainLight00Entity_;
        lfw::Entity mainCamera00Entity_;
        lfw::Entity behaviorEntities_[5];
        lfw::Entity sprite00_;
        lfw::Entity particle00_;
        lfw::Entity volumeParticle00_;
        lfw::Entity volumeParticle01_;
        lsound::UserPlayer* bgmPlayer_;

        lfw::f32 roty_;
        lfw::f32 rotx_;

        bool showTestWindow_;
        lfw::s32 count_;
    };
}
#endif //INC_SAMPLES_SCENE00_H_
