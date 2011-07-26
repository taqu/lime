LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

BULLETSRC_PATH := bullet-2.78/src
LOCAL_C_INCLUDES := $(LOCAL_PATH)/bullet-2.78/src
LOCAL_MODULE    := bullet
LOCAL_SRC_FILES := \
        $(BULLETSRC_PATH)/LinearMath/btQuickprof.cpp\
        $(BULLETSRC_PATH)/LinearMath/btGeometryUtil.cpp\
        $(BULLETSRC_PATH)/LinearMath/btAlignedAllocator.cpp\
        $(BULLETSRC_PATH)/LinearMath/btSerializer.cpp\
        $(BULLETSRC_PATH)/LinearMath/btConvexHull.cpp\
        $(BULLETSRC_PATH)/LinearMath/btConvexHullComputer.cpp\
\
        $(BULLETSRC_PATH)/BulletCollision/NarrowPhaseCollision/btRaycastCallback.cpp\
        $(BULLETSRC_PATH)/BulletCollision/NarrowPhaseCollision/btMinkowskiPenetrationDepthSolver.cpp\
        $(BULLETSRC_PATH)/BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.cpp\
        $(BULLETSRC_PATH)/BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.cpp\
        $(BULLETSRC_PATH)/BulletCollision/NarrowPhaseCollision/btGjkConvexCast.cpp\
        $(BULLETSRC_PATH)/BulletCollision/NarrowPhaseCollision/btPersistentManifold.cpp\
        $(BULLETSRC_PATH)/BulletCollision/NarrowPhaseCollision/btConvexCast.cpp\
        $(BULLETSRC_PATH)/BulletCollision/NarrowPhaseCollision/btPolyhedralContactClipping.cpp\
        $(BULLETSRC_PATH)/BulletCollision/NarrowPhaseCollision/btContinuousConvexCollision.cpp\
        $(BULLETSRC_PATH)/BulletCollision/NarrowPhaseCollision/btGjkPairDetector.cpp\
        $(BULLETSRC_PATH)/BulletCollision/NarrowPhaseCollision/btGjkEpa2.cpp\
        $(BULLETSRC_PATH)/BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionDispatch/btActivatingCollisionAlgorithm.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionDispatch/btCollisionObject.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionDispatch/btEmptyCollisionAlgorithm.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionDispatch/btGhostObject.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionDispatch/btSphereBoxCollisionAlgorithm.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionDispatch/btCollisionDispatcher.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionDispatch/btSimulationIslandManager.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionDispatch/btBoxBoxDetector.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionDispatch/btConvexPlaneCollisionAlgorithm.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionDispatch/btConvexConcaveCollisionAlgorithm.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionDispatch/btBoxBoxCollisionAlgorithm.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionDispatch/btBox2dBox2dCollisionAlgorithm.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionDispatch/SphereTriangleDetector.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionDispatch/btInternalEdgeUtility.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionDispatch/btManifoldResult.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionDispatch/btCollisionWorld.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionDispatch/btSphereTriangleCollisionAlgorithm.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionDispatch/btConvexConvexAlgorithm.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionDispatch/btConvex2dConvex2dAlgorithm.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionDispatch/btUnionFind.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionDispatch/btCompoundCollisionAlgorithm.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btTetrahedronShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btShapeHull.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btMinkowskiSumShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btCompoundShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btConeShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btConvexPolyhedron.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btMultiSphereShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btUniformScalingShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btSphereShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btTriangleMeshShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btTriangleBuffer.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btStaticPlaneShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btPolyhedralConvexShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btEmptyShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btCollisionShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btConvexShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btConvex2dShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btConvexInternalShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btConvexHullShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btTriangleCallback.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btCapsuleShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btConvexTriangleMeshShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btConcaveShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btConvexPointCloudShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btBoxShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btBox2dShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btOptimizedBvh.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btMultimaterialTriangleMeshShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btCylinderShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btStridingMeshInterface.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btTriangleIndexVertexMaterialArray.cpp\
        $(BULLETSRC_PATH)/BulletCollision/CollisionShapes/btTriangleMesh.cpp\
        $(BULLETSRC_PATH)/BulletCollision/BroadphaseCollision/btAxisSweep3.cpp\
        $(BULLETSRC_PATH)/BulletCollision/BroadphaseCollision/btOverlappingPairCache.cpp\
        $(BULLETSRC_PATH)/BulletCollision/BroadphaseCollision/btDbvtBroadphase.cpp\
        $(BULLETSRC_PATH)/BulletCollision/BroadphaseCollision/btMultiSapBroadphase.cpp\
        $(BULLETSRC_PATH)/BulletCollision/BroadphaseCollision/btDispatcher.cpp\
        $(BULLETSRC_PATH)/BulletCollision/BroadphaseCollision/btBroadphaseProxy.cpp\
        $(BULLETSRC_PATH)/BulletCollision/BroadphaseCollision/btQuantizedBvh.cpp\
        $(BULLETSRC_PATH)/BulletCollision/BroadphaseCollision/btCollisionAlgorithm.cpp\
        $(BULLETSRC_PATH)/BulletCollision/BroadphaseCollision/btDbvt.cpp\
        $(BULLETSRC_PATH)/BulletCollision/BroadphaseCollision/btSimpleBroadphase.cpp\
        $(BULLETSRC_PATH)/BulletCollision/Gimpact/btGImpactBvh.cpp\
        $(BULLETSRC_PATH)/BulletCollision/Gimpact/btGImpactQuantizedBvh.cpp\
        $(BULLETSRC_PATH)/BulletCollision/Gimpact/btTriangleShapeEx.cpp\
        $(BULLETSRC_PATH)/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.cpp\
        $(BULLETSRC_PATH)/BulletCollision/Gimpact/btGImpactShape.cpp\
        $(BULLETSRC_PATH)/BulletCollision/Gimpact/gim_box_set.cpp\
        $(BULLETSRC_PATH)/BulletCollision/Gimpact/gim_contact.cpp\
        $(BULLETSRC_PATH)/BulletCollision/Gimpact/gim_memory.cpp\
        $(BULLETSRC_PATH)/BulletCollision/Gimpact/gim_tri_collision.cpp\
\
        $(BULLETSRC_PATH)/BulletDynamics/Dynamics/btContinuousDynamicsWorld.cpp\
        $(BULLETSRC_PATH)/BulletDynamics/Dynamics/btRigidBody.cpp\
        $(BULLETSRC_PATH)/BulletDynamics/Dynamics/btSimpleDynamicsWorld.cpp\
        $(BULLETSRC_PATH)/BulletDynamics/Dynamics/Bullet-C-API.cpp\
        $(BULLETSRC_PATH)/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.cpp\
        $(BULLETSRC_PATH)/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.cpp\
        $(BULLETSRC_PATH)/BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.cpp\
        $(BULLETSRC_PATH)/BulletDynamics/ConstraintSolver/btSolve2LinearConstraint.cpp\
        $(BULLETSRC_PATH)/BulletDynamics/ConstraintSolver/btPoint2PointConstraint.cpp\
        $(BULLETSRC_PATH)/BulletDynamics/ConstraintSolver/btTypedConstraint.cpp\
        $(BULLETSRC_PATH)/BulletDynamics/ConstraintSolver/btContactConstraint.cpp\
        $(BULLETSRC_PATH)/BulletDynamics/ConstraintSolver/btSliderConstraint.cpp\
        $(BULLETSRC_PATH)/BulletDynamics/ConstraintSolver/btConeTwistConstraint.cpp\
        $(BULLETSRC_PATH)/BulletDynamics/ConstraintSolver/btHingeConstraint.cpp\
        $(BULLETSRC_PATH)/BulletDynamics/ConstraintSolver/btHinge2Constraint.cpp\
        $(BULLETSRC_PATH)/BulletDynamics/ConstraintSolver/btUniversalConstraint.cpp\
        $(BULLETSRC_PATH)/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.cpp\
        $(BULLETSRC_PATH)/BulletDynamics/Vehicle/btWheelInfo.cpp\
        $(BULLETSRC_PATH)/BulletDynamics/Vehicle/btRaycastVehicle.cpp\
        $(BULLETSRC_PATH)/BulletDynamics/Character/btKinematicCharacterController.cpp\

include $(BUILD_STATIC_LIBRARY)
