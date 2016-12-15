#ifndef INC_LFRAMEWORK_LOADGEOMETRY_H__
#define INC_LFRAMEWORK_LOADGEOMETRY_H__
/**
@file load_geometry.h
@author t-sakai
@date 2016/11/23 create
*/
#include "load.h"
#include <lmath/geometry/Sphere.h>

namespace lfw
{
    class LoadGeometry
    {
    public:
        inline LoadGeometry();

        u32 vflag_; /// 頂点宣言フラグ
        u32 vsize_; /// 頂点サイズ
        u32 numVertices_; /// 頂点数
        u32 numIndices_; /// インデックス数
    };

    inline LoadGeometry::LoadGeometry()
        :vflag_(0)
        ,vsize_(0)
        ,numVertices_(0)
        ,numIndices_(0)
    {
    }
}
#endif //INC_LFRAMEWORK_LOADGEOMETRY_H__
