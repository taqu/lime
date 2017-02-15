#ifndef INC_LFRAMEWORK_RENDERQUEUE2D_H__
#define INC_LFRAMEWORK_RENDERQUEUE2D_H__
/**
@file RenderQueue2D.h
@author t-sakai
@date 2016/12/20 create
*/
#include "../lframework.h"
#include <lmath/Vector4.h>

namespace lfw
{
    class Camera;
    class ComponentRenderer2D;

    /**
    */
    class RenderQueue2D
    {
    public:
        static const s32 DefaultIncSize = 128;

        struct Entry
        {
            f32 depth_;
            s32 materialId_;
            ComponentRenderer2D* component_;
        };

        struct Queue
        {
            s32 capacity_;
            s32 size_;
            Entry* entries_;
        };

        RenderQueue2D();
        ~RenderQueue2D();

        void initialize();
        void destroy();

        void add(f32 depth, s32 materialId, ComponentRenderer2D* component);
        inline void clear();
        inline void swap(RenderQueue2D& rhs);

        void sort();

        inline const Queue& get() const;
        inline Queue& get();
    private:
        RenderQueue2D(const RenderQueue2D&);
        RenderQueue2D& operator=(const RenderQueue2D&);

        Queue queue_;
    };

    inline void RenderQueue2D::clear()
    {
        queue_.size_ = 0;
    }

    inline void RenderQueue2D::swap(RenderQueue2D& rhs)
    {
        lcore::swap(queue_.capacity_, rhs.queue_.capacity_);
        lcore::swap(queue_.size_, rhs.queue_.size_);
        lcore::swap(queue_.entries_, rhs.queue_.entries_);
    }

    inline const RenderQueue2D::Queue& RenderQueue2D::get() const
    {
        return queue_;
    }

    inline RenderQueue2D::Queue& RenderQueue2D::get()
    {
        return queue_;
    }
}
#endif //INC_LFRAMEWORK_RENDERQUEUE2D_H__
