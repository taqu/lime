#ifndef INC_LFRAMEWORK_COMPONENTMESHRENDERER_H__
#define INC_LFRAMEWORK_COMPONENTMESHRENDERER_H__
/**
@file ComponentMeshRenderer.h
@author t-sakai
@date 2016/10/18 create
*/
#include "ComponentRenderer.h"
#include <lmath/Matrix44.h>
#include "../render/Model.h"

namespace lfw
{
    class ComponentMeshRenderer : public ComponentRenderer
    {
    public:
        ComponentMeshRenderer();
        virtual ~ComponentMeshRenderer();

        virtual void onCreate();
        virtual void onStart();
        virtual void update();
        virtual void postUpdate();
        virtual void onDestroy();
        virtual void addQueue(RenderQueue& queue);
        virtual void drawDepth(RenderContext& renderContext);
        virtual void drawOpaque(RenderContext& renderContext);
        virtual void drawTransparent(RenderContext& renderContext);

        inline Model::pointer& getMesh();
        void setMesh(Model::pointer& model);

        void pushMatrix();
    protected:
        ComponentMeshRenderer(const ComponentMeshRenderer&);
        ComponentMeshRenderer& operator=(const ComponentMeshRenderer&);

        void calcNodeMatrices();
        void calcPrevNodeMatrices();
        void resetShaderSet();
        void resetFlags();

        Model::pointer model_;
        lmath::Matrix44 prevMatrix_;
    };

    inline Model::pointer& ComponentMeshRenderer::getMesh()
    {
        return model_;
    }
}
#endif //INC_LFRAMEWORK_COMPONENTMESHRENDERER_H__
