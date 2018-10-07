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
        s16 geometry_; /// �W�I���g���C���f�b�N�X
        s16 material_; /// �}�e���A���C���f�b�N�X
        u32 indexOffset_;
        u32 numIndices_; /// �C���f�b�N�X��
        lmath::Sphere sphere_; /// ���E��
    };
}
#endif //INC_LFRAMEWORK_LOADMESH_H_
