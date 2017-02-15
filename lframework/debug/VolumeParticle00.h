#ifndef INC_SAMPLES_VOLUMEPARTICLE00_H__
#define INC_SAMPLES_VOLUMEPARTICLE00_H__
/**
@file VolumeParticle00.h
@author t-sakai
@date 2016/12/23 create
*/
#include <lframework/ecs/ComponentVolumeParticleRenderer.h>

namespace debug
{
    class VolumeParticle00 : public lfw::ComponentVolumeParticleRenderer<lfw::Nothing>
    {
    public:
        VolumeParticle00();
        virtual ~VolumeParticle00();

        virtual void onCreate();
        virtual void onStart();
        virtual void update();
        virtual void postUpdate();
        virtual void onDestroy();

    private:
        lfw::f32 time_;
    };
}
#endif //INC_SAMPLES_VOLUMEPARTICLE00_H__
