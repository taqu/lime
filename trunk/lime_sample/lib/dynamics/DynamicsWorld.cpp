/**
@file DynamicsWorld.cpp
@author t-sakai
@date 2011/07/11
*/
#include "DynamicsWorld.h"
#include "DynamicsDebugDraw.h"

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

namespace dynamics
{
    DynamicsWorld DynamicsWorld::instance_;

    DynamicsWorld::DynamicsWorld()
        :debugDraw_(NULL)
        ,broadphase_(NULL)
        ,dispatcher_(NULL)
        ,solver_(NULL)
        ,collisionConfiguration_(NULL)
        ,dynamicsWorld_(NULL)
        ,plane_(NULL)
        ,planeRigidBody_(NULL)
    {
    }

    DynamicsWorld::~DynamicsWorld()
    {
    }

    void DynamicsWorld::initialize(bool debugDraw)
    {
        if(instance_.dynamicsWorld_ != NULL){
            return;
        }

        instance_.collisionConfiguration_ = LIME_NEW btDefaultCollisionConfiguration;
        instance_.dispatcher_ = LIME_NEW btCollisionDispatcher( instance_.collisionConfiguration_ );

        {
            static const f32 maxVal = 2000.0f;
            static const f32 minVal = -2000.0f;

            btVector3 worldAABBMin(minVal, minVal, minVal);
            btVector3 worldAABBMax(maxVal, maxVal, maxVal);

            instance_.broadphase_ = LIME_NEW btAxisSweep3( worldAABBMin, worldAABBMax );
        }

        instance_.solver_ = LIME_NEW btSequentialImpulseConstraintSolver;

        instance_.dynamicsWorld_ = LIME_NEW btDiscreteDynamicsWorld(
            instance_.dispatcher_,
            instance_.broadphase_,
            instance_.solver_,
            instance_.collisionConfiguration_ );

        instance_.dynamicsWorld_->setGravity(btVector3(0.0f,-2.50f*9.8f,0.0f));

        //°–³ŒÀ•½–Êì¬
        instance_.plane_ = LIME_NEW btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), 0.0f);
        btRigidBody::btRigidBodyConstructionInfo info( 0.0f, LIME_NEW btDefaultMotionState(), instance_.plane_ );
        instance_.planeRigidBody_ = LIME_NEW btRigidBody(info);

        instance_.dynamicsWorld_->addRigidBody( instance_.planeRigidBody_ );

        if(debugDraw){
            instance_.debugDraw_ = LIME_NEW DynamicsDebugDraw();
            instance_.debugDraw_->initialize();
            instance_.dynamicsWorld_->setDebugDrawer( instance_.debugDraw_ );
        }

    }

    void DynamicsWorld::terminate()
    {
        if(instance_.dynamicsWorld_ != NULL){
            if(instance_.planeRigidBody_ != NULL){
                instance_.dynamicsWorld_->removeRigidBody( instance_.planeRigidBody_ );
                LIME_DELETE_NONULL instance_.planeRigidBody_->getMotionState();
                LIME_DELETE(instance_.plane_);
                LIME_DELETE(instance_.planeRigidBody_);
            }

            LIME_DELETE(instance_.collisionConfiguration_);
            LIME_DELETE(instance_.dispatcher_);
            LIME_DELETE(instance_.broadphase_);
            LIME_DELETE(instance_.solver_);


            instance_.dynamicsWorld_->setDebugDrawer(NULL);
            LIME_DELETE(instance_.dynamicsWorld_);
        }

        if(instance_.debugDraw_ != NULL){
            instance_.debugDraw_->terminate();
            LIME_DELETE(instance_.debugDraw_);
        }
    }

    void DynamicsWorld::step(f32 steptime)
    {
        dynamicsWorld_->stepSimulation(steptime, 1, (1.0f/30.0f));
    }

    void DynamicsWorld::debugDraw()
    {
        dynamicsWorld_->debugDrawWorld();
        if(debugDraw_ != NULL){
            debugDraw_->flush();
        }

    }
}
