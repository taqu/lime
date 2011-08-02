#ifndef INC_RIGIDBODYSKELETON_H__
#define INC_RIGIDBODYSKELETON_H__
/**
@file RigidBodySkeleton.h
@author t-sakai
@date 2011/07/08 create
*/
#include "dynamics.h"

//-------------------------------
//--- Bullet
class btBroadphaseInterface;
class btCollisionDispatcher;
class btConstraintSolver;
class btDefaultCollisionConfiguration;
class btDynamicsWorld;
class btCollisionShape;
class btRigidBody;
class btTypedConstraint;


namespace lanim
{
    class SkeletonPose;
    class Skeleton;
}

namespace pmd
{
    struct RigidBody;
    struct Constraint;
    struct RigidBodyPack;
}

namespace dynamics
{
    //----------------------------------------------------
    //---
    //--- RigidBodySkeleton
    //---
    //----------------------------------------------------
    class RigidBodySkeleton
    {
    public:
        class MotionStateKinematic;

        RigidBodySkeleton();
        ~RigidBodySkeleton();

        /**
        @param skeletonPose
        @param skeleton
        @param rigidBodyPack
        */
        bool create(
            lanim::SkeletonPose& skeletonPose,
            lanim::Skeleton& skeleton,
            pmd::RigidBodyPack& rigidBodyPack);

        void reset();

        void updateBoneMatrix();

        /// ���̂̈ʒu���{�[���ɍ��킹��
        void setRigidBodyPosition();

        /// �f�[�^���
        void release();

    private:
        lmath::Matrix34* matrices_;

        u32 numRigidBodies_; /// ���̐�
        btRigidBody** rigidBodies_; /// ����

        btCollisionShape** collisionShapes_; /// �R���W�����p�I�u�W�F�N�g

        u32 numConstraints_; /// �S����
        btTypedConstraint** constraints_; /// �S��

        u32 numTypeDynamicsBodies_;
        btRigidBody** typeDynamicsBodies_;

        u32 numTypeBoneDynamicsBodies_;
        btRigidBody** typeBoneDynamicsBodies_;
    };
}
#endif //INC_RIGIDBODYSKELETON_H__
