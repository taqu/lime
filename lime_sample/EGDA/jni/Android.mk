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
                   $(LIME_DIR)/lmath/Matrix43.cpp\
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
                   $(LIME_DIR)/lgraphics/io/IOJPEG.cpp\
                   $(LIME_DIR)/lgraphics/io/IOTGA.cpp\
                   $(LIME_DIR)/lgraphics/io/IOPNG.cpp\
                   $(LIME_DIR)/lgraphics/io/IOGeometryBuffer.cpp\
                   $(LIME_DIR)/lgraphics/io/IOIndexBuffer.cpp\
                   $(LIME_DIR)/lgraphics/io/IORenderState.cpp\
                   $(LIME_DIR)/lgraphics/io/IOSamplerState.cpp\
                   $(LIME_DIR)/lgraphics/io/IOShaderKey.cpp\
                   $(LIME_DIR)/lgraphics/io/IOTexture.cpp\
                   $(LIME_DIR)/lgraphics/io/IOTextureUtil.cpp\
                   $(LIME_DIR)/lgraphics/io/IOVertexBuffer.cpp\
                   $(LIME_DIR)/lgraphics/io/IOVertexDeclaration.cpp

LOCAL_C_INCLUDES := $(APP_C_INCLUDES)

include $(BUILD_STATIC_LIBRARY)

##################################
# lframework
include $(CLEAR_VARS)

LOCAL_MODULE    := lframework
LOCAL_SRC_FILES := $(LIME_DIR)/lframework/IOUtil.cpp\
                   $(LIME_DIR)/lframework/Static.cpp\
                   $(LIME_DIR)/lframework/System.cpp\
                   $(LIME_DIR)/lframework/anim/AnimationClip.cpp\
                   $(LIME_DIR)/lframework/anim/AnimationControler.cpp\
                   $(LIME_DIR)/lframework/anim/AnimationControlerPartial.cpp\
                   $(LIME_DIR)/lframework/anim/AnimationSystem.cpp\
                   $(LIME_DIR)/lframework/anim/IKPack.cpp\
                   $(LIME_DIR)/lframework/anim/JointAnimation.cpp\
                   $(LIME_DIR)/lframework/anim/Skeleton.cpp\
                   $(LIME_DIR)/lframework/anim/SkeletonPose.cpp\
                   $(LIME_DIR)/lframework/render/Batch.cpp\
                   $(LIME_DIR)/lframework/render/Drawable.cpp\
                   $(LIME_DIR)/lframework/render/PassMain.cpp\
                   $(LIME_DIR)/lframework/render/RenderingSystem.cpp\
                   $(LIME_DIR)/lframework/scene/shader/DefaultShader.cpp\
                   $(LIME_DIR)/lframework/scene/AnimObject.cpp\
                   $(LIME_DIR)/lframework/scene/Geometry.cpp\
                   $(LIME_DIR)/lframework/scene/IOGeometry.cpp\
                   $(LIME_DIR)/lframework/scene/IOHeader.cpp\
                   $(LIME_DIR)/lframework/scene/IOMaterial.cpp\
                   $(LIME_DIR)/lframework/scene/IOObject.cpp\
                   $(LIME_DIR)/lframework/scene/Material.cpp\
                   $(LIME_DIR)/lframework/scene/Object.cpp\
                   $(LIME_DIR)/lframework/scene/ShaderCreator.cpp\
                   $(LIME_DIR)/lframework/scene/ShaderManager.cpp\
                   $(LIME_DIR)/lframework/scene/StaticString.cpp

LOCAL_C_INCLUDES := $(APP_C_INCLUDES)

include $(BUILD_STATIC_LIBRARY)

##################################
# libConverter
include $(CLEAR_VARS)

LOCAL_MODULE    := Converter
LOCAL_SRC_FILES := $(LIBCONVERTER_DIR)/converter.cpp\
                   $(LIBCONVERTER_DIR)/Pmd.cpp\
                   $(LIBCONVERTER_DIR)/Vmd.cpp\
                   $(LIBCONVERTER_DIR)/SplitBone.cpp\
                   $(LIBCONVERTER_DIR)/PmmDef.cpp\
                   $(LIBCONVERTER_DIR)/Pmm.cpp\
                   $(LIBCONVERTER_DIR)/XLoaderDef.cpp\
                   $(LIBCONVERTER_DIR)/XLoader.cpp\
                   $(LIBCONVERTER_DIR)/charcode/conv_charcode.cpp\
                   $(LIBCONVERTER_DIR)/charcode/jis_level1_utf16.cpp\
                   $(LIBCONVERTER_DIR)/charcode/jis_level2_utf16.cpp

LOCAL_C_INCLUDES := $(APP_C_INCLUDES) $(LIBCONVERTER_DIR)

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
# egda
include $(CLEAR_VARS)

LOCAL_MODULE := egda
LOCAL_USER_STATIC_LIBRARIES := libConverter liblframework liblgraphics liblmath liblcore
LOCAL_STATIC_LIBRARIES := $(LOCAL_USER_STATIC_LIBRARIES) libjpeg libpng
LOCAL_SRC_FILES:= $(LIBANIM_DIR)/AnimationControlerIK.cpp\
                  ./stdafx.cpp\
                  ./main.cpp\
                  ./Application.cpp\
                  ./Scene.cpp\
                  ./Camera.cpp\
                  ./Light.cpp\
                  ./Input.cpp\
                  ./Config.cpp\
                  ./TextRenderer.cpp\

LOCAL_C_INCLUDES := $(APP_C_INCLUDES) $(LIBCONVERTER_DIR)
LOCAL_LDLIBS    := -lGLESv2 -ldl -llog -lz
include $(BUILD_SHARED_LIBRARY)
