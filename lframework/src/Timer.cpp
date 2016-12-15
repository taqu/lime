/**
@file Timer.cpp
@author t-sakai
@date 2016/11/11 create
*/
#include "Timer.h"
#include <lgraphics/lgraphics.h>
#include <lmath/lmath.h>

namespace lfw
{
    Timer::Timer()
        :prevTime_(0)
        ,mode_(Mode_Static)
        ,frameCount_(0)
        ,staticDeltaTime_(0.016666f)
        ,staticInvDeltaTime_(1.0f/0.016666f)
        ,deltaTime_(0.016666f)
        ,invDeltaTime_(1.0f/0.016666f)
    {
    }

    Timer::~Timer()
    {
    }

    void Timer::reset()
    {
        frameCount_ = 0;
        prevTime_ = lcore::getPerformanceCounter();
        for(s32 i=0; i<8; ++i){
            accumulation_[i] = deltaTime_;
        }
    }

    void Timer::update()
    {
        lcore::ClockType currentTime = lcore::getPerformanceCounter();
        f32 delta = lcore::calcTime(prevTime_, currentTime);
        delta = lcore::clamp(delta, 0.001f, 0.016666f*10.0f);

        accumulation_[frameCount_&0x07U] = delta;

        if(mode_ == Mode_Static){
            deltaTime_ = staticDeltaTime_;
            invDeltaTime_ = staticInvDeltaTime_;

        }else{
            deltaTime_ = accumulation_[0];
            for(s32 i=1; i<8; ++i){
                deltaTime_ += accumulation_[i];
            }
            deltaTime_ *= (1.0f/8.0f);
            invDeltaTime_ = 1.0f/deltaTime_;
        }

        prevTime_ = currentTime;
        ++frameCount_;
    }

    void Timer::setMode(Mode mode)
    {
        mode_ = mode;
    }

    void Timer::setStaticDeltaTime(f32 delta)
    {
        LASSERT(0.0f<delta);
        staticDeltaTime_ = delta;
        staticInvDeltaTime_ = 1.0f/delta;
    }
}
