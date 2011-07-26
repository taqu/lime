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
    /// ���̃f�[�^
    struct RigidBody
    {
        /// �R���W�����I�u�W�F�N�g�^�C�v
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

        Char name_[NameSize]; /// ���O

        u8 collideGroup_; /// �Փ˃O���[�vID
        u8 shapeType_; /// �V�F�C�v
        u8 type_;      /// �{�[���ʒu�̌v�Z���@
        u8 padding_;

        u16 boneIndex_; /// �֘A�{�[���C���f�b�N�X
        u16 collideMask_; // �Փ˃}�X�N

        lmath::Vector3 size_; /// �T�C�Y
        lmath::Vector3 position_; /// �����ʒu
        lmath::Vector3 rotation_; /// ������]

        f32 mass_; /// ����

        f32 linearDamping_;  /// �ړ�����
        f32 angularDamping_; /// ��]����

        f32 restitution_; /// ����
        f32 friction_;    /// ���C
    };

    /// ����
    lcore::istream& operator>>(lcore::istream& is, RigidBody& rhs);


    //----------------------------------------------------
    //---
    //--- Constraint
    //---
    //----------------------------------------------------
    /// �S���f�[�^
    struct Constraint
    {
        Char name_[NameSize]; /// ���O
        u32 rigidBodyIndex_[2]; /// �Ώۂ̍���

        lmath::Vector3 position_; /// �ڑ��_�̈ʒu
        lmath::Vector3 rotation_; /// �ڑ��_�̈ʒu

        lmath::Vector3 limitLowerTrans_; /// �ړ�����
        lmath::Vector3 limitUpperTrans_; /// �ړ�����

        lmath::Vector3 limitLowerRot_; /// ��]����
        lmath::Vector3 limitUpperRot_; /// ��]����

        f32 stiffness_[6]; /// 6���R�x�̌���
    };

    /// ����
    lcore::istream& operator>>(lcore::istream& is, Constraint& rhs);


    //----------------------------------------------------
    //---
    //--- RigidBodyPack
    //---
    //----------------------------------------------------
    /// ���̕����f�[�^
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
