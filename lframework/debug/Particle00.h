#ifndef INC_SAMPLES_PARTICLE00_H_
#define INC_SAMPLES_PARTICLE00_H_
/**
@file Particle00.h
@author t-sakai
@date 2016/12/23 create
*/
#include <lframework/ecs/ComponentParticleRenderer.h>

namespace debug
{
    struct Particle00Vertex
    {
        lfw::s32 index_;
        lfw::f32 time_;
    };

    class Particle00 : public lfw::ComponentParticleRenderer<Particle00Vertex>
    {
    public:
        static const lcore::s32 capacity = 1;//64;

        Particle00();
        virtual ~Particle00();

        virtual void onCreate();
        virtual void onStart();
        virtual void update();
        virtual void postUpdate();
        virtual void onDestroy();
    private:
        Particle00Vertex vertices1_[capacity];
    };
}
#endif //INC_SAMPLES_PARTICLE00_H_
