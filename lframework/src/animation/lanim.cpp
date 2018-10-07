/**
@file lanim.cpp
@author t-sakai
@date 2016/11/24 create
*/
#include "animation/lanim.h"
#include <lcore/File.h>

namespace lfw
{
    AnimationSystem::AnimationSystem()
        :unitTime_(0.01667f)
        ,invUnitTime_(1.0f/0.01667f)
        ,animationSpeed_(0.01667f)
    {
    }

    AnimationSystem::~AnimationSystem()
    {
    }

    AnimationSystem AnimationSystem::instance_;

    void write(lcore::File& os, const Name& name)
    {
        os.write(name.size()+1, name.c_str());
    }

    void read(Name& name, lcore::File& is)
    {
        Char tmp[Name::Size];
        Char c;
        for(s32 i=0; i<Name::Size; ++i){
            is.read(c);
            tmp[i] = c;
            if(c == lcore::CharNull){
                break;
            }
        }
        name.assign(tmp);
    }
}
