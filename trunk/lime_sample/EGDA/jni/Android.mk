LOCAL_PATH := $(call my-dir)

##################################
# lcore
include $(CLEAR_VARS)

LOCAL_MODULE    := lcore
LOCAL_SRC_FILES := $(LIME_DIR)/lcore/lcore.cpp\
                   $(LIME_DIR)/lcore/liostream.cpp\
                   $(LIME_DIR)/lcore/HashMap.cpp\
                   $(LIME_DIR)/lcore/Buffer.cpp\
                   $(LIME_DIR)/lcore/utlity.cpp

LOCAL_C_INCLUDES := $(APP_C_INCLUDES)

include $(BUILD_STATIC_LIBRARY)

##################################
# lmath
include $(CLEAR_VARS)

LOCAL_MODULE    := lmath
LOCAL_SRC_FILES := $(LIME_DIR)/lmath/lmathcore.cpp\
                   $(LIME_DIR)/lmath/Vector2.cpp\
                   $(LIME_DIR)/lmath/Vector3.cpp\
                   $(LIME_DIR)/lmath/Vector4.cpp\
                   $(LIME_DIR)/lmath/Quaternion.cpp\
                   $(LIME_DIR)/lmath/Matrix34.cpp\
                   $(LIME_DIR)/lmath/Matrix44.cpp\
                   $(LIME_DIR)/lmath/DualQuaternion.cpp\
                   $(LIME_DIR)/lmath/utility_DualQuaternion.cpp\
                   $(LIME_DIR)/lmath/utility.cpp\
                   $(LIME_DIR)/lmath/geometry/PrimitiveTest.cpp

LOCAL_C_INCLUDES := $(APP_C_INCLUDES)
include $(BUILD_STATIC_LIBRARY)

##################################
# lgraphics
include $(CLEAR_VARS)

LOCAL_MODULE    := lgraphics
LOCAL_SRC_FILES := $(LIME_DIR)/lgraphics/lgraphics.cpp\
                   $(LIME_DIR)/lgraphics/api/EnumerationCodes.cpp\
                   $(LIME_DIR)/lgraphics/api/gles2/lgl2.cpp\
                   $(LIME_DIR)/lgraphics/api/gles2/GeometryBuffer.cpp\
                   $(LIME_DIR)/lgraphics/api/gles2/GraphicsDeviceRef.cpp\
                   $(LIME_DIR)/lgraphics/api/gles2/IndexBufferRef.cpp\
                   $(LIME_DIR)/lgraphics/api/gles2/RenderStateRef.cpp\
                   $(LIME_DIR)/lgraphics/api/gles2/SamplerState.cpp\
                   $(LIME_DIR)/lgraphics/api/gles2/TextureRef.cpp\
                   $(LIME_DIR)/lgraphics/api/gles2/ShaderRef.cpp\
                   $(LIME_DIR)/lgraphics/api/gles2/VertexBufferRef.cpp\
                   $(LIME_DIR)/lgraphics/api/gles2/VertexDeclarationRef.cpp\
                   $(LIME_DIR)/lgraphics/io/IOBMP.cpp\
                   $(LIME_DIR)/lgraphics/io/IODDS.cpp\
                   $(LIME_DIR)/lgraphics/io/ETC1.cpp\
                   $(LIME_DIR)/lgraphics/io/IOJPEG.cpp\
                   $(LIME_DIR)/lgraphics/io/IOTGA.cpp\
                   $(LIME_DIR)/lgraphics/io/IOPNG.cpp\
                   $(LIME_DIR)/lgraphics/io/IOTexture.cpp\
                   $(LIME_DIR)/lgraphics/io/IOTextureUtil.cpp

LOCAL_C_INCLUDES := $(APP_C_INCLUDES)

include $(BUILD_STATIC_LIBRARY)

##################################
# lframework
include $(CLEAR_VARS)

LOCAL_MODULE    := lframework
LOCAL_SRC_FILES := $(LIME_DIR)/lframework/IOUtil.cpp\
                   $(LIME_DIR)/lframework/Static.cpp\
                   $(LIME_DIR)/lframework/anim/AnimationClip.cpp\
                   $(LIME_DIR)/lframework/anim/AnimationControler.cpp\
                   $(LIME_DIR)/lframework/anim/AnimationControlerPartial.cpp\
                   $(LIME_DIR)/lframework/anim/AnimationSystem.cpp\
                   $(LIME_DIR)/lframework/anim/IKPack.cpp\
                   $(LIME_DIR)/lframework/anim/JointAnimation.cpp\
                   $(LIME_DIR)/lframework/anim/Skeleton.cpp\
                   $(LIME_DIR)/lframework/anim/SkeletonPose.cpp\
                   $(LIME_DIR)/lframework/scene/Camera.cpp\
                   $(LIME_DIR)/lframework/scene/StaticString.cpp

LOCAL_C_INCLUDES := $(APP_C_INCLUDES)

include $(BUILD_STATIC_LIBRARY)


##################################
# libpng
include $(CLEAR_VARS)

LOCAL_MODULE := png
LOCAL_SRC_FILES := ../../../ext/arm/lib/$(TARGET_ARCH_ABI)/libpng.a
include $(PREBUILT_STATIC_LIBRARY)

##################################
# libpng
include $(CLEAR_VARS)

LOCAL_MODULE := jpeg
LOCAL_SRC_FILES := ../../../ext/arm/lib/$(TARGET_ARCH_ABI)/libjpeg.a
include $(PREBUILT_STATIC_LIBRARY)

##################################
# libbullet
include $(CLEAR_VARS)

LOCAL_MODULE := bullet
LOCAL_SRC_FILES := ../../../ext/arm/lib/$(TARGET_ARCH_ABI)/libbullet.a
include $(PREBUILT_STATIC_LIBRARY)

##################################
# egda
include $(CLEAR_VARS)

LOCAL_MODULE := egda
LOCAL_USER_STATIC_LIBRARIES := liblframework liblgraphics liblmath liblcore
LOCAL_STATIC_LIBRARIES := $(LOCAL_USER_STATIC_LIBRARIES) libjpeg libpng libbullet
LOCAL_SRC_FILES:= $(VIEWER_DIR)/libConverter/converter.cpp\
                  $(VIEWER_DIR)/libConverter/Pmd.cpp\
                  $(VIEWER_DIR)/libConverter/Vmd.cpp\
                  $(VIEWER_DIR)/libConverter/SplitBone.cpp\
                  $(VIEWER_DIR)/libConverter/PmmDef.cpp\
                  $(VIEWER_DIR)/libConverter/Pmm.cpp\
                  $(VIEWER_DIR)/libConverter/XLoaderDef.cpp\
                  $(VIEWER_DIR)/libConverter/XLoader.cpp\
                  $(VIEWER_DIR)/libConverter/RigidBodyData.cpp\
                  $(VIEWER_DIR)/libConverter/charcode/conv_charcode.cpp\
                  $(VIEWER_DIR)/libConverter/charcode/jis_level1_utf16.cpp\
                  $(VIEWER_DIR)/libConverter/charcode/jis_level2_utf16.cpp\
                  $(LIBANIM_DIR)/AnimationControlerIK.cpp\
                  $(LIBDYNAMICS_DIR)/DynamicsWorld.cpp\
                  $(LIBDYNAMICS_DIR)/RigidBodySkeleton.cpp\
                  $(LIBDYNAMICS_DIR)/DynamicsDebugDraw.cpp\
                  $(VIEWER_DIR)/System.cpp\
                  $(VIEWER_DIR)/render/Batch.cpp\
                  $(VIEWER_DIR)/render/Drawable.cpp\
                  $(VIEWER_DIR)/render/PassMain.cpp\
                  $(VIEWER_DIR)/render/RenderingSystem.cpp\
                  $(VIEWER_DIR)/scene/shader/DefaultShader.cpp\
                  $(VIEWER_DIR)/scene/AnimObject.cpp\
                  $(VIEWER_DIR)/scene/Geometry.cpp\
                  $(VIEWER_DIR)/scene/IOHeader.cpp\
                  $(VIEWER_DIR)/scene/Material.cpp\
                  $(VIEWER_DIR)/scene/Object.cpp\
                  $(VIEWER_DIR)/scene/ShaderCreator.cpp\
                  $(VIEWER_DIR)/scene/ShaderManager.cpp\
                  ./stdafx.cpp\
                  ./main.cpp\
                  ./Application.cpp\
                  ./Scene.cpp\
                  ./Camera.cpp\
                  ./Light.cpp\
                  ./Input.cpp\
                  ./Config.cpp\
                  ./TextRenderer.cpp\

LOCAL_C_INCLUDES := $(APP_C_INCLUDES)
LOCAL_LDLIBS    := -lGLESv2 -ldl -llog -lz
include $(BUILD_SHARED_LIBRARY)
