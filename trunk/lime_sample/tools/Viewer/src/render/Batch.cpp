/**
@file Batch.cpp
@author t-sakai
@date 2010/11/21 create
*/
#include "Batch.h"
#include "../scene/Geometry.h"
#include "../scene/Material.h"

namespace lrender
{
    // コンストラクタ
    Batch::Batch()
        :drawable_(NULL)
        ,geometry_(NULL)
        ,material_(NULL)
        ,lengthToCamera_(0.0f)
    {
    }

    // コンストラクタ
    Batch::Batch(
        Drawable* drawable,
        lscene::Geometry* geometry,
        lscene::Material* material)
        :drawable_(drawable)
        ,geometry_(geometry)
        ,material_(material)
        ,lengthToCamera_(0.0f)
    {
        LASSERT(drawable_ != NULL);
        LASSERT(geometry_ != NULL);
        LASSERT(material_ != NULL);
    }

    // デストラクタ
    Batch::~Batch()
    {
        drawable_ = NULL;
        geometry_ = NULL;
        material_ = NULL;
    }

    // スワップ
    void Batch::swap(Batch& rhs)
    {
        lcore::swap(drawable_, rhs.drawable_);
        lcore::swap(geometry_, rhs.geometry_);
        lcore::swap(material_, rhs.material_);
        lcore::swap(lengthToCamera_, rhs.lengthToCamera_);
    }
}
