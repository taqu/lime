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

    inline void setTransposed33(lmath::Matrix34& dst, const lmath::Matrix44& src)
    {
        for(s32 i=0; i<3; ++i){
            for(s32 j=0; j<3; ++j){
                dst.m_[i][j] = src.m_[j][i];
            }
        }
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

}

    // ボーンアニメの結果反映、物理演算反映
    class RigidBodySkeleton::MotionStateBoneDynamics : public btMotionState
    {
    public:
        MotionStateBoneDynamics(lmath::Matrix34* matrix, const lmath::Matrix34& matRigidBody)
            :matrix_(matrix)
            ,matRigidBody_(matRigidBody)
            ,invMatRigidBody_(matRigidBody)
            ,parent_(NULL)
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

        void setParent(btRigidBody* parent)
        {
            parent_ = parent;
        }

        void reset()
        {
            LASSERT(parent_ != NULL);
            btTransform worldTrans;
            getWorldTransform(worldTrans);
            parent_->setWorldTransform( worldTrans );
        }

    protected:
        lmath::Matrix34* matrix_;
        lmath::Matrix34 matRigidBody_;
        lmath::Matrix34 invMatRigidBody_;
        btRigidBody* parent_;
    };


    // ボーンアニメの位置反映、物理演算の回転反映
    class RigidBodySkeleton::MotionStateDynamicsRotation : public MotionStateBoneDynamics
    {
    public:
        MotionStateDynamicsRotation(lmath::Matrix34* matrix, const lmath::Matrix34& matRigidBody)
            :MotionStateBoneDynamics(matrix, matRigidBody)
            ,translation_(0.0f, 0.0f, 0.0f)
        {
        }


        virtual ~MotionStateDynamicsRotation()
        {
        }


        //virtual void setWorldTransform(const btTransform &worldTrans)
        //{
        //    if(matrix_ != NULL){
        //        btTransform world(worldTrans);
        //        world.setOrigin(btVector3(translation_.x_, translation_.y_, translation_.z_));
        //        lmath::Matrix44 tmp;
        //        world.getOpenGLMatrix( reinterpret_cast<btScalar*>(&tmp) );
        //        setTransposed(*matrix_, tmp);
        //        matrix_->mul(*matrix_, invMatRigidBody_);

        //        //const btVector3& btTrans = worldTrans.getOrigin();
        //    }
        //}

        void updatePosition()
        {
            LASSERT(parent_ != NULL);
            btTransform worldTrans;
            getWorldTransform(worldTrans);
            parent_->setWorldTransform( worldTrans );

            const btVector3& btTrans = worldTrans.getOrigin();
            translation_.set( btTrans.x(), btTrans.y(), btTrans.z() );
        }

        lmath::Vector3 translation_;
    };


    //-----------------------------------------------------------
    RigidBodySkeleton::RigidBodySkeleton()
        :matrices_(NULL)
        ,numRigidBodies_(0)
        ,rigidBodies_(NULL)
        ,collisionShapes_(NULL)
        ,numConstraints_(0)
        ,constraints_(NULL)
        ,numMotionStateNotDynamicsRots_(0)
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

        u32 lastIndex = numRigidBodies_-1;
        u32 startIndex = 0;

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


            MotionStateBoneDynamics* motionState = (rigidBodies[i].type_ == pmd::RigidBody::Type_BoneDynamics)
                ? LIME_NEW MotionStateBoneDynamics( matrix, matRigidBody )
                : LIME_NEW MotionStateBoneDynamics( matrix, matRigidBody );

            btRigidBody::btRigidBodyConstructionInfo rbInfo(
                rigidBodies[i].mass_,
                motionState,
                collisionShapes_[i],
                localInertia);

            rbInfo.m_linearDamping = rigidBodies[i].linearDamping_;
            rbInfo.m_angularDamping = rigidBodies[i].angularDamping_;
            rbInfo.m_restitution = rigidBodies[i].restitution_;
            rbInfo.m_friction = rigidBodies[i].friction_;

            btRigidBody* rigidBody = LIME_NEW btRigidBody(rbInfo);

            rigidBody->setDeactivationTime(0.8f);
            rigidBody->setSleepingThresholds(1.6f, 2.5f);

            motionState->setParent(rigidBody);

            //物理演算の結果フィードバックのタイプごとに処理
            switch(rigidBodies[i].type_)
            {
            case pmd::RigidBody::Type_Bone: //ボーンの状態を反映
                {//Kinematicオブジェクトで、前から詰める
                    rigidBody->setActivationState(DISABLE_DEACTIVATION);
                    rigidBody->setCollisionFlags( rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);

                    rigidBodies_[startIndex++] = rigidBody;
                    ++numMotionStateNotDynamicsRots_;
                }
                break;

            case pmd::RigidBody::Type_Dynamics: //物理演算の結果反映
                {//前から詰める
                    rigidBodies_[startIndex++] = rigidBody;
                    ++numMotionStateNotDynamicsRots_;
                }
                break;

            case pmd::RigidBody::Type_BoneDynamics: //位置はボーン、回転は物理演算の結果反映
                {//後から詰める
                    rigidBodies_[startIndex++] = rigidBody;
                    ++numMotionStateNotDynamicsRots_;
                    //rigidBodies_[lastIndex--] = rigidBody;
                }
                break;
            };

            u16 group = 0x01U << rigidBodies[i].collideGroup_;
            world->addRigidBody(rigidBody, group, rigidBodies[i].collideMask_);

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
    void RigidBodySkeleton::reset()
    {
        MotionStateBoneDynamics* motionState = NULL;
        for(u32 i=0; i<numRigidBodies_; ++i){
            //g++の-fno-rttiではdynamic_castが使えないため
            motionState = reinterpret_cast<MotionStateBoneDynamics*>( rigidBodies_[i]->getMotionState() );
            motionState->reset();
        }
    }

    //-----------------------------------------------------------
    void RigidBodySkeleton::updateWorldTransform()
    {
        MotionStateDynamicsRotation* motionState = NULL;
        for(u32 i=numMotionStateNotDynamicsRots_; i<numRigidBodies_; ++i){
            //g++の-fno-rttiではdynamic_castが使えないため
            motionState = reinterpret_cast<MotionStateDynamicsRotation*>( rigidBodies_[i]->getMotionState() );
            motionState->updatePosition();
        }
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

        numMotionStateNotDynamicsRots_ = 0;
    }

}
