#ifndef INC_LSCENE_SCENERENDERABLE_H__
#define INC_LSCENE_SCENERENDERABLE_H__
/**
@file lscene.h
@author t-sakai
@date 2014/11/11 create
*/
#include "lscene.h"
#include "mixin/Renderable.h"
#include "mixin/ReferenceCounted.h"

namespace lscene
{
    typedef mixin::Renderable< mixin::ReferenceCounted< RenderableAllocator > > SceneRenderable;
}
#endif //INC_LSCENE_SCENERENDERABLE_H__
