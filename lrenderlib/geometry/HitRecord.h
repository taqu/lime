#ifndef INC_LRENDER_HITRECORD_H__
#define INC_LRENDER_HITRECORD_H__
/**
@file HitRecord.h
@author t-sakai
@date 2010/01/15 create

*/
#include "../lrendercore.h"

namespace lrender
{
    class Shape;

    struct HitRecord
    {
        f64 t_; //光線の半直線距離パラメータ
        f64 u_;
        f64 v_;
        Vector3 position_;
        s32 index_;
        const Shape *shape_;
    };
}

#endif //INC_LRENDER_HITRECORD_H__
