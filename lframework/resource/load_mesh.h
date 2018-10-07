#ifndef INC_LFRAMEWORK_LOADMESH_H_
#define INC_LFRAMEWORK_LOADMESH_H_
/**
@file load_mesh.h
@author t-sakai
@date 2016/11/23 create
*/
#include "load.h"
#include <lmath/geometry/Sphere.h>

namespace lfw
{
    class LoadMesh
    {
    public:
        //Char name_[MaxNameSize];
        s16 geometry_; /// ジオメトリインデックス
        s16 material_; /// マテリアルインデックス
        u32 indexOffset_;
        u32 numIndices_; /// インデックス数
        lmath::Sphere sphere_; /// 境界球
    };
}
#endif //INC_LFRAMEWORK_LOADMESH_H_
