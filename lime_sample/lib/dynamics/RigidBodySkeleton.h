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

        /// 剛体の位置をボーンに合わせる
        void setRigidBodyPosition();

        /// データ解放
        void release();

    private:
        lmath::Matrix34* matrices_;

        u32 numRigidBodies_; /// 剛体数
        btRigidBody** rigidBodies_; /// 剛体

        btCollisionShape** collisionShapes_; /// コリジョン用オブジェクト

        u32 numConstraints_; /// 拘束数
        btTypedConstraint** constraints_; /// 拘束

        u32 numTypeDynamicsBodies_;
        btRigidBody** typeDynamicsBodies_;

        u32 numTypeBoneDynamicsBodies_;
        btRigidBody** typeBoneDynamicsBodies_;
    };
}
#endif //INC_RIGIDBODYSKELETON_H__
