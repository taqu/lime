/**
@file lgl2.cpp
@author t-sakai
@date 2010/12/02 create
*/
#include <lcore/ldll.h>
#include <lcore/liostream.h>

#include "../../lgraphicscore.h"

#define LIME_GLES2_API
#define LIME_GLES2_FUNC_NULL = NULL
#define LIME_GLES2_NO_FUNCDEF

#define LIME_EGL_API
#define LIME_EGL_FUNC_NULL =NULL
#define LIME_EGL_NO_FUNCDEF

#if defined(LIME_FUNCDEF_GLES2)
#undef LIME_FUNCDEF_GLES2
#endif

#include "lgl2.h"
#include "lgl2ext.h"

#if defined(LIME_INCLUDE_EGL)
#include "legl.h"
#endif

#if defined(LIME_INIT_GLES2)
static lcore::HMODULE g_gles2DLL =NULL;
#endif

#ifdef __cplusplus
extern "C"
{
#endif


#define LIM_ES2_GET_FUNCPTR(name) \
    {*((void**)&LIME_ES2_FUNCPTR(name)) = LIME_EGL_FUNCPTR(eglGetProcAddress)(#name);\
    if(LIME_ES2_FUNCPTR(name) == NULL) return false;}

#define LIM_ES2_GET_FUNCPTR_NO_RETURN(name) \
    {*((void**)&LIME_ES2_FUNCPTR(name)) = LIME_EGL_FUNCPTR(eglGetProcAddress)(#name);}

#define LIME_ES2L_DEUMP_FUNCNAME(name) \
    {if(LIME_ES2_FUNCPTR(name) != NULL){ file.write(#name"\n", sizeof(#name"\n"));}}


#define LIM_EGL_GET_FUNCPTR(name) \
    {*((void**)&LIME_EGL_FUNCPTR(name)) = lcore::getProcAddressDLL(g_gles2DLL, #name);\
    if(LIME_EGL_FUNCPTR(name) == NULL) return false;}

#define LIM_EGL_GET_FUNCPTR_NO_RETURN(name) \
    {*((void**)&LIME_EGL_FUNCPTR(name)) = lcore::getProcAddressDLL(g_gles2DLL, #name);}

#define LIME_EGL_DEUMP_FUNCNAME(name) \
    {if(LIME_EGL_FUNCPTR(name) != NULL){ file.write(#name"\n", sizeof(#name"\n"));}}

    bool initializeEGL()
    {
#if defined(LIME_INIT_EGL)
        LASSERT(g_gles2DLL == NULL);
        g_gles2DLL = lcore::openDLL(LIME_GLES2_DLL_NAME);
        if(g_gles2DLL == NULL){
            return false;
        }

        /**************************************************************/
        LIM_EGL_GET_FUNCPTR(eglGetError);
        LIM_EGL_GET_FUNCPTR(eglGetDisplay);
        LIM_EGL_GET_FUNCPTR(eglInitialize);
        LIM_EGL_GET_FUNCPTR(eglTerminate);
        LIM_EGL_GET_FUNCPTR(eglQueryString);
        LIM_EGL_GET_FUNCPTR(eglDestroySurface);
        LIM_EGL_GET_FUNCPTR(eglBindAPI);
        LIM_EGL_GET_FUNCPTR(eglQueryAPI);
        LIM_EGL_GET_FUNCPTR(eglWaitClient);
        LIM_EGL_GET_FUNCPTR(eglReleaseThread);
        LIM_EGL_GET_FUNCPTR(eglBindTexImage);
        LIM_EGL_GET_FUNCPTR(eglReleaseTexImage);
        LIM_EGL_GET_FUNCPTR(eglSwapInterval);
        LIM_EGL_GET_FUNCPTR(eglDestroyContext);
        LIM_EGL_GET_FUNCPTR(eglGetCurrentContext);
        LIM_EGL_GET_FUNCPTR(eglGetCurrentSurface);
        LIM_EGL_GET_FUNCPTR(eglGetCurrentDisplay);
        LIM_EGL_GET_FUNCPTR(eglWaitGL);
        LIM_EGL_GET_FUNCPTR(eglWaitNative);
        LIM_EGL_GET_FUNCPTR(eglSwapBuffers);

        // egl1.4ヘッダには含まれない
        LIM_EGL_GET_FUNCPTR(eglCreateWindowSurface);
        LIM_EGL_GET_FUNCPTR(eglCreateContext);
        LIM_EGL_GET_FUNCPTR(eglMakeCurrent);
        LIM_EGL_GET_FUNCPTR(eglChooseConfig);
        LIM_EGL_GET_FUNCPTR(eglGetProcAddress);


        /**************************************************************/
        LIM_EGL_GET_FUNCPTR_NO_RETURN(eglLockSurfaceKHR);
        LIM_EGL_GET_FUNCPTR_NO_RETURN(eglUnlockSurfaceKHR);

        LIM_EGL_GET_FUNCPTR_NO_RETURN(eglCreateImageKHR);
        LIM_EGL_GET_FUNCPTR_NO_RETURN(eglDestroyImageKHR);

        LIM_EGL_GET_FUNCPTR_NO_RETURN(eglCreateSyncKHR);
        LIM_EGL_GET_FUNCPTR_NO_RETURN(eglDestroySyncKHR);
        LIM_EGL_GET_FUNCPTR_NO_RETURN(eglClientWaitSyncKHR);
        LIM_EGL_GET_FUNCPTR_NO_RETURN(eglSignalSyncKHR);
        LIM_EGL_GET_FUNCPTR_NO_RETURN(eglGetSyncAttribKHR);

        LIM_EGL_GET_FUNCPTR_NO_RETURN(eglCreatePixmapSurfaceHI);

        LIM_EGL_GET_FUNCPTR_NO_RETURN(eglCreateDRMImageMESA);
        LIM_EGL_GET_FUNCPTR_NO_RETURN(eglExportDRMImageMESA);

#endif //LIME_INIT_EGL
        return true;
    }

    void terminateEGL()
    {
#if defined(LIME_INIT_EGL)
        lcore::closeDLL(g_gles2DLL);
        g_gles2DLL = NULL;
#endif
    }

    void dumpEGLExtensions(const char* filepath)
    {
        LASSERT(filepath != NULL);

#if defined(LIME_INIT_EGL)
        LASSERT(g_gles2DLL != NULL);

        lcore::ofstream file(filepath, lcore::ios::binary);
        if(file.is_open() == false){
            return;
        }

        /**************************************************************/
        LIME_EGL_DEUMP_FUNCNAME(eglLockSurfaceKHR);
        LIME_EGL_DEUMP_FUNCNAME(eglUnlockSurfaceKHR);

        LIME_EGL_DEUMP_FUNCNAME(eglCreateImageKHR);
        LIME_EGL_DEUMP_FUNCNAME(eglDestroyImageKHR);

        LIME_EGL_DEUMP_FUNCNAME(eglCreateSyncKHR);
        LIME_EGL_DEUMP_FUNCNAME(eglDestroySyncKHR);
        LIME_EGL_DEUMP_FUNCNAME(eglClientWaitSyncKHR);
        LIME_EGL_DEUMP_FUNCNAME(eglSignalSyncKHR);
        LIME_EGL_DEUMP_FUNCNAME(eglGetSyncAttribKHR);

        LIME_EGL_DEUMP_FUNCNAME(eglCreatePixmapSurfaceHI);

        LIME_EGL_DEUMP_FUNCNAME(eglCreateDRMImageMESA);
        LIME_EGL_DEUMP_FUNCNAME(eglExportDRMImageMESA);

        file.close();
#endif //LIME_INIT_EGL
    }

    bool initializeGLES2()
    {
#if defined(LIME_INIT_GLES2)
        if(g_gles2DLL == NULL){
            g_gles2DLL = lcore::openDLL(LIME_GLES2_DLL_NAME);
            if(g_gles2DLL == NULL){
                return false;
            }
        }

        /**************************************************************/
        LIM_ES2_GET_FUNCPTR(glActiveTexture);
        LIM_ES2_GET_FUNCPTR(glAttachShader);
        LIM_ES2_GET_FUNCPTR(glBindAttribLocation);
        LIM_ES2_GET_FUNCPTR(glBindBuffer);
        LIM_ES2_GET_FUNCPTR(glBindFramebuffer);
        LIM_ES2_GET_FUNCPTR(glBindRenderbuffer);
        LIM_ES2_GET_FUNCPTR(glBindTexture);
        LIM_ES2_GET_FUNCPTR(glBlendColor);
        LIM_ES2_GET_FUNCPTR(glBlendEquation);
        LIM_ES2_GET_FUNCPTR(glBlendEquationSeparate);
        LIM_ES2_GET_FUNCPTR(glBlendFunc);
        LIM_ES2_GET_FUNCPTR(glBlendFuncSeparate);
        LIM_ES2_GET_FUNCPTR(glBufferData);
        LIM_ES2_GET_FUNCPTR(glBufferSubData);
        LIM_ES2_GET_FUNCPTR(glCheckFramebufferStatus);
        LIM_ES2_GET_FUNCPTR(glClear);
        LIM_ES2_GET_FUNCPTR(glClearColor);
        LIM_ES2_GET_FUNCPTR(glClearDepthf);
        LIM_ES2_GET_FUNCPTR(glClearStencil);
        LIM_ES2_GET_FUNCPTR(glColorMask);
        LIM_ES2_GET_FUNCPTR(glCompileShader);
        LIM_ES2_GET_FUNCPTR(glCompressedTexImage2D);
        LIM_ES2_GET_FUNCPTR(glCompressedTexSubImage2D);
        LIM_ES2_GET_FUNCPTR(glCopyTexImage2D);
        LIM_ES2_GET_FUNCPTR(glCopyTexSubImage2D);
        LIM_ES2_GET_FUNCPTR(glCreateProgram);
        LIM_ES2_GET_FUNCPTR(glCreateShader);
        LIM_ES2_GET_FUNCPTR(glCullFace);
        LIM_ES2_GET_FUNCPTR(glDeleteBuffers);
        LIM_ES2_GET_FUNCPTR(glDeleteFramebuffers);
        LIM_ES2_GET_FUNCPTR(glDeleteProgram);
        LIM_ES2_GET_FUNCPTR(glDeleteRenderbuffers);
        LIM_ES2_GET_FUNCPTR(glDeleteShader);
        LIM_ES2_GET_FUNCPTR(glDeleteTextures);
        LIM_ES2_GET_FUNCPTR(glDepthFunc);
        LIM_ES2_GET_FUNCPTR(glDepthMask);
        LIM_ES2_GET_FUNCPTR(glDepthRangef);
        LIM_ES2_GET_FUNCPTR(glDetachShader);
        LIM_ES2_GET_FUNCPTR(glDisable);
        LIM_ES2_GET_FUNCPTR(glDisableVertexAttribArray);
        LIM_ES2_GET_FUNCPTR(glDrawArrays);
        LIM_ES2_GET_FUNCPTR(glDrawElements);
        LIM_ES2_GET_FUNCPTR(glEnable);
        LIM_ES2_GET_FUNCPTR(glEnableVertexAttribArray);
        LIM_ES2_GET_FUNCPTR(glFinish);
        LIM_ES2_GET_FUNCPTR(glFlush);
        LIM_ES2_GET_FUNCPTR(glFramebufferRenderbuffer);
        LIM_ES2_GET_FUNCPTR(glFramebufferTexture2D);
        LIM_ES2_GET_FUNCPTR(glFrontFace);
        LIM_ES2_GET_FUNCPTR(glGenBuffers);
        LIM_ES2_GET_FUNCPTR(glGenerateMipmap);
        LIM_ES2_GET_FUNCPTR(glGenFramebuffers);
        LIM_ES2_GET_FUNCPTR(glGenRenderbuffers);
        LIM_ES2_GET_FUNCPTR(glGenTextures);
        LIM_ES2_GET_FUNCPTR(glGetActiveAttrib);
        LIM_ES2_GET_FUNCPTR(glGetActiveUniform);
        LIM_ES2_GET_FUNCPTR(glGetAttachedShaders);
        LIM_ES2_GET_FUNCPTR(glGetAttribLocation);
        LIM_ES2_GET_FUNCPTR(glGetBooleanv);
        LIM_ES2_GET_FUNCPTR(glGetBufferParameteriv);
        LIM_ES2_GET_FUNCPTR(glGetError);
        LIM_ES2_GET_FUNCPTR(glGetFloatv);
        LIM_ES2_GET_FUNCPTR(glGetFramebufferAttachmentParameteriv);
        LIM_ES2_GET_FUNCPTR(glGetIntegerv);
        LIM_ES2_GET_FUNCPTR(glGetProgramiv);
        LIM_ES2_GET_FUNCPTR(glGetProgramInfoLog);
        LIM_ES2_GET_FUNCPTR(glGetRenderbufferParameteriv);
        LIM_ES2_GET_FUNCPTR(glGetShaderiv);
        LIM_ES2_GET_FUNCPTR(glGetShaderInfoLog);
        LIM_ES2_GET_FUNCPTR(glGetShaderPrecisionFormat);
        LIM_ES2_GET_FUNCPTR(glGetShaderSource);
        LIM_ES2_GET_FUNCPTR(glGetString);
        LIM_ES2_GET_FUNCPTR(glGetTexParameterfv);
        LIM_ES2_GET_FUNCPTR(glGetTexParameteriv);
        LIM_ES2_GET_FUNCPTR(glGetUniformfv);
        LIM_ES2_GET_FUNCPTR(glGetUniformiv);
        LIM_ES2_GET_FUNCPTR(glGetUniformLocation);
        LIM_ES2_GET_FUNCPTR(glGetVertexAttribfv);
        LIM_ES2_GET_FUNCPTR(glGetVertexAttribiv);
        LIM_ES2_GET_FUNCPTR(glGetVertexAttribPointerv);
        LIM_ES2_GET_FUNCPTR(glHint);
        LIM_ES2_GET_FUNCPTR(glIsBuffer);
        LIM_ES2_GET_FUNCPTR(glIsEnabled);
        LIM_ES2_GET_FUNCPTR(glIsFramebuffer);
        LIM_ES2_GET_FUNCPTR(glIsProgram);
        LIM_ES2_GET_FUNCPTR(glIsRenderbuffer);
        LIM_ES2_GET_FUNCPTR(glIsShader);
        LIM_ES2_GET_FUNCPTR(glIsTexture);
        LIM_ES2_GET_FUNCPTR(glLineWidth);
        LIM_ES2_GET_FUNCPTR(glLinkProgram);
        LIM_ES2_GET_FUNCPTR(glPixelStorei);
        LIM_ES2_GET_FUNCPTR(glPolygonOffset);
        LIM_ES2_GET_FUNCPTR(glReadPixels);
        LIM_ES2_GET_FUNCPTR(glReleaseShaderCompiler);
        LIM_ES2_GET_FUNCPTR(glRenderbufferStorage);
        LIM_ES2_GET_FUNCPTR(glSampleCoverage);
        LIM_ES2_GET_FUNCPTR(glScissor);
        LIM_ES2_GET_FUNCPTR(glShaderBinary);
        LIM_ES2_GET_FUNCPTR(glShaderSource);
        LIM_ES2_GET_FUNCPTR(glStencilFunc);
        LIM_ES2_GET_FUNCPTR(glStencilFuncSeparate);
        LIM_ES2_GET_FUNCPTR(glStencilMask);
        LIM_ES2_GET_FUNCPTR(glStencilMaskSeparate);
        LIM_ES2_GET_FUNCPTR(glStencilOp);
        LIM_ES2_GET_FUNCPTR(glStencilOpSeparate);
        LIM_ES2_GET_FUNCPTR(glTexImage2D);
        LIM_ES2_GET_FUNCPTR(glTexParameterf);
        LIM_ES2_GET_FUNCPTR(glTexParameterfv);
        LIM_ES2_GET_FUNCPTR(glTexParameteri);
        LIM_ES2_GET_FUNCPTR(glTexParameteriv);
        LIM_ES2_GET_FUNCPTR(glTexSubImage2D);
        LIM_ES2_GET_FUNCPTR(glUniform1f);
        LIM_ES2_GET_FUNCPTR(glUniform1fv);
        LIM_ES2_GET_FUNCPTR(glUniform1i);
        LIM_ES2_GET_FUNCPTR(glUniform1iv);
        LIM_ES2_GET_FUNCPTR(glUniform2f);
        LIM_ES2_GET_FUNCPTR(glUniform2fv);
        LIM_ES2_GET_FUNCPTR(glUniform2i);
        LIM_ES2_GET_FUNCPTR(glUniform2iv);
        LIM_ES2_GET_FUNCPTR(glUniform3f);
        LIM_ES2_GET_FUNCPTR(glUniform3fv);
        LIM_ES2_GET_FUNCPTR(glUniform3i);
        LIM_ES2_GET_FUNCPTR(glUniform3iv);
        LIM_ES2_GET_FUNCPTR(glUniform4f);
        LIM_ES2_GET_FUNCPTR(glUniform4fv);
        LIM_ES2_GET_FUNCPTR(glUniform4i);
        LIM_ES2_GET_FUNCPTR(glUniform4iv);
        LIM_ES2_GET_FUNCPTR(glUniformMatrix2fv);
        LIM_ES2_GET_FUNCPTR(glUniformMatrix3fv);
        LIM_ES2_GET_FUNCPTR(glUniformMatrix4fv);
        LIM_ES2_GET_FUNCPTR(glUseProgram);
        LIM_ES2_GET_FUNCPTR(glValidateProgram);
        LIM_ES2_GET_FUNCPTR(glVertexAttrib1f);
        LIM_ES2_GET_FUNCPTR(glVertexAttrib1fv);
        LIM_ES2_GET_FUNCPTR(glVertexAttrib2f);
        LIM_ES2_GET_FUNCPTR(glVertexAttrib2fv);
        LIM_ES2_GET_FUNCPTR(glVertexAttrib3f);
        LIM_ES2_GET_FUNCPTR(glVertexAttrib3fv);
        LIM_ES2_GET_FUNCPTR(glVertexAttrib4f);
        LIM_ES2_GET_FUNCPTR(glVertexAttrib4fv);
        LIM_ES2_GET_FUNCPTR(glVertexAttribPointer);
        LIM_ES2_GET_FUNCPTR(glViewport);


        /**************************************************************/
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glEGLImageTargetTexture2DOES);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glEGLImageTargetRenderbufferStorageOES);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glGetProgramBinaryOES);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glProgramBinaryOES);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glMapBufferOES);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glUnmapBufferOES);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glGetBufferPointervOES);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glTexImage3DOES);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glTexSubImage3DOES);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glCopyTexSubImage3DOES);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glCompressedTexImage3DOES);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glCompressedTexSubImage3DOES);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glFramebufferTexture3DOES);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glBindVertexArrayOES);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glDeleteVertexArraysOES);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glGenVertexArraysOES);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glIsVertexArrayOES);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glGetPerfMonitorGroupsAMD);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glGetPerfMonitorCountersAMD);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glGetPerfMonitorGroupStringAMD);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glGetPerfMonitorCounterStringAMD);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glGetPerfMonitorCounterInfoAMD);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glGenPerfMonitorsAMD);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glDeletePerfMonitorsAMD);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glSelectPerfMonitorCountersAMD);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glBeginPerfMonitorAMD);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glEndPerfMonitorAMD);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glGetPerfMonitorCounterDataAMD);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glBlitFramebufferANGLE);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glRenderbufferStorageMultisampleANGLE);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glRenderbufferStorageMultisampleAPPLE);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glResolveMultisampleFramebufferAPPLE);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glDiscardFramebufferEXT);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glMultiDrawArraysEXT);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glMultiDrawElementsEXT);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glRenderbufferStorageMultisampleIMG);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glFramebufferTexture2DMultisampleIMG);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glDeleteFencesNV);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glGenFencesNV);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glIsFenceNV);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glTestFenceNV);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glGetFenceivNV);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glFinishFenceNV);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glSetFenceNV);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glCoverageMaskNV);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glCoverageOperationNV);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glGetDriverControlsQCOM);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glGetDriverControlStringQCOM);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glEnableDriverControlQCOM);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glDisableDriverControlQCOM);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glExtGetTexturesQCOM);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glExtGetBuffersQCOM);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glExtGetRenderbuffersQCOM);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glExtGetFramebuffersQCOM);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glExtGetTexLevelParameterivQCOM);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glExtTexObjectStateOverrideiQCOM);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glExtGetTexSubImageQCOM);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glExtGetBufferPointervQCOM);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glExtGetShadersQCOM);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glExtGetProgramsQCOM);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glExtIsProgramBinaryQCOM);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glExtGetProgramBinarySourceQCOM);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glStartTilingQCOM);
        LIM_ES2_GET_FUNCPTR_NO_RETURN(glEndTilingQCOM);
#endif //LIME_INIT_GLES2
        return true;
    }

    void terminateGLES2()
    {
    }
#if defined(LIME_INIT_GLES2)
    void dumpGLES2Extensions(const char* filepath)
    {
        LASSERT(filepath != NULL);
        LASSERT(g_gles2DLL != NULL);

        lcore::ofstream file(filepath, lcore::ios::binary);
        if(file.is_open() == false){
            return;
        }

        /**************************************************************/
        LIME_ES2L_DEUMP_FUNCNAME(glEGLImageTargetTexture2DOES);
        LIME_ES2L_DEUMP_FUNCNAME(glEGLImageTargetRenderbufferStorageOES);
        LIME_ES2L_DEUMP_FUNCNAME(glGetProgramBinaryOES);
        LIME_ES2L_DEUMP_FUNCNAME(glProgramBinaryOES);
        LIME_ES2L_DEUMP_FUNCNAME(glMapBufferOES);
        LIME_ES2L_DEUMP_FUNCNAME(glUnmapBufferOES);
        LIME_ES2L_DEUMP_FUNCNAME(glGetBufferPointervOES);
        LIME_ES2L_DEUMP_FUNCNAME(glTexImage3DOES);
        LIME_ES2L_DEUMP_FUNCNAME(glTexSubImage3DOES);
        LIME_ES2L_DEUMP_FUNCNAME(glCopyTexSubImage3DOES);
        LIME_ES2L_DEUMP_FUNCNAME(glCompressedTexImage3DOES);
        LIME_ES2L_DEUMP_FUNCNAME(glCompressedTexSubImage3DOES);
        LIME_ES2L_DEUMP_FUNCNAME(glFramebufferTexture3DOES);
        LIME_ES2L_DEUMP_FUNCNAME(glBindVertexArrayOES);
        LIME_ES2L_DEUMP_FUNCNAME(glDeleteVertexArraysOES);
        LIME_ES2L_DEUMP_FUNCNAME(glGenVertexArraysOES);
        LIME_ES2L_DEUMP_FUNCNAME(glIsVertexArrayOES);
        LIME_ES2L_DEUMP_FUNCNAME(glGetPerfMonitorGroupsAMD);
        LIME_ES2L_DEUMP_FUNCNAME(glGetPerfMonitorCountersAMD);
        LIME_ES2L_DEUMP_FUNCNAME(glGetPerfMonitorGroupStringAMD);
        LIME_ES2L_DEUMP_FUNCNAME(glGetPerfMonitorCounterStringAMD);
        LIME_ES2L_DEUMP_FUNCNAME(glGetPerfMonitorCounterInfoAMD);
        LIME_ES2L_DEUMP_FUNCNAME(glGenPerfMonitorsAMD);
        LIME_ES2L_DEUMP_FUNCNAME(glDeletePerfMonitorsAMD);
        LIME_ES2L_DEUMP_FUNCNAME(glSelectPerfMonitorCountersAMD);
        LIME_ES2L_DEUMP_FUNCNAME(glBeginPerfMonitorAMD);
        LIME_ES2L_DEUMP_FUNCNAME(glEndPerfMonitorAMD);
        LIME_ES2L_DEUMP_FUNCNAME(glGetPerfMonitorCounterDataAMD);
        LIME_ES2L_DEUMP_FUNCNAME(glBlitFramebufferANGLE);
        LIME_ES2L_DEUMP_FUNCNAME(glRenderbufferStorageMultisampleANGLE);
        LIME_ES2L_DEUMP_FUNCNAME(glRenderbufferStorageMultisampleAPPLE);
        LIME_ES2L_DEUMP_FUNCNAME(glResolveMultisampleFramebufferAPPLE);
        LIME_ES2L_DEUMP_FUNCNAME(glDiscardFramebufferEXT);
        LIME_ES2L_DEUMP_FUNCNAME(glMultiDrawArraysEXT);
        LIME_ES2L_DEUMP_FUNCNAME(glMultiDrawElementsEXT);
        LIME_ES2L_DEUMP_FUNCNAME(glRenderbufferStorageMultisampleIMG);
        LIME_ES2L_DEUMP_FUNCNAME(glFramebufferTexture2DMultisampleIMG);
        LIME_ES2L_DEUMP_FUNCNAME(glDeleteFencesNV);
        LIME_ES2L_DEUMP_FUNCNAME(glGenFencesNV);
        LIME_ES2L_DEUMP_FUNCNAME(glIsFenceNV);
        LIME_ES2L_DEUMP_FUNCNAME(glTestFenceNV);
        LIME_ES2L_DEUMP_FUNCNAME(glGetFenceivNV);
        LIME_ES2L_DEUMP_FUNCNAME(glFinishFenceNV);
        LIME_ES2L_DEUMP_FUNCNAME(glSetFenceNV);
        LIME_ES2L_DEUMP_FUNCNAME(glCoverageMaskNV);
        LIME_ES2L_DEUMP_FUNCNAME(glCoverageOperationNV);
        LIME_ES2L_DEUMP_FUNCNAME(glGetDriverControlsQCOM);
        LIME_ES2L_DEUMP_FUNCNAME(glGetDriverControlStringQCOM);
        LIME_ES2L_DEUMP_FUNCNAME(glEnableDriverControlQCOM);
        LIME_ES2L_DEUMP_FUNCNAME(glDisableDriverControlQCOM);
        LIME_ES2L_DEUMP_FUNCNAME(glExtGetTexturesQCOM);
        LIME_ES2L_DEUMP_FUNCNAME(glExtGetBuffersQCOM);
        LIME_ES2L_DEUMP_FUNCNAME(glExtGetRenderbuffersQCOM);
        LIME_ES2L_DEUMP_FUNCNAME(glExtGetFramebuffersQCOM);
        LIME_ES2L_DEUMP_FUNCNAME(glExtGetTexLevelParameterivQCOM);
        LIME_ES2L_DEUMP_FUNCNAME(glExtTexObjectStateOverrideiQCOM);
        LIME_ES2L_DEUMP_FUNCNAME(glExtGetTexSubImageQCOM);
        LIME_ES2L_DEUMP_FUNCNAME(glExtGetBufferPointervQCOM);
        LIME_ES2L_DEUMP_FUNCNAME(glExtGetShadersQCOM);
        LIME_ES2L_DEUMP_FUNCNAME(glExtGetProgramsQCOM);
        LIME_ES2L_DEUMP_FUNCNAME(glExtIsProgramBinaryQCOM);
        LIME_ES2L_DEUMP_FUNCNAME(glExtGetProgramBinarySourceQCOM);
        LIME_ES2L_DEUMP_FUNCNAME(glStartTilingQCOM);
        LIME_ES2L_DEUMP_FUNCNAME(glEndTilingQCOM);

        file.close();
    }
#else
    void dumpGLES2Extensions(const char*)
    {
    }
#endif //LIME_INIT_GLES2

#ifdef __cplusplus
}
#endif

