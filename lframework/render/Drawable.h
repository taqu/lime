#ifndef INC_DRAWABLE_H__
#define INC_DRAWABLE_H__
/**
@file Drawable.h
@author t-sakai
@date 2009/08/15 create
*/
#include "lrender.h"
#include "Spatial.h"

namespace lmath
{
    class Matrix43;
}

namespace lscene
{
    class Scene;
}

namespace lrender
{
    class Drawable : public Spatial
    {
    public:
        virtual u32 getNumJointPoses() const{ return 0; }
        virtual const lmath::Matrix43* getGlobalJointPoses() const{ return NULL;}
    };
}

#endif //INC_DRAWABLE_H__
