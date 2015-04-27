/**
@file Node.cpp
@author t-sakai
@date 2012/02/28 create
*/
#include "Node.h"
#include "../load/load.h"

namespace lscene
{
namespace lrender
{
    //--------------------------------------------------
    Node::Node()
        :rotationOrder_(0)
        ,meshStartIndex_(0)
        ,numMeshes_(0)
        ,index_(0)
        ,parentIndex_(lload::InvalidNode)
        ,childStartIndex_(0)
        ,numChildren_(0)
        ,meshes_(NULL)
        //,dualQuaternions_(NULL)
    {
    }

    Node::~Node()
    {
    }

    //--------------------------------------------------
    void Node::clearTransform()
    {
        translation_.zero();
        rotation_.identity();
        scale_.one();
    }

    //--------------------------------------------------
    // ÉXÉèÉbÉv
    void Node::swap(Node& rhs)
    {
        lcore::swap(rotationOrder_, rhs.rotationOrder_);
        lcore::swap(meshStartIndex_, rhs.meshStartIndex_);
        lcore::swap(numMeshes_, rhs.numMeshes_);

        lcore::swap(index_, rhs.index_);
        lcore::swap(parentIndex_, rhs.parentIndex_);
        lcore::swap(childStartIndex_, rhs.childStartIndex_);
        lcore::swap(numChildren_, rhs.numChildren_);

        lcore::swap(numMeshes_, rhs.numMeshes_);
        lcore::swap(meshes_, rhs.meshes_);

        translation_.swap(rhs.translation_);
        rotation_.swap(rhs.rotation_);
        scale_.swap(rhs.scale_);
    }

    //--------------------------------------------------
    void Node::calcRotation(const lmath::Vector3& angle)
    {
        lmath::Quaternion rotX, rotY, rotZ;
        rotX.setRotateX(angle.x_);
        rotY.setRotateX(angle.y_);
        rotZ.setRotateX(angle.z_);

        rotation_.identity();

        switch(rotationOrder_)
        {
        case lload::RotationOrder_EulerXYZ:
            rotation_ *= rotX;
            rotation_ *= rotY;
            rotation_ *= rotZ;
            break;
        case lload::RotationOrder_EulerXZY:
            rotation_ *= rotX;
            rotation_ *= rotZ;
            rotation_ *= rotY;
            break;
        case lload::RotationOrder_EulerYZX:
            rotation_ *= rotY;
            rotation_ *= rotZ;
            rotation_ *= rotX;
            break;
        case lload::RotationOrder_EulerYXZ:
            rotation_ *= rotY;
            rotation_ *= rotX;
            rotation_ *= rotZ;
            break;
        case lload::RotationOrder_EulerZXY:
            rotation_ *= rotZ;
            rotation_ *= rotX;
            rotation_ *= rotY;
            break;
        case lload::RotationOrder_EulerZYX:
            rotation_ *= rotZ;
            rotation_ *= rotY;
            rotation_ *= rotX;
            break;
        case lload::RotationOrder_SphericXYZ:
            LASSERT(false);
            break;
        };
    }

    //--------------------------------------------------
    void Node::calcLocalMatrix(lmath::Matrix44& local)
    {
        local.identity();
        //local.setScale(scale_.x_, scale_.y_, scale_.z_);
        rotation_.getMatrix(local);

        local.translate(translation_.x_, translation_.y_, translation_.z_);
    }
}
}
