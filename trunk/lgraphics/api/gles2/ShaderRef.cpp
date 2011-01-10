/**
@file ShaderRef.cpp
@author t-sakai
@date 2009/01/26 create
*/
#include <lcore/liostream.h>
#include <lmath/Vector2.h>
#include <lmath/Vector4.h>
#include <lmath/Matrix44.h>

#include "../../lgraphicsAPIInclude.h"
#include "../../lgraphics.h"
#include "GraphicsDeviceRef.h"
#include "VertexDeclarationRef.h"

#include "ShaderRef.h"
//#include "../ShaderKey.h"

namespace lgraphics
{

    //------------------------------------------------------------
    //---
    //--- Shader
    //---
    //------------------------------------------------------------
    Descriptor* Shader::createPixelShaderFromFile(const char* filename, s32 numDefines, const char** defines)
    {
        LASSERT(filename != NULL);
        lcore::ifstream file(filename, lcore::ios::binary|lcore::ios::in);

        if(file.is_open() == false){
            return NULL;
        }

        u32 size = 0;
        file.seekg(0, lcore::ios::end);
        size = file.tellg();
        file.seekg(0, lcore::ios::beg);

        char* str = LIME_NEW char[size+1];
        file.read(str, size);
        str[size] = '\0';
        file.close();

        Descriptor* desc = createPixelShaderFromMemory(str, size+1, numDefines, defines);

        LIME_DELETE_ARRAY(str);

        return desc;
    }

    Descriptor* Shader::createPixelShaderFromMemory(const char* memory, u32 size, s32 numDefines, const char** defines)
    {
        LASSERT(memory != NULL);

        u32 shader = glCreateShader(GL_FRAGMENT_SHADER);

        bool ret = compileShaderFromMemory(shader, memory, size, numDefines, defines);

        if(!ret){
            glDeleteShader(shader);
            return NULL;
        }

        Descriptor *desc = lgraphics::Graphics::getDevice().allocate();
        LASSERT(desc != NULL);

        desc->addRef();
        desc->id_ = shader;

        return desc;
    }


    Descriptor* Shader::createVertexShaderFromFile(const char* filename, s32 numDefines, const char** defines)
    {
        LASSERT(filename != NULL);
        lcore::ifstream file(filename, lcore::ios::binary|lcore::ios::in);

        if(file.is_open() == false){
            return NULL;
        }

        u32 size = 0;
        file.seekg(0, lcore::ios::end);
        size = file.tellg();
        file.seekg(0, lcore::ios::beg);

        char* str = LIME_NEW char[size+1];
        file.read(str, size);
        str[size] = '\0';
        file.close();

        Descriptor *desc = createVertexShaderFromMemory(str, size+1, numDefines, defines);

        LIME_DELETE_ARRAY(str);

        return desc;
    }

    Descriptor* Shader::createVertexShaderFromMemory(const char* memory, u32 size, s32 numDefines, const char** defines)
    {
        LASSERT(memory != NULL);

        u32 shader = glCreateShader(GL_VERTEX_SHADER);

        bool ret = compileShaderFromMemory(shader, memory, size, numDefines, defines);

        if(!ret){
            glDeleteShader(shader);
            return NULL;
        }

        Descriptor *desc = lgraphics::Graphics::getDevice().allocate();
        LASSERT(desc != NULL);

        desc->addRef();
        desc->id_ = shader;

        return desc;
    }

    bool Shader::linkShader(VertexShaderRef& vs, PixelShaderRef& ps, VertexDeclarationRef& decl, Descriptor* vsDesc, Descriptor* psDesc)
    {
        LASSERT(vsDesc != NULL);
        LASSERT(psDesc != NULL);

        u32 program = glCreateProgram();
        glAttachShader(program, vsDesc->id_);
        glAttachShader(program, psDesc->id_);

        decl.bindAttributes(program);

        glLinkProgram(program);
        GLint status = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if(status == GL_FALSE){
            GLint length = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

            GLchar *infoLog = LIME_NEW GLchar[length+1];
            glGetProgramInfoLog(program, length, &length, infoLog);
            infoLog[length] = '\0';
            Debug_LogError(infoLog);
            lcore::Log("Shader fail to link program: %s", infoLog);
            LIME_DELETE_ARRAY(infoLog);
            return false;
        }

        Descriptor *desc = lgraphics::Graphics::getDevice().allocate();
        LASSERT(desc != NULL);

        desc->id_ = program;

        //２回参照カウントを増やす
        desc->counter_ = 2;

        VertexShaderRef vsTmp(desc, vsDesc);
        PixelShaderRef psTmp(desc, psDesc);
        vs.swap(vsTmp);
        ps.swap(psTmp);

        return true;
    }

    // シェーダコンパイル
    bool Shader::compileShaderFromMemory(
        u32 shader,
        const char* str,
        u32,
        s32 numDefines,
        const char** defines)
    {
        LASSERT(numDefines<=MaxDefines);
        GLboolean hasCompiler = GL_FALSE;
        glGetBooleanv(GL_SHADER_COMPILER, &hasCompiler);
        if(hasCompiler == GL_FALSE){
            static const char* msg = "Shader have no comiler";
            Debug_LogError(msg);
            lcore::Log(msg);
            return false;
        }

        //マクロ配列作成
        const char* strs[MaxDefines+1];
        for(s32 i=0; i<numDefines; ++i){
            strs[i] = defines[i];
        }
        strs[numDefines] = str;

        glShaderSource(
            shader,
            numDefines+1,
            strs,
            NULL);

        glCompileShader(shader);

        GLint status = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if(status == GL_FALSE){
            //コンパイル失敗
            GLint length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

            GLchar *infoLog = LIME_NEW GLchar[length+1];
            glGetShaderInfoLog(shader, length, &length, infoLog);
            infoLog[length] = '\0';
            Debug_LogError(infoLog);

#if defined(ANDROID)
            lcore::Log("Shader fail to compile:(%d)%s", length, infoLog);
#endif
            LIME_DELETE_ARRAY(infoLog);
            return false;
        }
        return true;
    }


    //------------------------------------------------------------
    //---
    //--- ShaderRefBase
    //---
    //------------------------------------------------------------
    ShaderRefBase::ShaderRefBase(const ShaderRefBase& rhs)
        :programID_(rhs.programID_)
        ,key_(rhs.key_)
    {
        if(programID_ != NULL){
            programID_->addRef();
        }
    }

    void ShaderRefBase::destroy()
    {
        if(programID_!=NULL){
            if(programID_->counter_ <= 1){
                glDeleteProgram(programID_->id_);
            }

            LRELEASE(programID_);
        }
    }

    void ShaderRefBase::setMatrix(HANDLE handle, const lmath::Matrix44& matrix, u8 transpose)
    {
        LASSERT(handle >=0);
        LASSERT(programID_ != NULL);
        glUniformMatrix4fv(handle, 1, transpose, reinterpret_cast<const f32*>(&matrix));
    }

    void ShaderRefBase::setMatrix(HANDLE handle, const lmath::Matrix43& matrix)
    {
        LASSERT(handle >=0);
        LASSERT(programID_ != NULL);
        glUniform4fv(handle, 3, reinterpret_cast<const f32*>(&matrix));
    }


    void ShaderRefBase::setFloat(HANDLE handle, f32 value)
    {
        LASSERT(handle >=0);
        LASSERT(programID_ != NULL);
        glUniform1f(handle, value);
    }

    void ShaderRefBase::setVector2(HANDLE handle, const lmath::Vector2& vector)
    {
        LASSERT(handle >=0);
        LASSERT(programID_ != NULL);
        glUniform2f(handle, vector._x, vector._y);
    }

    void ShaderRefBase::setVector3(HANDLE handle, const lmath::Vector3& vector)
    {
        //LASSERT(handle >=0);
        LASSERT(programID_ != NULL);
        glUniform3f(handle, vector._x, vector._y, vector._z);
    }

    void ShaderRefBase::setVector4(HANDLE handle, const lmath::Vector4& vector)
    {
        //LASSERT(handle >=0);
        LASSERT(programID_ != NULL);
        glUniform4f(handle, vector._x, vector._y, vector._z, vector._w);
    }

    void ShaderRefBase::setVector4Array(HANDLE handle, const lmath::Vector4* vector, u32 count)
    {
        //LASSERT(handle >=0);
        LASSERT(programID_ != NULL);
        glUniform4fv(handle, count, reinterpret_cast<const f32*>(vector));
    }

    lgraphics::HANDLE ShaderRefBase::getHandle(const char* name)
    {
        LASSERT(programID_ != NULL);
        LASSERT(name != NULL);

        HANDLE handle = glGetUniformLocation(programID_->id_, name);
        return handle;
    }

    u32 ShaderRefBase::getSamplerIndex(const char* name)
    {
        LASSERT(name != NULL);
        HANDLE handle = glGetUniformLocation(programID_->id_, name);
        if(handle<0){
            return INVALID_SAMPLER_INDEX;
        }

        return static_cast<u32>(handle);
    }

    void ShaderRefBase::attach() const
    {
        LASSERT(programID_ != NULL);
        glUseProgram(programID_->id_);
    }


    //------------------------------------------------------------
    //---
    //--- PixelShaderRef
    //---
    //------------------------------------------------------------
    PixelShaderRef::PixelShaderRef(const PixelShaderRef& rhs)
        :ShaderRefBase(rhs)
        ,shaderID_(rhs.shaderID_)
    {
        if(shaderID_){
            shaderID_->addRef();
        }
    }

    void PixelShaderRef::destroy()
    {
        ShaderRefBase::destroy();
        if(shaderID_!=NULL){
            if(shaderID_->counter_ <= 1){
                glDeleteShader(shaderID_->id_);
            }

            LRELEASE(shaderID_);
        }
    }
   

    //------------------------------------------------------------
    //---
    //--- VertexShaderRef
    //---
    //------------------------------------------------------------
    VertexShaderRef::VertexShaderRef(const VertexShaderRef& rhs)
        :ShaderRefBase(rhs)
        ,shaderID_(rhs.shaderID_)
    {
        if(shaderID_){
            shaderID_->addRef();
        }
    }

    void VertexShaderRef::destroy()
    {
        ShaderRefBase::destroy();
        if(shaderID_!=NULL){
            if(shaderID_->counter_ <= 1){
                glDeleteShader(shaderID_->id_);
            }

            LRELEASE(shaderID_);
        }
    }
}
