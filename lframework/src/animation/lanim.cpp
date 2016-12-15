/**
@file lanim.cpp
@author t-sakai
@date 2016/11/24 create
*/
#include "animation/lanim.h"
#include <lcore/liostream.h>

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

    void write(lcore::ostream& os, const Name& name)
    {
        lcore::io::write(os, name.c_str(), name.size()+1);
    }

    void read(Name& name, lcore::istream& is)
    {
        Char tmp[Name::Size];
        Char c;
        for(s32 i=0; i<Name::Size; ++i){
            lcore::io::read(is, c);
            tmp[i] = c;
            if(c == lcore::CharNull){
                break;
            }
        }
        name.assign(tmp);
    }
}
