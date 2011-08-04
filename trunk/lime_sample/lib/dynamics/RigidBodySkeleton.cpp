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


    inline void setTransposed(btTransform& dst, const lmath::Matrix34& src)
    {
        //btMatrix3x3はrow major?
        btMatrix3x3& basis = dst.getBasis();

        for(s32 i=0; i<3; ++i){
            for(s32 j=0; j<3; ++j){
                basis[i][j] = src.m_[i][j];
            }
        }
        dst.setOrigin( btVector3(src.m_[0][3], src.m_[1][3], src.m_[2][3]) );
    }

    inline void setTransposed(lmath::Matrix34& dst, const btTransform& src)
    {
        //btMatrix3x3はrow major?
        const btMatrix3x3& basis = src.getBasis();

        for(s32 i=0; i<3; ++i){
            for(s32 j=0; j<3; ++j){
                dst.m_[i][j] = basis[i][j];
            }
        }

        const btVector3& origin = src.getOrigin();
        dst.m_[0][3] = origin.x();
        dst.m_[1][3] = origin.y();
        dst.m_[2][3] = origin.z();
    }

}

    // ボーンアニメの結果反映、物理演算反映
    class RigidBodySkeleton::MotionStateKinematic : public btMotionState
    {
    public:
        MotionStateKinematic(const btTransform& rigidBodyWorld, const btTransform& rigidBodyOffset, lmath::Matrix34* matrix)
            :rigidBodyWorld_(rigidBodyWorld)
            ,rigidBodyOffset_(rigidBodyOffset)
            ,matrix_(matrix)
            ,parent_(NULL)
        {
            invRigidBodyWorld_ = rigidBodyWorld_.inverse();
        }


        virtual ~MotionStateKinematic()
        {
        }

        // アニメーション後の剛体位置を取得
        virtual void getWorldTransform(btTransform &worldTrans) const
        {
            lmath::Matrix44 tmp;

            if(matrix_ != NULL){
                btTransform matBone;
                setTransposed(matBone, *matrix_);

                worldTrans = matBone * rigidBodyWorld_;

            }else{
                worldTrans = rigidBodyWorld_;
            }
        }

        virtual void setWorldTransform(const btTransform &worldTrans)
        {
#if 0
            LASSERT(parent_ != NULL);
            LASSERT(matrix_ != NULL);

            btTransform tmp = worldTrans * invRigidBodyWorld_;

            setTransposed(*matrix_, tmp);
#endif
        }

        void setParent(btRigidBody* parent)
        {
            parent_ = parent;
        }

        // 位置リセット
        void reset()
        {
            LASSERT(parent_ != NULL);
            LASSERT(matrix_ != NULL);


            btTransform transform = parent_->getWorldTransform();

            getWorldTransform(transform);
            parent_->setWorldTransform(transform);

            btVector3 zero(0.0f, 0.0f, 0.0f);
            parent_->setLinearVelocity(zero);
            parent_->setAngularVelocity(zero);
            parent_->setInterpolationLinearVelocity(zero);
            parent_->setInterpolationAngularVelocity(zero);
            parent_->setInterpolationWorldTransform(parent_->getWorldTransform());
            parent_->clearForces();

        }

        // スキニング用行列更新
        void updateBoneMatrix()
        {
            LASSERT(parent_ != NULL);
            LASSERT(matrix_ != NULL);

            btTransform tmp = parent_->getWorldTransform() * invRigidBodyWorld_;

            setTransposed(*matrix_, tmp);
        }

        // 剛体の位置をボーンに合わせる
        void updatePosition()
        {
            LASSERT(parent_ != NULL);
            LASSERT(matrix_ != NULL);

            btTransform matBone;
            setTransposed(matBone, *matrix_);

            btTransform mat = matBone * rigidBodyWorld_;
            matBone = matBone * rigidBodyWorld_;

            const btVector3& currentPos = parent_->getWorldTransform().getOrigin();

            btVector3 trans = matBone.getOrigin() - currentPos;
            parent_->translate( trans );
        }

        const btTransform& getRigidBodyWorld() const
        {
            return rigidBodyWorld_;
        }

    protected:
        btTransform rigidBodyWorld_;
        btTransform rigidBodyOffset_;
        btTransform invRigidBodyWorld_;
        lmath::Matrix34* matrix_;
        btRigidBody* parent_;
    };

    //-----------------------------------------------------------
    RigidBodySkeleton::RigidBodySkeleton()
        :matrices_(NULL)
        ,numRigidBodies_(0)
        ,rigidBodies_(NULL)
        ,collisionShapes_(NULL)
        ,numConstraints_(0)
        ,constraints_(NULL)
        ,numTypeDynamicsBodies_(0)
        ,typeDynamicsBodies_(NULL)
        ,numTypeBoneDynamicsBodies_(0)
        ,typeBoneDynamicsBodies_(NULL)
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

        btDiscreteDynamicsWorld* world = DynamicsWorld::getInstance().getWorld();
        LASSERT(world != NULL);

        matrices_ = skeletonPose.getMatrices();

        // 剛体・衝突用オブジェクト生成
        //------------------------------------------------------------
        numRigidBodies_ = numRigidBodies;

        rigidBodies_ = LIME_NEW btRigidBody*[3*numRigidBodies_]; //タイプ別処理用に３倍確保
        typeDynamicsBodies_ = rigidBodies_ + numRigidBodies_; //タイプ別処理用
        typeBoneDynamicsBodies_ = typeDynamicsBodies_ + numRigidBodies_; //タイプ別処理用

        collisionShapes_ = LIME_NEW btCollisionShape*[numRigidBodies_];

        for(u32 i=0; i<numRigidBodies_; ++i){
            switch(rigidBodies[i].shapeType_)
            {
            case pmd::RigidBody::Shape_Sphere:
                collisionShapes_[i] = LIME_NEW 	btSphereShape(rigidBodies[i].size_.x_);
                break;

            case pmd::RigidBody::Shape_Box:
                {
                    collisionShapes_[i] = LIME_NEW 	btBoxShape( btVector3(rigidBodies[i].size_.x_, rigidBodies[i].size_.y_, rigidBodies[i].size_.z_) );
                }
                break;

            case pmd::RigidBody::Shape_Capsule:
            default:
                collisionShapes_[i] = LIME_NEW btCapsuleShape(rigidBodies[i].size_.x_, rigidBodies[i].size_.y_);
                break;
            }

            //ボーン位置、スキニング用マトリックス取得
            lmath::Matrix34* matrix = NULL;
            lmath::Vector3 bonePos(0.0f, 0.0f, 0.0f);
            if(rigidBodies[i].boneIndex_ != 0xFFFFU){
                matrix = matrices_ + rigidBodies[i].boneIndex_;
                bonePos = skeleton.getJoint( rigidBodies[i].boneIndex_ ).getPosition();
                //bonePos.mul( bonePos, *matrix );
            }else if(skeleton.getNumJoints()>0){
                matrix = matrices_;
                bonePos = skeleton.getJoint(0).getPosition();
            }

            //重量は1.0以上にならない
            btVector3 localInertia(0.0f, 0.0f, 0.0f);
            
            f32 mass = (rigidBodies[i].type_ == pmd::RigidBody::Type_Bone)? 0.0f : rigidBodies[i].mass_;


            if(mass>1.0f){
                mass = 1.0f;
            }

            if(mass>0.0f){
                collisionShapes_[i]->calculateLocalInertia(mass, localInertia);
            }

            btTransform matRigidBody;
            matRigidBody.getBasis().setEulerZYX(rigidBodies[i].rotation_.x_, rigidBodies[i].rotation_.y_, rigidBodies[i].rotation_.z_);
            matRigidBody.setOrigin( btVector3(rigidBodies[i].position_.x_, rigidBodies[i].position_.y_, rigidBodies[i].position_.z_) );

            btTransform matRigidBodyWorld;
            matRigidBodyWorld.setIdentity();
            matRigidBodyWorld.setOrigin( btVector3(bonePos.x_, bonePos.y_, bonePos.z_) );
            matRigidBodyWorld = matRigidBodyWorld * matRigidBody;


            MotionStateKinematic* motionState = LIME_NEW MotionStateKinematic( matRigidBodyWorld, matRigidBody, matrix );

            btRigidBody::btRigidBodyConstructionInfo rbInfo(
                mass,
                motionState,
                collisionShapes_[i],
                localInertia);

            rbInfo.m_linearDamping = rigidBodies[i].linearDamping_;
            rbInfo.m_angularDamping = rigidBodies[i].angularDamping_;
            rbInfo.m_restitution = rigidBodies[i].restitution_;
            rbInfo.m_friction = rigidBodies[i].friction_;
            rbInfo.m_additionalDamping = true;

            btRigidBody* rigidBody = LIME_NEW btRigidBody(rbInfo);

            rigidBody->setDeactivationTime(0.8f);
            rigidBody->setSleepingThresholds(0.0f, 0.0f);

            motionState->setParent(rigidBody);

            //物理演算の結果フィードバックのタイプごとに処理
            switch(rigidBodies[i].type_)
            {
            case pmd::RigidBody::Type_Bone: //ボーンの状態を反映
                {//Kinematicオブジェクト
                    rigidBody->setActivationState(DISABLE_DEACTIVATION);
                    rigidBody->setCollisionFlags( rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
                }
                break;

            case pmd::RigidBody::Type_Dynamics: //物理演算の結果反映
                {
                    //関連ボーンがあれば、タイプ別処理用の配列にも入れる
                    if(matrix != NULL){
                        typeDynamicsBodies_[numTypeDynamicsBodies_++] = rigidBody;
                    }
                }
                break;

            case pmd::RigidBody::Type_BoneDynamics: //位置はボーン、回転は物理演算の結果反映
                {
                    //関連ボーンがあれば、タイプ別処理用の配列にも入れる
                    if(matrix != NULL){
                        typeBoneDynamicsBodies_[numTypeBoneDynamicsBodies_++] = rigidBody;
                    }
                }
                break;
            };

            rigidBodies_[i] = rigidBody;
            u16 group = 0x01U << rigidBodies[i].collideGroup_;
            world->addRigidBody(rigidBody, (short)group, (short)rigidBodies[i].collideMask_);

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

            const lmath::Vector3& rot = constraints[i].rotation_;
            const lmath::Vector3& pos = constraints[i].position_;

            u16 indexA = constraints[i].rigidBodyIndex_[0];
            u16 indexB = constraints[i].rigidBodyIndex_[1];

            btRigidBody* bodyA = rigidBodies_[indexA];
            btRigidBody* bodyB = rigidBodies_[indexB];

            const MotionStateKinematic* motionStateA = reinterpret_cast<const MotionStateKinematic*>(bodyA->getMotionState());
            const MotionStateKinematic* motionStateB = reinterpret_cast<const MotionStateKinematic*>(bodyB->getMotionState());

            btTransform matConstraint;
            matConstraint.getBasis().setEulerZYX( rot.x_, rot.y_, rot.z_ );
            matConstraint.setOrigin( btVector3(pos.x_, pos.y_, pos.z_) );

            transformA = motionStateA->getRigidBodyWorld().inverse();
            transformB = motionStateB->getRigidBodyWorld().inverse();

            transformA = transformA * matConstraint;
            transformB = transformB * matConstraint;

            btGeneric6DofSpringConstraint* constraint = LIME_NEW btGeneric6DofSpringConstraint(*bodyA, *bodyB, transformA, transformB, true);

            set(tmp, constraints[i].limitLowerTrans_);
            constraint->setLinearLowerLimit(tmp);

            set(tmp, constraints[i].limitUpperTrans_);
            constraint->setLinearUpperLimit(tmp);

            set(tmp, constraints[i].limitLowerRot_);
            constraint->setAngularLowerLimit(tmp);

            set(tmp, constraints[i].limitUpperRot_);
            constraint->setAngularUpperLimit(tmp);


            for(s32 j=0; j<6; ++j){
                if(constraints[i].stiffness_[j]<0.001f){
                    constraint->enableSpring(j, false);
                }else{
                    constraint->enableSpring(j, true);
                }
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
        MotionStateKinematic* motionState = NULL;
        for(u32 i=0; i<numTypeDynamicsBodies_; ++i){
            //g++の-fno-rttiではdynamic_castが使えないため
            motionState = reinterpret_cast<MotionStateKinematic*>( typeDynamicsBodies_[i]->getMotionState() );
            motionState->reset();
        }

        for(u32 i=0; i<numTypeBoneDynamicsBodies_; ++i){
            //g++の-fno-rttiではdynamic_castが使えないため
            motionState = reinterpret_cast<MotionStateKinematic*>( typeBoneDynamicsBodies_[i]->getMotionState() );
            motionState->reset();
        }
    }

    //-----------------------------------------------------------
    void RigidBodySkeleton::updateBoneMatrix()
    {
#if 1
        MotionStateKinematic* motionState = NULL;

        for(u32 i=0; i<numTypeDynamicsBodies_; ++i){
            //g++の-fno-rttiではdynamic_castが使えないため
            motionState = reinterpret_cast<MotionStateKinematic*>( typeDynamicsBodies_[i]->getMotionState() );
            motionState->updateBoneMatrix();
        }

        for(u32 i=0; i<numTypeBoneDynamicsBodies_; ++i){
            //g++の-fno-rttiではdynamic_castが使えないため
            motionState = reinterpret_cast<MotionStateKinematic*>( typeBoneDynamicsBodies_[i]->getMotionState() );
            motionState->updateBoneMatrix();
        }
#endif
    }

    //-----------------------------------------------------------
    // 剛体の位置をボーンに合わせる
    void RigidBodySkeleton::setRigidBodyPosition()
    {
        MotionStateKinematic* motionState = NULL;
        for(u32 i=0; i<numTypeBoneDynamicsBodies_; ++i){
            //g++の-fno-rttiではdynamic_castが使えないため
            motionState = reinterpret_cast<MotionStateKinematic*>( typeBoneDynamicsBodies_[i]->getMotionState() );
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

        numTypeDynamicsBodies_ = 0;
        typeDynamicsBodies_ = NULL;

        numTypeBoneDynamicsBodies_ = 0;
        typeBoneDynamicsBodies_ = NULL;
    }

}
