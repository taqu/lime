#ifndef INC_LSCENE_LLOAD_GEOMETRY_H__
#define INC_LSCENE_LLOAD_GEOMETRY_H__
/**
@file load_geometry.h
@author t-sakai
@date 2012/03/31 create
*/
#include "load.h"
#include <lmath/geometry/Sphere.h>

namespace lscene
{
namespace lload
{
    class Geometry
    {
    public:
        inline Geometry();

        u32 vflag_; /// ���_�錾�t���O
        u32 vsize_; /// ���_�T�C�Y
        u32 numVertices_; /// ���_��
        u32 numIndices_; /// �C���f�b�N�X��
    };

    inline Geometry::Geometry()
        :vflag_(0)
        ,vsize_(0)
        ,numVertices_(0)
        ,numIndices_(0)
    {
    }
}
}
#endif //INC_LSCENE_LLOAD_GEOMETRY_H__
