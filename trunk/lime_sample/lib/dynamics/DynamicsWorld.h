#ifndef INC_DYNAMICSWORLD_H__
#define INC_DYNAMICSWORLD_H__
/**
@file DynamicsWorld.h
@author t-sakai
@date 2011/07/11 create
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
class btTypedCostraint;

namespace dynamics
{
    class DynamicsDebugDraw;

    class DynamicsWorld
    {
    public:
        static DynamicsWorld& getInstance(){ return instance_;}
        static void initialize();
        static void terminate();

        btDynamicsWorld* getWorld(){ return dynamicsWorld_; }

        void step(f32 steptime);

        void debugDraw();
    private:
        DynamicsWorld();
        ~DynamicsWorld();

        DynamicsWorld(const DynamicsWorld&);
        DynamicsWorld& operator=(const DynamicsWorld&);

        static DynamicsWorld instance_;

        DynamicsDebugDraw* debugDraw_;

        btBroadphaseInterface*	broadphase_;

        btCollisionDispatcher*	dispatcher_;

        btConstraintSolver*	solver_;

        btDefaultCollisionConfiguration* collisionConfiguration_;

        btDynamicsWorld* dynamicsWorld_;
    };
}
#endif //INC_DYNAMICSWORLD_H__
