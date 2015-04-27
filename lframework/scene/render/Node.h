#ifndef INC_LSCENE_LRENDER_NODE_H__
#define INC_LSCENE_LRENDER_NODE_H__
/**
@file Node.h
@author t-sakai
@date 2012/02/25 create
*/
#include "render.h"
#include <lmath/Vector4.h>
#include <lmath/Matrix44.h>
#include <lmath/Quaternion.h>
#include "../SceneRenderable.h"
namespace lmath
{
    class DualQuaternion;
}

namespace lgraphics
{
    class ConstantBufferRef;
}

namespace lscene
{
namespace lrender
{
    class Mesh;

    //--------------------------------------------------
    class Node
    {
    public:
        Node();
        ~Node();

        void clearTransform();

        /// ÉXÉèÉbÉv
        void swap(Node& rhs);

        void calcRotation(const lmath::Vector3& angle);
        void calcLocalMatrix(lmath::Matrix44& local);

        lmath::Vector4 translation_;
        lmath::Quaternion rotation_;
        lmath::Vector4 scale_;
        lmath::Matrix44 world_;

        u8 type_;
        u8 rotationOrder_;
        u8 meshStartIndex_;
        u8 numMeshes_;

        u8 index_;
        u8 parentIndex_;
        u8 childStartIndex_;
        u8 numChildren_;

        Mesh* meshes_;
    };
}
}
#endif //INC_LSCENE_LRENDER_NODE_H__
