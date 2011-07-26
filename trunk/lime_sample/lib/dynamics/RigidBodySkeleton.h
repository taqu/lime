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

        /// �f�[�^���
        void release();

    private:

        lmath::Matrix34* matrices_;

        u32 numRigidBodies_; /// ���̐�
        btRigidBody** rigidBodies_; /// ����

        btCollisionShape** collisionShapes_; /// �R���W�����p�I�u�W�F�N�g

        u32 numConstraints_; /// �S����
        btTypedConstraint** constraints_; /// �S��
    };
}
#endif //INC_RIGIDBODYSKELETON_H__
