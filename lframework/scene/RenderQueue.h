#ifndef INC_LSCENE_RENDERQUEUE_H__
#define INC_LSCENE_RENDERQUEUE_H__
/**
@file RenderQueue.h
@author t-sakai
@date 2014/10/09 create
*/
#include "lscene.h"
#include <lcore/Vector.h>
#include "SceneRenderable.h"

namespace lgraphics
{
    class ContextRef;
}

namespace lscene
{
    class RenderQueue
    {
    public:
        typedef lcore::vector_arena<SceneRenderable*, lcore::vector_arena_static_inc_size<128>, lscene::SceneAllocator> RenderableVector;

        RenderQueue();
        RenderQueue(lgraphics::ContextRef* context, s32 numPaths);
        ~RenderQueue();

        void initialize(lgraphics::ContextRef* context, s32 numPaths);
        void terminate();

        lgraphics::ContextRef& getContext(){ return *context_;}
        void setContext(lgraphics::ContextRef* context){context_ = context;}

        s32 getNumPaths() const;

        void add(s32 path, SceneRenderable* renderable);
        void clear();

        RenderableVector& getPath(s32 path);

        void swap(RenderQueue& rhs);

        inline s32 getPathToIndex(s32 path) const;
        void setPathToIndex(s32 path, s8 index);

        void sortDepthAscend(s32 path);
        void sortDepthDescend(s32 path);
    private:
        RenderQueue(const RenderQueue& rhs);
        RenderQueue& operator=(const RenderQueue& rhs);

        lgraphics::ContextRef* context_;
        s32 numPaths_;
        RenderableVector* paths_;

        s8 pathToIndex_[MaxPaths];
    };

    inline s32 RenderQueue::getPathToIndex(s32 path) const
    {
        return pathToIndex_[path];
    }
}
#endif //INC_LSCENE_RENDERQUEUE_H__
