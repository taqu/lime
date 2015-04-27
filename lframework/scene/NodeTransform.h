#ifndef INC_LSCENE_NODETRANSFORM_H__
#define INC_LSCENE_NODETRANSFORM_H__
/**
@file NodeTransform.h
@author t-sakai
@date 2014/12/29 create
*/
#include <lmath/lmath.h>
#include <lmath/Quaternion.h>
#include <lmath/geometry/Sphere.h>
#include "NodeBase.h"

namespace lscene
{
    class RenderQueue;

    class NodeTransform : public NodeBase
    {
    public:
        explicit NodeTransform(const Char* name = NULL);
        virtual ~NodeTransform();
        virtual s32 getType() const;

        inline const lmath::Vector4& getPosition() const;
        inline void setPosition(const lmath::Vector4& position);

        inline const lmath::Quaternion& getOrientation() const;
        inline void setOrientation(lmath::Quaternion& orientation);

        inline virtual const lmath::Matrix44& getMatrix() const;
        void createMatrix(lmath::Matrix44& matrix);

        virtual void calcDepth(const RenderContext& renderContext);
        virtual bool checkInWorldFrustum(const lmath::Vector4& bsphere, const RenderContext& renderContext) const;
    protected:
        NodeTransform(const NodeTransform&);
        NodeTransform& operator=(const NodeTransform&);

        lmath::Vector4 position_;
        lmath::Quaternion orientation_;
        lmath::Matrix44 matrix_;
    };


    inline const lmath::Vector4& NodeTransform::getPosition() const
    {
        return position_;
    }

    inline void NodeTransform::setPosition(const lmath::Vector4& position)
    {
        position_ = position;
    }

    inline const lmath::Quaternion& NodeTransform::getOrientation() const
    {
        return orientation_;
    }

    inline void NodeTransform::setOrientation(lmath::Quaternion& orientation)
    {
        orientation_ = orientation;
    }

    inline const lmath::Matrix44& NodeTransform::getMatrix() const
    {
        return matrix_;
    }

}
#endif //INC_LSCENE_NODETRANSFORM_H__
