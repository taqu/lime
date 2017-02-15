/**
@file Node.cpp
@author t-sakai
@date 2016/11/23 create
*/
#include "render/Node.h"
#include "resource/load.h"

namespace lfw
{
    //--------------------------------------------------
    Node::Node()
        :rotationOrder_(0)
        ,meshStartIndex_(0)
        ,numMeshes_(0)
        ,index_(0)
        ,parentIndex_(InvalidNode)
        ,childStartIndex_(0)
        ,numChildren_(0)
        ,meshes_(NULL)
    {
    }

    Node::~Node()
    {
    }

    //--------------------------------------------------
    void Node::clearTransform()
    {
        translation_ = lmath::Vector4::zero();
        rotation_.setIdentity();
        scale_ = lmath::Vector3::one();
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
    void Node::copyTo(Node& dst)
    {
        dst.translation_ = translation_;
        dst.rotation_ = rotation_;
        dst.scale_ = scale_;
        dst.world0_ = world0_;
        dst.world1_ = world1_;
        dst.type_ = type_;
        dst.rotationOrder_ = rotationOrder_;
        dst.meshStartIndex_ = meshStartIndex_;
        dst.numMeshes_ = numMeshes_;
        dst.index_ = index_;
        dst.parentIndex_ = parentIndex_;
        dst.childStartIndex_ = childStartIndex_;
        dst.numChildren_ = numChildren_;
        dst.meshes_ = meshes_;
    }

    //--------------------------------------------------
    void Node::calcEuler(f32 x, f32 y, f32 z)
    {
        lmath::Quaternion rotX, rotY, rotZ;
        rotX.setRotateX(x);
        rotY.setRotateX(y);
        rotZ.setRotateX(z);

        rotation_.setIdentity();

        switch(rotationOrder_)
        {
        case RotationOrder_EulerXYZ:
            rotation_ *= rotX;
            rotation_ *= rotY;
            rotation_ *= rotZ;
            break;
        case RotationOrder_EulerXZY:
            rotation_ *= rotX;
            rotation_ *= rotZ;
            rotation_ *= rotY;
            break;
        case RotationOrder_EulerYZX:
            rotation_ *= rotY;
            rotation_ *= rotZ;
            rotation_ *= rotX;
            break;
        case RotationOrder_EulerYXZ:
            rotation_ *= rotY;
            rotation_ *= rotX;
            rotation_ *= rotZ;
            break;
        case RotationOrder_EulerZXY:
            rotation_ *= rotZ;
            rotation_ *= rotX;
            rotation_ *= rotY;
            break;
        case RotationOrder_EulerZYX:
            rotation_ *= rotZ;
            rotation_ *= rotY;
            rotation_ *= rotX;
            break;
        case RotationOrder_SphericXYZ:
            LASSERT(false);
            break;
        };
    }

    //--------------------------------------------------
    void Node::calcLocalMatrix(lmath::Matrix44& local)
    {
        //local.identity();
        //local.setScale(scale_.x_, scale_.y_, scale_.z_);
        rotation_.getMatrix(local);

        local.translate(translation_.x_, translation_.y_, translation_.z_);
    }
}
