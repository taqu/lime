/**
@file Emitter.cpp
@author t-sakai
@date 2015/09/24 create
*/
#include "emitter/Emitter.h"

namespace lrender
{
    Emitter::Emitter()
        :numSamples_(1)
    {
    }

    Emitter::Emitter(s32 numSamples, const lmath::Matrix44& localToWorld)
        :numSamples_(numSamples)
        ,localToWorld_(localToWorld)
    {
        worldToLocal_.getInvert(localToWorld_);
    }

    Emitter::~Emitter()
    {
    }
}
