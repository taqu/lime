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

        u32 vflag_; /// ���_�錾�t���O
        u32 vsize_; /// ���_�T�C�Y
        u32 numVertices_; /// ���_��
        u32 numIndices_; /// �C���f�b�N�X��
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
