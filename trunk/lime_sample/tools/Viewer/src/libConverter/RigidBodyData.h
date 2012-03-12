#ifndef INC_PMD_RIGIDBODYDATA_H__
#define INC_PMD_RIGIDBODYDATA_H__
/**
@file RigidBodyData.h
@author t-sakai
@date 2011/07/08 create
*/
#include "PmdDef.h"
#include <lmath/Vector3.h>

namespace pmd
{
    static const u32 MaxCollisionGroups = 16;

    //----------------------------------------------------
    //---
    //--- RigidBody
    //---
    //----------------------------------------------------
    /// 剛体データ
    struct RigidBody
    {
        /// コリジョンオブジェクトタイプ
        enum ShapeType
        {
            Shape_Sphere = 0,
            Shape_Box,
            Shape_Capsule,
            Shape_Num,
        };

        enum Type
        {
            Type_Bone =0,
            Type_Dynamics,
            Type_BoneDynamics,
        };

        Char name_[NameSize]; /// 名前

        u8 collideGroup_; /// 衝突グループID
        u8 shapeType_; /// シェイプ
        u8 type_;      /// ボーン位置の計算方法
        u8 padding_;

        u16 boneIndex_; /// 関連ボーンインデックス
        u16 collideMask_; // 衝突マスク

        lmath::Vector3 size_; /// サイズ
        lmath::Vector3 position_; /// 初期位置
        lmath::Vector3 rotation_; /// 初期回転

        f32 mass_; /// 質量

        f32 linearDamping_;  /// 移動減衰
        f32 angularDamping_; /// 回転減衰

        f32 restitution_; /// 反発
        f32 friction_;    /// 摩擦
    };

    /// 入力
    lcore::istream& operator>>(lcore::istream& is, RigidBody& rhs);


    //----------------------------------------------------
    //---
    //--- Constraint
    //---
    //----------------------------------------------------
    /// 拘束データ
    struct Constraint
    {
        Char name_[NameSize]; /// 名前
        u32 rigidBodyIndex_[2]; /// 対象の剛体

        lmath::Vector3 position_; /// 接続点の位置
        lmath::Vector3 rotation_; /// 接続点の位置

        lmath::Vector3 limitLowerTrans_; /// 移動制限
        lmath::Vector3 limitUpperTrans_; /// 移動制限

        lmath::Vector3 limitLowerRot_; /// 回転制限
        lmath::Vector3 limitUpperRot_; /// 回転制限

        f32 stiffness_[6]; /// 6自由度の減衰
    };

    /// 入力
    lcore::istream& operator>>(lcore::istream& is, Constraint& rhs);


    //----------------------------------------------------
    //---
    //--- RigidBodyPack
    //---
    //----------------------------------------------------
    /// 剛体物理データ
    struct RigidBodyPack
    {
    public:
        RigidBodyPack()
            :numRigidBodies_(0)
            ,rigidBodies_(NULL)
            ,numConstraints_(0)
            ,constraints_(NULL)
        {
        }

        RigidBodyPack(
            u32 numRigidBodies,
            RigidBody* rigidBodies,
            u32 numConstraints,
            Constraint* constraints)
            :numRigidBodies_(numRigidBodies)
            ,rigidBodies_(rigidBodies)
            ,numConstraints_(numConstraints)
            ,constraints_(constraints)
        {
            LASSERT(rigidBodies_ != NULL);
            LASSERT(constraints_ != NULL);
        }

        ~RigidBodyPack()
        {
            LIME_DELETE_ARRAY(constraints_);
            LIME_DELETE_ARRAY(rigidBodies_);
        }

        void swap(RigidBodyPack& rhs)
        {
            lcore::swap(numRigidBodies_, rhs.numRigidBodies_);
            lcore::swap(rigidBodies_, rhs.rigidBodies_);
            lcore::swap(numConstraints_, rhs.numConstraints_);
            lcore::swap(constraints_, rhs.constraints_);
        }


        u32 numRigidBodies_;
        RigidBody* rigidBodies_;

        u32 numConstraints_;
        Constraint* constraints_;
    };
}
#endif //INC_PMD_RIGIDBODYDATA_H__
