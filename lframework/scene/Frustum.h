#ifndef INC_LSCENE_FRUSTUM_H__
#define INC_LSCENE_FRUSTUM_H__
/**
@file Frustum.h
@author t-sakai
@date 2011/11/03 create

*/
#include "lscene.h"
#include <lmath/geometry/Plane.h>

namespace lmath
{
    class Vector3;
    class Matrix44;
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
        @brief �r���[��Ԃ̎�����v�Z
        */
        void calc(const Camera& camera, f32 zfar);

        /**
        @brief �r���[��Ԃ̋�����������ɂ��邩
        */
        bool contains(const lmath::Vector3& position, f32 radius)
        {
            contains(position.x_, position.y_, position.z_, radius);
        }

        /**
        @brief �r���[��Ԃ̋�����������ɂ��邩
        */
        bool contains(f32 x, f32 y, f32 z, f32 radius);

        /**
        @brief ������̂W���_�擾
        */
        void getPoints(lmath::Vector4* points);

        /**
        @brief ������̂W���_�擾
        */
        void getPoints(lmath::Vector4* points, f32 znear);

        static f32 calcFitZFar(const lmath::Vector4& aabbMin, const lmath::Vector4& aabbMax, const Camera& camera, f32 minz, f32 maxz);

        lmath::Plane planes_[6]; ///���A�E�A��A���A�߁A��
    };

}
#endif //INC_LSCENE_FRUSTUM_H__
