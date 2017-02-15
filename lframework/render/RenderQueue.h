#ifndef INC_LFRAMEWORK_RENDERQUEUE_H__
#define INC_LFRAMEWORK_RENDERQUEUE_H__
/**
@file RenderQueue.h
@author t-sakai
@date 2016/11/15 create
*/
#include "../lframework.h"
#include <lmath/Vector4.h>
#include "Frustum.h"

namespace lgfx
{
    class ContextRef;
}

namespace lfw
{
    class Camera;
    class ShadowMap;
    class ComponentRenderer;

    /**
    @note RenderContextÇ™ê≥ÇµÇ≠ä«óùÇ∑ÇÈÇÃÇ≈, ÉRÉsÅ[ÇÕã÷é~ÇµÇ»Ç¢
    */
    class RenderQueue
    {
    public:
        static const s32 DefaultIncSize = 128;

        struct Entry
        {
            f32 depth_;
            ComponentRenderer* component_;
        };

        struct Queue
        {
            s32 capacity_;
            s32 size_;
            Entry* entries_;
        };

        RenderQueue();
        ~RenderQueue();

        void initialize();
        void destroy();

        void add(s32 renderPath, f32 depth, ComponentRenderer* component);
        inline s32 size() const;
        inline void clear();
        inline void swap(RenderQueue& rhs);

        inline const Camera& getCamera() const;
        void setCamera(const Camera* camera);
        inline const ShadowMap& getShadowMap();
        inline void setShadowMap(const ShadowMap* shadowMap);
        inline const Frustum& getWorldFrustum() const;

        inline const Queue& operator[](s32 index) const;
        inline Queue& operator[](s32 index);

        void sort();
    private:
        Queue queues_[RenderPath_Num];

        const Camera* camera_;
        const ShadowMap* shadowMap_;

        Frustum worldFrustum_;
    };

    inline s32 RenderQueue::size() const
    {
        return RenderPath_Num;
    }

    inline void RenderQueue::clear()
    {
        for(s32 i=0; i<RenderPath_Num; ++i){
            queues_[i].size_ = 0;
        }
    }

    inline void RenderQueue::swap(RenderQueue& rhs)
    {
        for(s32 i=0; i<RenderPath_Num; ++i){
            lcore::swap(queues_[i].capacity_, rhs.queues_[i].capacity_);
            lcore::swap(queues_[i].size_, rhs.queues_[i].size_);
            lcore::swap(queues_[i].entries_, rhs.queues_[i].entries_);
        }
    }

    inline const Camera& RenderQueue::getCamera() const
    {
        return *camera_;
    }

    inline const ShadowMap& RenderQueue::getShadowMap()
    {
        return *shadowMap_;
    }

    inline void RenderQueue::setShadowMap(const ShadowMap* shadowMap)
    {
        shadowMap_ = shadowMap;
    }

    inline const Frustum& RenderQueue::getWorldFrustum() const
    {
        return worldFrustum_;
    }

    inline const RenderQueue::Queue& RenderQueue::operator[](s32 index) const
    {
        LASSERT(0<=index && index<RenderPath_Num);
        return queues_[index];
    }

    inline RenderQueue::Queue& RenderQueue::operator[](s32 index)
    {
        LASSERT(0<=index && index<RenderPath_Num);
        return queues_[index];
    }
}
#endif //INC_LFRAMEWORK_RENDERQUEUE_H__
