#ifndef INC_LSCENE_MIXIN_RENDERABLE_H__
#define INC_LSCENE_MIXIN_RENDERABLE_H__
/**
@file Renderable.h
@author t-sakai
@date 2014/10/30 create
*/
#include "../lscene.h"

namespace lscene
{
    class RenderContext;

namespace mixin
{
    template<class Base>
    class Renderable : public Base
    {
    public:
        virtual ~Renderable()
        {}

        virtual void render(RenderContext& renderContext) =0;

        inline f32 getDepth() const;
        inline void setDepth(f32 depth);
    protected:
        Renderable()
            :depth_(0.0f)
        {}

        f32 depth_;
    };

    template<class Base>
    inline f32 Renderable<Base>::getDepth() const
    {
        return depth_;
    }

    template<class Base>
    inline void Renderable<Base>::setDepth(f32 depth)
    {
        depth_ = depth;
    }
}
}
#endif //INC_LSCENE_MIXIN_RENDERABLE_H__
