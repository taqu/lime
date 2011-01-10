#include "stdafx.h"
#include "Application.h"

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jboolean JNICALL Java_hm_orz_stabo_EGDA_EGDALib_initialize(JNIEnv *, jclass);
JNIEXPORT void JNICALL Java_hm_orz_stabo_EGDA_EGDALib_terminate(JNIEnv *, jclass);
JNIEXPORT void JNICALL Java_hm_orz_stabo_EGDA_EGDALib_setViewport(JNIEnv *, jclass, jint width, jint height);
JNIEXPORT void JNICALL Java_hm_orz_stabo_EGDA_EGDALib_update(JNIEnv *, jclass);
JNIEXPORT jboolean JNICALL Java_hm_orz_stabo_EGDA_EGDALib_load(JNIEnv *, jclass, jbyteArray, jbyteArray, jint);

jboolean JNICALL Java_hm_orz_stabo_EGDA_EGDALib_initialize(JNIEnv *, jclass)
{
    bool ret = egda::Application::getInstance().initialize();
    if(ret){
        lcore::Log("initialize");
    }
    return (ret)? JNI_TRUE : JNI_FALSE;
}

void JNICALL Java_hm_orz_stabo_EGDA_EGDALib_terminate(JNIEnv *, jclass)
{
    egda::Application::getInstance().terminate();
    lcore::Log("terminate");
}

void JNICALL Java_hm_orz_stabo_EGDA_EGDALib_setViewport(JNIEnv *, jclass, jint width, jint height)
{
    egda::Application::getInstance().setViewport(0, 0, width, height);
}

void JNICALL Java_hm_orz_stabo_EGDA_EGDALib_update(JNIEnv *, jclass)
{
    egda::Application::getInstance().update();
}

jboolean JNICALL Java_hm_orz_stabo_EGDA_EGDALib_load(JNIEnv *env, jclass, jbyteArray bPath, jbyteArray bDir, jint resourceType)
{
    lcore::Log("load called");
    if(resourceType<0 || egda::ResType_Num<=resourceType){
        lcore::Log("load invalid call");
        return JNI_FALSE;
    }

    egda::JNIByteArray path(env, bPath);
    egda::JNIByteArray directory(env, bDir);

    if(path.getData() == NULL || directory.getData() == NULL){
        lcore::Log("load internal error");
        return JNI_FALSE;
    }

    bool ret = false;
    egda::Application& app = egda::Application::getInstance();

    switch(resourceType)
    {
    case egda::ResType_Model:
        ret = app.openModel( path.getData(), directory.getData() );
        break;

    case egda::ResType_Anim:
        ret = app.openAnim(path.getData());
        break;

    default:
        break;
    }

    if(false == ret){
        lcore::Log("load fail to open %s", path.getData());
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

#ifdef __cplusplus
}
#endif
