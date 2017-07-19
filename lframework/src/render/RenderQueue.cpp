/**
@file RenderQueue.cpp
@author t-sakai
@date 2016/11/15 create
*/
#include "render/RenderQueue.h"
#include <lcore/Sort.h>
#include "ecs/ComponentRenderer.h"
#include "render/Camera.h"

namespace lfw
{
    RenderQueue::RenderQueue()
    {
        for(s32 i=0; i<RenderPath_Num; ++i){
            queues_[i].entries_ = NULL;
        }
    }

    RenderQueue::~RenderQueue()
    {

    }

    void RenderQueue::initialize()
    {
        for(s32 i=0; i<RenderPath_Num; ++i){
            queues_[i].entries_ = NULL;
        }
        for(s32 i=0; i<RenderPath_Num; ++i){
            queues_[i].capacity_ = DefaultIncSize;
            queues_[i].size_ = 0;
            queues_[i].entries_ = LNEW Entry[queues_[i].capacity_];
        }
    }

    void RenderQueue::destroy()
    {
        for(s32 i=0; i<RenderPath_Num; ++i){
            queues_[i].capacity_ = 0;
            queues_[i].size_ = 0;
            LDELETE_ARRAY(queues_[i].entries_);
        }
    }

    void RenderQueue::add(s32 renderPath, f32 depth, ComponentRenderer* component)
    {
        LASSERT(NULL != component);
        LASSERT(0<=renderPath && renderPath<RenderPath_Num);

        Queue& queue = queues_[renderPath];
        if(queue.capacity_<=queue.size_){
            s32 capacity = queue.capacity_ + DefaultIncSize;
            Entry* entries = LNEW Entry[capacity];
            lcore::memcpy(entries, queue.entries_, sizeof(Entry)*queue.capacity_);
            LDELETE_ARRAY(queue.entries_);
            queue.capacity_ = capacity;
            queue.entries_ = entries;
        }
        queue.entries_[queue.size_].depth_ = depth;
        queue.entries_[queue.size_].component_ = component;
        lmath::lm128 bmin, bmax;
        ++queue.size_;
    }

    void RenderQueue::setCamera(const Camera* camera)
    {
        camera_ = camera;
        worldFrustum_.calcInWorld(*camera_);
    }

    namespace
    {
        bool SortCompDepthAscend(const RenderQueue::Entry& lhs, const RenderQueue::Entry& rhs)
        {
            return (lhs.component_->getSortLayer() == rhs.component_->getSortLayer())
                ? lhs.depth_<rhs.depth_
                : lhs.component_->getSortLayer()<rhs.component_->getSortLayer();
        }

        bool SortCompDepthDescend(const RenderQueue::Entry& lhs, const RenderQueue::Entry& rhs)
        {
            return (lhs.component_->getSortLayer() == rhs.component_->getSortLayer())
                ? rhs.depth_<lhs.depth_
                : lhs.component_->getSortLayer()<rhs.component_->getSortLayer();
        }
    }

    void RenderQueue::sort()
    {
        lcore::introsort(queues_[RenderPath_Opaque].size_, queues_[RenderPath_Opaque].entries_, SortCompDepthAscend);
        lcore::introsort(queues_[RenderPath_Transparent].size_, queues_[RenderPath_Transparent].entries_, SortCompDepthDescend);
    }
}
