#ifndef INC_LSCENE_NODEOBJECTMOTION_H__
#define INC_LSCENE_NODEOBJECTMOTION_H__
/**
@file NodeObjectMotion.h
@author t-sakai
@date 2015/01/26 create
*/
#include "NodeObject.h"

namespace lscene
{
    class NodeObjectMotion : public NodeObject
    {
    public:
        explicit NodeObjectMotion(const Char* name = NULL);
        virtual ~NodeObjectMotion();

        virtual s32 getType() const;

        inline virtual const lmath::Matrix44& getPrevMatrix() const;
        void pushMatrix();

        virtual void visitRenderQueue(lscene::RenderContext& renderContext);
        virtual void render(lscene::RenderContext& renderContext);

    protected:
        void renderMotionVelocity(lscene::RenderContext& renderContext);

        lmath::Matrix44 prevMatrix_;
    };

    inline const lmath::Matrix44& NodeObjectMotion::getPrevMatrix() const
    {
        return prevMatrix_;
    }
}
#endif //INC_LSCENE_NODEOBJECTMOTION_H__
