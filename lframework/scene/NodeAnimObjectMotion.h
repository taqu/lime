#ifndef INC_LSCENE_NODEANIMOBJECTMOTION_H__
#define INC_LSCENE_NODEANIMOBJECTMOTION_H__
/**
@file NodeAnimObjectMotion.h
@author t-sakai
@date 2015/01/26 create
*/
#include "NodeAnimObject.h"

namespace lgraphics
{
    class ContextRef;

    class ConstantBufferRef;

    class VertexShaderRef;
    class GeometryShaderRef;
    class PixelShaderRef;
}

namespace lscene
{
    class NodeAnimObjectMotion : public NodeAnimObject
    {
    public:
        explicit NodeAnimObjectMotion(const Char* name = NULL);
        virtual ~NodeAnimObjectMotion();

        virtual s32 getType() const;
        virtual void update();

        inline virtual const lmath::Matrix44& getPrevMatrix() const;
        void pushMatrix();
        void copyMatrices();

        virtual void visitRenderQueue(lscene::RenderContext& renderContext);
        virtual void render(lscene::RenderContext& renderContext);

    protected:
        void renderMotionVelocity(lscene::RenderContext& renderContext);

        inline void setSkinningMatricesTwoFrames(lscene::RenderContext& renderContext);

        virtual void createMatrices(s32 num);
        virtual void releaseMatrices();

        lmath::Matrix44 prevMatrix_;
        lmath::Matrix34* prevSkinningMatrices_;
    };

    inline const lmath::Matrix44& NodeAnimObjectMotion::getPrevMatrix() const
    {
        return prevMatrix_;
    }
}
#endif //INC_LSCENE_NODEANIMOBJECTMOTION_H__
