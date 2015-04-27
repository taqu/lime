/**
@file RenderQueue.cpp
@author t-sakai
@date 2014/10/09 create
*/
#include "RenderQueue.h"
#include <lcore/Sort.h>

namespace lscene
{
    RenderQueue::RenderQueue()
        :context_(NULL)
        ,numPaths_(0)
        ,paths_(NULL)
    {
        lcore::memset(pathToIndex_, -1, sizeof(s8)*MaxPaths);
    }

    RenderQueue::RenderQueue(lgraphics::ContextRef* context, s32 numPaths)
        :context_(context)
        ,numPaths_(numPaths)
    {
        LASSERT(0<numPaths_);
        lcore::memset(pathToIndex_, -1, sizeof(s8)*MaxPaths);

        paths_ = (RenderableVector*)LSCENE_MALLOC(numPaths_*sizeof(RenderableVector));
        for(s32 i=0; i<numPaths_; ++i){
            LIME_PLACEMENT_NEW(&paths_[i]) RenderableVector(128);
        }
    }

    RenderQueue::~RenderQueue()
    {
        terminate();
    }

    void RenderQueue::initialize(lgraphics::ContextRef* context, s32 numPaths)
    {
        RenderQueue(context, numPaths).swap(*this);
    }

    void RenderQueue::terminate()
    {
        if(NULL != paths_){
            for(s32 i=0; i<numPaths_; ++i){
                paths_[i].~RenderableVector();
            }

            LSCENE_FREE(paths_);
        }
    }

    s32 RenderQueue::getNumPaths() const
    {
        return numPaths_;
    }

    void RenderQueue::add(s32 path, SceneRenderable* renderable)
    {
        LASSERT(0<=path && path<MaxPaths);
        LASSERT(0<=pathToIndex_[path] && pathToIndex_[path]<numPaths_);

        LASSERT(NULL != renderable);
        renderable->addRef();
        paths_[ pathToIndex_[path] ].push_back(renderable);
    }

    void RenderQueue::clear()
    {
        for(s32 i=0; i<numPaths_; ++i){
            for(RenderableVector::iterator itr = paths_[i].begin();
                itr != paths_[i].end();
                ++itr)
            {
                (*itr)->release();
            }
            paths_[i].clear();
        }
    }

    RenderQueue::RenderableVector& RenderQueue::getPath(s32 path)
    {
        return paths_[ pathToIndex_[path] ];
    }

    void RenderQueue::swap(RenderQueue& rhs)
    {
        for(s32 i=0; i<MaxPaths; ++i){
            lcore::swap(pathToIndex_[i], rhs.pathToIndex_[i]);
        }

        lcore::swap(context_, rhs.context_);
        lcore::swap(numPaths_, rhs.numPaths_);
        lcore::swap(paths_, rhs.paths_);
    }

    void RenderQueue::setPathToIndex(s32 path, s8 index)
    {
        LASSERT(0<=path && path<MaxPaths);
        LASSERT(0<=index && index<numPaths_);
        pathToIndex_[path] = index;
    }

namespace
{
    bool SortCompDepthAscend(SceneRenderable*& lhs, SceneRenderable*& rhs)
    {
        return lhs->getDepth()<rhs->getDepth();
    }

    bool SortCompDepthDescend(SceneRenderable*& lhs, SceneRenderable*& rhs)
    {
        return rhs->getDepth()<lhs->getDepth();
    }
}

    void RenderQueue::sortDepthAscend(s32 path)
    {
        RenderableVector& vec = getPath(path);
        lcore::introsort(vec.size(), vec.begin(), SortCompDepthAscend);
    }

    void RenderQueue::sortDepthDescend(s32 path)
    {
        RenderableVector& vec = getPath(path);
        lcore::introsort(vec.size(), vec.begin(), SortCompDepthDescend);
    }
}
