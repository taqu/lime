/**
@file RenderQueue2D.cpp
@author t-sakai
@date 2016/12/20 create
*/
#include "render/RenderQueue2D.h"
#include <lcore/Sort.h>
#include "ecs/ComponentRenderer2D.h"

namespace lfw
{
    RenderQueue2D::RenderQueue2D()
    {
        queue_.entries_ = NULL;
    }

    RenderQueue2D::~RenderQueue2D()
    {
    }

    void RenderQueue2D::initialize()
    {
        queue_.entries_ = NULL;
        queue_.capacity_ = DefaultIncSize;
        queue_.size_ = 0;
        queue_.entries_ = LNEW Entry[queue_.capacity_];
    }

    void RenderQueue2D::destroy()
    {
        queue_.capacity_ = 0;
        queue_.size_ = 0;
        LDELETE_ARRAY(queue_.entries_);
    }

    void RenderQueue2D::add(f32 depth, s32 materialId, ComponentRenderer2D* component)
    {
        LASSERT(NULL != component);

        if(queue_.capacity_<=queue_.size_){
            s32 capacity = queue_.capacity_ + DefaultIncSize;
            Entry* entries = LNEW Entry[capacity];
            lcore::memcpy(entries, queue_.entries_, sizeof(Entry)*queue_.capacity_);
            LDELETE_ARRAY(queue_.entries_);
            queue_.capacity_ = capacity;
            queue_.entries_ = entries;
        }
        queue_.entries_[queue_.size_].depth_ = depth;
        queue_.entries_[queue_.size_].materialId_ = materialId;
        queue_.entries_[queue_.size_].component_ = component;
        ++queue_.size_;
    }

    namespace
    {
        bool SortCompDepthDescend(const RenderQueue2D::Entry& lhs, const RenderQueue2D::Entry& rhs)
        {
#if 0
            if(lhs.component_->getSortLayer() == rhs.component_->getSortLayer()){
                return (lmath::isEqual(lhs.depth_, rhs.depth_))? lhs.component_->getSortLayer()<rhs.component_->getSortLayer() : rhs.depth_<lhs.depth_;
            }else{
                return lhs.component_->getSortLayer()<rhs.component_->getSortLayer();
            }
#else
            return (lhs.component_->getSortLayer() == rhs.component_->getSortLayer())
                ? rhs.depth_<lhs.depth_
                : lhs.component_->getSortLayer()<rhs.component_->getSortLayer();
#endif
        }
    }

    void RenderQueue2D::sort()
    {
        lcore::introsort(queue_.size_, queue_.entries_, SortCompDepthDescend);
    }
}
