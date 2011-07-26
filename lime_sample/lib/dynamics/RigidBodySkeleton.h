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

        /// データ解放
        void release();

    private:

        lmath::Matrix34* matrices_;

        u32 numRigidBodies_; /// 剛体数
        btRigidBody** rigidBodies_; /// 剛体

        btCollisionShape** collisionShapes_; /// コリジョン用オブジェクト

        u32 numConstraints_; /// 拘束数
        btTypedConstraint** constraints_; /// 拘束
    };
}
#endif //INC_RIGIDBODYSKELETON_H__
