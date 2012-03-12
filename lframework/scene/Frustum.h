#ifndef INC_LSCENE_FRUSTUM_H__
#define INC_LSCENE_FRUSTUM_H__
/**
@file Frustum.h
@author t-sakai
@date 2011/11/03 create

*/
#include "lscene.h"
#include <lmath/Vector3.h>

namespace lmath
{
    class Sphere;
}

namespace lscene
{
    class Camera;

    class Frustum
    {
    public:
        Frustum()
        {}

        ~Frustum()
        {}

        /**
        @brief ビュー空間の視錘台計算
        */
        void calc(const Camera& camera);

        /**
        @brief ビュー空間の球が視錘台内にあるか
        @param sphere ... 
        */
        bool include(const lmath::Vector3& position, f32 radius);
    private:
        f32 znear_;
        f32 zfar_;

        f32 leftRightX_;
        f32 leftRightZ_;

        f32 topBottomY_;
        f32 topBottomZ_;

        //視錘台法線
        //left  (leftRightX, 0, leftRightZ)
        //right (-leftRightX, 0, leftRightZ)
        //top   (0, topBottomY, topBottomZ)
        //bottom (0, -topBottomY, topBottomZ)
    };
}
#endif //INC_LSCENE_FRUSTUM_H__
