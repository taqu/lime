#include "stdafx.h"
#include "Application.h"
#include "Input.h"
#include "Config.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
    @brief �V�X�e��������
    */
    JNIEXPORT jboolean JNICALL Java_hm_orz_stabo_EGDA_EGDALib_initialize(JNIEnv *, jclass, jbyteArray);

    /**
    @brief �V�X�e���I��
    */
    JNIEXPORT void JNICALL Java_hm_orz_stabo_EGDA_EGDALib_terminate(JNIEnv *, jclass);

    /**
    @brief �r���[�|�[�g�Z�b�g
    */
    JNIEXPORT void JNICALL Java_hm_orz_stabo_EGDA_EGDALib_setViewport(JNIEnv *, jclass, jint width, jint height);

    /**
    @brief �X�V
    */
    JNIEXPORT void JNICALL Java_hm_orz_stabo_EGDA_EGDALib_update(JNIEnv *, jclass);

    /**
    @brief �X���X�V
    */
    JNIEXPORT void JNICALL Java_hm_orz_stabo_EGDA_EGDALib_updateOrientation(JNIEnv *, jclass, jfloat, jfloat, jfloat);

    /**
    @brief �^�b�`�C�x���g�X�V
    */
    JNIEXPORT void JNICALL Java_hm_orz_stabo_EGDA_EGDALib_updateTouch(JNIEnv *, jclass, jboolean, jfloat, jfloat);

    /**
    @brief ���\�[�X���[�h
    */
    JNIEXPORT jboolean JNICALL Java_hm_orz_stabo_EGDA_EGDALib_load(JNIEnv *, jclass, jbyteArray, jbyteArray, jint);

    /**
    @brief ��ԃZ�b�g
    */
    JNIEXPORT void JNICALL Java_hm_orz_stabo_EGDA_EGDALib_setState(JNIEnv *, jclass, jint);

    /**
    @brief ���[�h�Z�b�g
    */
    JNIEXPORT void JNICALL Java_hm_orz_stabo_EGDA_EGDALib_setMode(JNIEnv *, jclass, jint, jint);

    //--------------------------------------------------------------------------
    // �V�X�e��������
    jboolean JNICALL Java_hm_orz_stabo_EGDA_EGDALib_initialize(JNIEnv *env, jclass, jbyteArray btex)
    {
        egda::JNIByteArray tex(env, btex);

        bool ret = egda::Application::getInstance().initialize(reinterpret_cast<const lcore::Char*>(tex.getData()), tex.size());
        if(!ret){
            lcore::Log("cannot initialize");
        }

        return (ret)? JNI_TRUE : JNI_FALSE;
    }

    //--------------------------------------------------------------------------
    // �V�X�e���I��
    void JNICALL Java_hm_orz_stabo_EGDA_EGDALib_terminate(JNIEnv *, jclass)
    {
        egda::Application::getInstance().terminate();
        lcore::Log("terminate");
    }

    //--------------------------------------------------------------------------
    // �r���[�|�[�g�Z�b�g
    void JNICALL Java_hm_orz_stabo_EGDA_EGDALib_setViewport(JNIEnv *, jclass, jint width, jint height)
    {
        egda::Application::getInstance().setViewport(0, 0, width, height);
        egda::Application::getInstance().resetProjection();
    }

    //--------------------------------------------------------------------------
    // �X�V
    void JNICALL Java_hm_orz_stabo_EGDA_EGDALib_update(JNIEnv *, jclass)
    {
        egda::Application::getInstance().update();
    }

    //--------------------------------------------------------------------------
    // �X���X�V
    JNIEXPORT void JNICALL Java_hm_orz_stabo_EGDA_EGDALib_updateOrientation(JNIEnv *, jclass, jfloat x, jfloat y, jfloat z)
    {
        egda::Input::updateOrientation(x, y, z);
    }

    //--------------------------------------------------------------------------
    //  �^�b�`�C�x���g�X�V
    JNIEXPORT void JNICALL Java_hm_orz_stabo_EGDA_EGDALib_updateTouch(JNIEnv *, jclass, jboolean isOn, jfloat x, jfloat y)
    {
        egda::Input::updateTouch(isOn, x, y);
    }

    //--------------------------------------------------------------------------
    // ���\�[�X���[�h
    jboolean JNICALL Java_hm_orz_stabo_EGDA_EGDALib_load(JNIEnv *env, jclass, jbyteArray bPath, jbyteArray bDir, jint resourceType)
    {
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
        case egda::ResType_Pmm:
            ret = app.loadPmm( path.getData(), directory.getData() );
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


    //--------------------------------------------------------------------------
    // ��ԃZ�b�g
    JNIEXPORT void JNICALL Java_hm_orz_stabo_EGDA_EGDALib_setState(JNIEnv *, jclass, jint state)
    {
        lcore::Log("set state: %d", state);
        egda::Application::getInstance().setState(state);
    }

    //--------------------------------------------------------------------------
    // ���[�h�Z�b�g
    JNIEXPORT void JNICALL Java_hm_orz_stabo_EGDA_EGDALib_setMode(JNIEnv *, jclass, jint screenMode, jint cameraMode)
    {
        lcore::Log("set mode: screen %d, camera %d", screenMode, cameraMode);

        egda::Config::getInstance().setScreenMode((egda::ScreenMode)screenMode);
        egda::Application::getInstance().setCameraMode(cameraMode);
        egda::Application::getInstance().resetProjection();
    }


#ifdef __cplusplus
}
#endif
