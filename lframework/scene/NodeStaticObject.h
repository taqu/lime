#ifndef INC_LSCENE_NODESTATICOBJECT_H__
#define INC_LSCENE_NODESTATICOBJECT_H__
/**
@file NodeStaticObject.h
@author t-sakai
@date 2015/05/24 create
*/
#include "lscene.h"
#include "NodeObject.h"

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

    class NodeStaticObject : public lscene::NodeObject
    {
    public:
        explicit NodeStaticObject(const Char* name = NULL);
        virtual ~NodeStaticObject();

        virtual s32 getType() const;
        virtual void update();
    };
}
#endif //INC_LSCENE_NODESTATICOBJECT_H__
