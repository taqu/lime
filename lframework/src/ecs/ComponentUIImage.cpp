/**
@file ComponentUIText.cpp
@author t-sakai
@date 2017/07/16 create
*/
#include "ecs/ComponentUIImage.h"
#include <lmath/lmath.h>
#include "System.h"
#include "ecs/Entity.h"
#include "ecs/ComponentGeometric.h"
#include "ecs/ComponentCanvas.h"
#include "render/Renderer.h"
#include "resource/Resources.h"

namespace lfw
{
    ComponentUIImage::ComponentUIImage()
        :rect_(lmath::Vector4::zero())
        ,scale_(1.0f)
        ,abgr_(0xFFFFFFFFU)
    {
        uvRect_[0] = uvRect_[1] = lcore::toBinary16Float(0.0f);
        uvRect_[2] = uvRect_[3] = lcore::toBinary16Float(1.0f);
    }

    ComponentUIImage::~ComponentUIImage()
    {
    }

    void ComponentUIImage::updateMesh(ComponentCanvas& canvas)
    {
        if(image_.valid()){
            lmath::Vector4 rect = lmath::mul(rect_, scale_);
            canvas.addRect(rect, abgr_, uvRect_, image_);
        }
    }

    const lmath::Vector4& ComponentUIImage::getUVRect() const
    {
        return lmath::Vector4::construct(
            lcore::fromBinary16Float(uvRect_[0]),
            lcore::fromBinary16Float(uvRect_[1]),
            lcore::fromBinary16Float(uvRect_[2]),
            lcore::fromBinary16Float(uvRect_[3]));
    }

    void ComponentUIImage::setUVRect(f32 left, f32 top, f32 right, f32 bottom)
    {
        uvRect_[0] = lcore::toBinary16Float(left);
        uvRect_[1] = lcore::toBinary16Float(top);
        uvRect_[2] = lcore::toBinary16Float(right);
        uvRect_[3] = lcore::toBinary16Float(bottom);
    }
}
