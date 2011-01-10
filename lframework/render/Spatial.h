#ifndef INC_SPATIAL_H__
#define INC_SPATIAL_H__
/**
@file Spatial.h
@author t-sakai
@date 2009/08/15 create
*/
#include <lmath/lmath.h>

namespace lrender
{
    /**
    @brief オブジェクトの位置
    */
    class Spatial
    {
    public:
        /**
        @brief ワールドマトリックスセット
        */
        void setWorldMatrix(const lmath::Matrix43& matrix)
        {
            matrix_ = matrix;
        }

        /**
        @brief ワールドマトリックス取得
        */
        const lmath::Matrix43& getWorldMatrix() const
        {
            return matrix_;
        }

        /**
        @brief ワールドマトリックス取得

        直に触りたいこともあるよね
        */
        lmath::Matrix43& getWorldMatrix()
        {
            return matrix_;
        }

        /**
        @brief 境界球取得
        */
        const lmath::Vector4& getBSphere() const
        {
            return BSphere_;
        }

        /**
        @brief 境界球セット
        */
        void setBSphere(const lmath::Vector4& bsphere)
        {
            BSphere_ = bsphere;
        }

    protected:
        Spatial()
        {
            matrix_.identity();
            BSphere_.set(0.0f, 0.0f, 0.0f, 1.0f);
        }

        ~Spatial(){}

        lmath::Matrix43 matrix_;
        lmath::Vector4 BSphere_; //境界球
    };
}

#endif //INC_SPATIAL_H__
