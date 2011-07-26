/**
@file RigidBodySkeleton.cpp
@author t-sakai
@date 2011/07/08
*/
#include "RigidBodySkeleton.h"

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <lframework/anim/Skeleton.h>
#include <lframework/anim/SkeletonPose.h>
#include <lframework/anim/JointPose.h>

#include <RigidBodyData.h>

#include "DynamicsWorld.h"

namespace dynamics
{
namespace
{
    inline void set(btVector3& lhs, const lmath::Vector3& rhs)
    {
        lhs.setValue( rhs.x_, rhs.y_, rhs.z_ );
    }

    inline void setTransposed(lmath::Matrix44& dst, const lmath::Matrix34& src)
    {
        for(s32 i=0; i<3; ++i){
            for(s32 j=0; j<3; ++j){
                dst.m_[i][j] = src.m_[j][i];
            }
        }
        dst.m_[0][3] = dst.m_[1][3] = dst.m_[2][3] = 0.0f;
        dst.m_[3][0] = src.m_[0][3]; dst.m_[3][1] = src.m_[1][3]; dst.m_[3][2] = src.m_[2][3];
        dst.m_[3][3] = 1.0f;
    }

    inline void setTransposed(lmath::Matrix34& dst, const lmath::Matrix44& src)
    {
        for(s32 i=0; i<3; ++i){
            for(s32 j=0; j<3; ++j){
                dst.m_[i][j] = src.m_[j][i];
            }
        }
        dst.m_[0][3] = src.m_[3][0]; dst.m_[1][3] = src.m_[3][1]; dst.m_[2][3] = src.m_[3][2];
    }


    void setTransform(btTransform& transform, const lmath::Vector3& trans, const lmath::Vector3& rot, const lanim::JointPose& jointPose)
    {
        btQuaternion quaternion( rot.x_, rot.y_, rot.z_ );
        transform.setIdentity();
        transform.setOrigin( btVector3(trans.x_, trans.y_, trans.z_) );
        transform.setRotation( quaternion );

        btQuaternion quaternion2( jointPose.rotation_.x_, jointPose.rotation_.y_, jointPose.rotation_.z_, jointPose.rotation_.w_);
        btTransform jointTrans(quaternion2, btVector3( jointPose.translation_.x_, jointPose.translation_.y_, jointPose.translation_.z_ ) );

        transform *= jointTrans;
    }

    void setTransformRelative(btTransform& transform, const pmd::RigidBody& rigidBody, const lmath::Vector3& trans, const lmath::Vector3& rot, const lanim::Joint* joints)
    {
        lmath::Matrix34 mat;
        mat.identity();
        mat.rotateZ(-rot.z_);
        mat.rotateY(-rot.y_);
        mat.rotateX(-rot.x_);
        mat.translate(trans);

        lmath::Matrix34 matRigidBody;
        matRigidBody.identity();
        matRigidBody.rotateZ(-rigidBody.rotation_.z_);
        matRigidBody.rotateY(-rigidBody.rotation_.y_);
        matRigidBody.rotateX(-rigidBody.rotation_.x_);
        matRigidBody.translate(rigidBody.position_);

        if(rigidBody.boneIndex_ != 0xFFFFU){
            const lanim::Joint& joint = joints[rigidBody.boneIndex_];
            matRigidBody.translate(joint.getPosition());
        }

        matRigidBody.invert();
        mat.mul(matRigidBody, mat);

        lmath::Matrix44 tmp;
        setTransposed(tmp, mat);
        transform.setFromOpenGLMatrix(reinterpret_cast<btScalar*>(&tmp));
    }


    // ボーンアニメの結果反映、物理演算反映
    class MotionStateBoneDynamics : public btMotionState
    {
    public:
        MotionStateBoneDynamics(lmath::Matrix34* matrix, const lmath::Matrix34& matRigidBody)
            :matrix_(matrix)
            ,matRigidBody_(matRigidBody)
            ,invMatRigidBody_(matRigidBody)
        {
            invMatRigidBody_.invert();
        }


        virtual ~MotionStateBoneDynamics()
        {
        }

        virtual void getWorldTransform(btTransform &worldTrans) const
        {
            lmath::Matrix44 tmp;

            if(matrix_ != NULL){
                lmath::Matrix34 mat(*matrix_);

                mat.mul(mat, matRigidBody_);

                setTransposed(tmp, mat);

            }else{
                setTransposed(tmp, matRigidBody_);
            }

            worldTrans.setFromOpenGLMatrix( reinterpret_cast<const btScalar*>(&tmp) );
        }

        virtual void setWorldTransform(const btTransform &worldTrans)
        {
            if(matrix_ != NULL){
                lmath::Matrix44 tmp;
                worldTrans.getOpenGLMatrix( reinterpret_cast<btScalar*>(&tmp) );
                setTransposed(*matrix_, tmp);
                matrix_->mul(*matrix_, invMatRigidBody_);
            }
        }

    private:
        lmath::Matrix34* matrix_;
        lmath::Matrix34 matRigidBody_;
        lmath::Matrix34 invMatRigidBody_;
    };

}

    //-----------------------------------------------------------
    RigidBodySkeleton::RigidBodySkeleton()
        :matrices_(NULL)
        ,numRigidBodies_(0)
        ,rigidBodies_(NULL)
        ,collisionShapes_(NULL)
        ,numConstraints_(0)
        ,constraints_(NULL)
    {
    }

    //-----------------------------------------------------------
    RigidBodySkeleton::~RigidBodySkeleton()
    {
        release();
    }

    //-----------------------------------------------------------
    bool RigidBodySkeleton::create(
        lanim::SkeletonPose& skeletonPose,
        lanim::Skeleton& skeleton,
        pmd::RigidBodyPack& rigidBodyPack)
    {
        u32 numRigidBodies = rigidBodyPack.numRigidBodies_;
        pmd::RigidBody* rigidBodies = rigidBodyPack.rigidBodies_;

        u32 numConstraints = rigidBodyPack.numConstraints_;
        pmd::Constraint* constraints = rigidBodyPack.constraints_;

        LASSERT( (0==numRigidBodies && rigidBodies == NULL)
            || (0<numRigidBodies && rigidBodies != NULL));

        LASSERT( (0==numConstraints && constraints == NULL)
            || (0<numConstraints && constraints != NULL));


        release(); //解放

        btDynamicsWorld* world = DynamicsWorld::getInstance().getWorld();
        LASSERT(world != NULL);

        matrices_ = skeletonPose.getMatrices();

        // 剛体・衝突用オブジェクト生成
        //------------------------------------------------------------
        numRigidBodies_ = numRigidBodies;

        rigidBodies_ = LIME_NEW btRigidBody*[numRigidBodies_];
        collisionShapes_ = LIME_NEW btCollisionShape*[numRigidBodies_];

        lanim::JointPose* jointPoses = skeletonPose.getPoses();

        for(u32 i=0; i<numRigidBodies_; ++i){
            switch(rigidBodies[i].shapeType_)
            {
            case pmd::RigidBody::Shape_Sphere:
                collisionShapes_[i] = LIME_NEW 	btSphereShape(1.0f);
                break;

            case pmd::RigidBody::Shape_Box:
                {
                    btVector3 size(1.0f, 1.0f, 1.0f);
                    collisionShapes_[i] = LIME_NEW 	btBoxShape(size);
                }
                break;

            case pmd::RigidBody::Shape_Capsule:
            default:
                collisionShapes_[i] = LIME_NEW btCapsuleShape(1.0f, 1.0f);
                break;
            }

            collisionShapes_[i]->setLocalScaling( btVector3( rigidBodies[i].size_.x_, rigidBodies[i].size_.y_, rigidBodies[i].size_.z_ ) );

            lmath::Matrix34* matrix = NULL;
            lmath::Vector3 bonePos(0.0f, 0.0f, 0.0f); //ボーン位置
            if(rigidBodies[i].boneIndex_ != 0xFFFFU){
                matrix = matrices_ + rigidBodies[i].boneIndex_;
                bonePos = skeleton.getJoint( rigidBodies[i].boneIndex_ ).getPosition();
            }

            btVector3 localInertia(0.0f, 0.0f, 0.0f);
            if(rigidBodies[i].mass_>0.0f){
                collisionShapes_[i]->calculateLocalInertia(rigidBodies[i].mass_, localInertia);
            }

            lmath::Matrix34 matRigidBody;
            matRigidBody.identity();
            matRigidBody.rotateZ( -rigidBodies[i].rotation_.z_ );
            matRigidBody.rotateY( -rigidBodies[i].rotation_.y_ );
            matRigidBody.rotateX( -rigidBodies[i].rotation_.x_ );
            
            matRigidBody.translate( rigidBodies[i].position_ );
            matRigidBody.translate(bonePos);


            btMotionState* motionState = LIME_NEW MotionStateBoneDynamics( matrix, matRigidBody );

            btRigidBody::btRigidBodyConstructionInfo rbInfo(
                rigidBodies[i].mass_,
                motionState,
                collisionShapes_[i],
                localInertia);

            rbInfo.m_linearDamping = rigidBodies[i].linearDamping_;
            rbInfo.m_angularDamping = rigidBodies[i].angularDamping_;
            rbInfo.m_restitution = rigidBodies[i].restitution_;
            rbInfo.m_friction = rigidBodies[i].friction_;

            rigidBodies_[i] = LIME_NEW btRigidBody(rbInfo);

            rigidBodies_[i]->setDeactivationTime(0.8f);
            rigidBodies_[i]->setSleepingThresholds(1.6f, 2.5f);

            switch(rigidBodies[i].type_)
            {
            case pmd::RigidBody::Type_Bone:
            //case pmd::RigidBody::Type_Dynamics:
            case pmd::RigidBody::Type_BoneDynamics:
                rigidBodies_[i]->setActivationState(DISABLE_DEACTIVATION);
                rigidBodies_[i]->setCollisionFlags( rigidBodies_[i]->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
                break;
            };

            u16 group = 0x01U << rigidBodies[i].collideGroup_;
            world->addRigidBody(rigidBodies_[i], group, rigidBodies[i].collideMask_);
        }

        // 拘束生成
        //------------------------------------------------------------
        numConstraints_ = numConstraints;
        constraints_ = LIME_NEW btTypedConstraint*[numConstraints_];

        btTransform transformA, transformB;
        transformB.setIdentity();

        btVector3 tmp;

        for(u32 i=0; i<numConstraints_; ++i){

            if(constraints[i].rigidBodyIndex_[0] >= numRigidBodies_){
                continue;
            }

            if(constraints[i].rigidBodyIndex_[1] >= numRigidBodies_){
                continue;
            }

            u16 indexA = constraints[i].rigidBodyIndex_[0];
            u16 indexB = constraints[i].rigidBodyIndex_[1];

            btRigidBody* bodyA = rigidBodies_[indexA];
            btRigidBody* bodyB = rigidBodies_[indexB];

            const lanim::Joint* joints = &skeleton.getJoint(0);

            setTransformRelative( transformA, rigidBodies[indexA], constraints[i].position_, constraints[i].rotation_, joints);
            setTransformRelative( transformB, rigidBodies[indexB], constraints[i].position_, constraints[i].rotation_, joints);

            btGeneric6DofSpringConstraint* constraint = LIME_NEW btGeneric6DofSpringConstraint(*bodyA, *bodyB, transformA, transformB, false);

            set(tmp, constraints[i].limitLowerTrans_);
            constraint->setLinearLowerLimit(tmp);

            set(tmp, constraints[i].limitUpperTrans_);
            constraint->setLinearUpperLimit(tmp);

            set(tmp, constraints[i].limitLowerRot_);
            constraint->setAngularLowerLimit(tmp);

            set(tmp, constraints[i].limitUpperRot_);
            constraint->setAngularUpperLimit(tmp);

            for(s32 j=0; j<6; ++j){
                constraint->enableSpring(j, true);
                constraint->setStiffness(j, constraints[i].stiffness_[j]);
            }

            constraints_[i] = constraint;

            world->addConstraint( constraints_[i] );
        }

        return true;
    }

    //-----------------------------------------------------------
    // データ解放
    void RigidBodySkeleton::release()
    {
        btDynamicsWorld* world = DynamicsWorld::getInstance().getWorld();
        LASSERT(world != NULL);

        //ジョイント制約クラス解放
        for(u32 i=0; i<numConstraints_; ++i){
            world->removeConstraint( constraints_[i] );
            LIME_DELETE(constraints_[i]);
        }
        numConstraints_ = 0;
        LIME_DELETE_ARRAY(constraints_);

        //剛体とコリジョンクラス解放
        for(u32 i=0; i<numRigidBodies_; ++i){
            world->removeRigidBody( rigidBodies_[i] );
            LIME_DELETE_NONULL rigidBodies_[i]->getMotionState();
            LIME_DELETE(rigidBodies_[i]);
            LIME_DELETE(collisionShapes_[i]);
        }
        numRigidBodies_ = 0;
        LIME_DELETE_ARRAY(collisionShapes_);
        LIME_DELETE_ARRAY(rigidBodies_);
    }

}
