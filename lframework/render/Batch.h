#ifndef INC_BATCH_H__
#define INC_BATCH_H__
/**
@file Batch.h
@author t-sakai
@date 2010/11/21 create
*/
#include "lrender.h"
#include <lcore/Buffer.h>

namespace lgraphics
{
}

namespace lscene
{
    class Geometry;
    class Material;
}

namespace lrender
{
    class Drawable;

    /**
    @brief 描画バッチ
    */
    class Batch
    {
    public:

        /// コンストラクタ
        Batch();

        /**
        @brief コンストラクタ
        @param drawable ... 描画対象
        @param geometry ... ジオメトリ
        @param material ... マテリアル
        */
        Batch(
            Drawable* drawable,
            lscene::Geometry* geometry,
            lscene::Material* material);

        /// デストラクタ
        ~Batch();

        /// 描画対象取得
        Drawable* getDrawable(){ return drawable_;}
        const Drawable* getDrawable() const{ return drawable_;}

        /// 描画対象セット
        void setDrawable(Drawable* drawable){ drawable_ = drawable;}

        /// ジオメトリ取得
        lscene::Geometry* getGeometry(){ return geometry_;}
        const lscene::Geometry* getGeometry() const{ return geometry_;}

        /// ジオメトリセット
        void setGeometry(lscene::Geometry* geometry){ geometry_ = geometry;}

        /// マテリアル取得
        lscene::Material* getMaterial(){ return material_;}
        const lscene::Material* getMaterial() const{ return material_;}

        /// マテリアルセット
        void setMaterial(lscene::Material* material){material_ = material;}

        /// カメラからの距離セット
        void setLengthToCamera(f32 length){ lengthToCamera_ = length;}

        /// カメラからの距離取得
        f32 getLengthToCamera() const{ return lengthToCamera_;}

        /// スワップ
        void swap(Batch& rhs);

    protected:
        Drawable* drawable_; /// 描画対象

        lscene::Geometry *geometry_; /// ジオメトリ

        lscene::Material *material_; /// マテリアル

        f32 lengthToCamera_;

    private:
        // 代入禁止
        Batch(const Batch&);
        Batch& operator=(const Batch&);
    };
}
#endif //INC_BATCH_H__
