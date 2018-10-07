#ifndef INC_LFRAMEWORK_TIMER_H_
#define INC_LFRAMEWORK_TIMER_H_
/**
@file Timer.h
@author t-sakai
@date 2016/11/11 create
*/
#include "lframework.h"

namespace lfw
{
    class Timer
    {
    public:
        enum Mode
        {
            Mode_Static,
            Mode_Dynamic,
        };

        Timer();
        ~Timer();

        void reset();
        void update();

        void setMode(Mode mode);
        void setStaticDeltaTime(f32 delta);

        u32 getFrameCount() const{ return frameCount_;}

        f32 getDeltaTime() const{ return deltaTime_;}
        f32 getInvDeltaTime() const{ return invDeltaTime_;}
    private:
        lcore::ClockType prevTime_;
        s32 mode_;
        u32 frameCount_;
        f32 staticDeltaTime_;
        f32 staticInvDeltaTime_;
        f32 deltaTime_;
        f32 invDeltaTime_;
        f32 accumulation_[8];
    };
}
#endif //INC_LFRAMEWORK_TIMER_H_
