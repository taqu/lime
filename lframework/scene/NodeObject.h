#ifndef INC_LSCENE_NODEOBJECT_H__
#define INC_LSCENE_NODEOBJECT_H__
/**
@file NodeObject.h
@author t-sakai
@date 2014/11/16 create
*/
#include "lscene.h"
#include "NodeTransform.h"

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
namespace lrender
{
    class Object;
    class Material;
}

    class NodeObject : public lscene::NodeTransform
    {
    public:
        explicit NodeObject(const Char* name = NULL, u16 group=Group_None, u16 type=NodeType_Object);
        virtual ~NodeObject();

        lrender::Object* getObject();
        void setObject(lrender::Object* obj);

        virtual void resetAttributes();
        virtual void resetShaders();

        virtual void visitRenderQueue(lscene::RenderContext& renderContext);
        virtual void render(lscene::RenderContext& renderContext);

    protected:

        void calcNodeMatrices();
        void renderDepth(lscene::RenderContext& renderContext);
        void renderSolid(lscene::RenderContext& renderContext);
        void renderTransparent(lscene::RenderContext& renderContext);

        inline void setTexture(lgraphics::ContextRef& context, lrender::Material& material);

        lrender::Object* object_;
    };
}
#endif //INC_LSCENE_NODEOBJECT_H__
