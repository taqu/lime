/**
@file NodeTransform.cpp
@author t-sakai
@date 2014/12/29 create
*/
#include "NodeTransform.h"
#include "SystemBase.h"
#include "RenderContext.h"
#include "Scene.h"

namespace lscene
{
    NodeTransform::NodeTransform(const Char* name, u16 group, u16 type)
        :NodeBase(name, group, type)
    {
        setFlag(NodeFlag_Transform);
        position_.zero();
        orientation_.identity();
        //bsphere_.zero();
        matrix_.identity();
    }

    NodeTransform::~NodeTransform()
    {
    }

    void NodeTransform::update()
    {
        updateTransform();
    }

    void NodeTransform::traverseUpdateTransform()
    {
        updateTransform();
        for(NodeVector::iterator itr = childBegin();
            itr != childEnd();
            ++itr)
        {
            (*itr)->traverseUpdateTransform();
        }
    }

    void NodeTransform::createMatrix(lmath::Matrix44& matrix)
    {
        orientation_.getMatrix(matrix);
        matrix.translate(position_.x_, position_.y_, position_.z_);
    }

    void NodeTransform::calcDepth(const RenderContext& renderContext)
    {
        f32 depth = renderContext.getScene().getCamera().getEyePosition().distanceSqr(position_);
        setDepth(depth);
    }

    bool NodeTransform::checkInWorldFrustum(const lmath::Vector4& bsphere, const RenderContext& renderContext) const
    {
        lmath::Vector4 position;
        position.add(position_, bsphere);
        return renderContext.getWorldFrustum().contains(position, bsphere.w_);
    }
}
