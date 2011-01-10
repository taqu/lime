/**
@file ShaderParamBind.cpp
@author t-sakai
@date 2009/08/16
*/
#include "ShaderParamBind.h"

#include <lgraphics/api/ShaderRef.h>
#include <lmath/lmath.h>

namespace lscene
{
    using lgraphics::HANDLE;

    void ShaderParamBind::bindFloat(lgraphics::ShaderRefBase& shader, HANDLE handle, Value& value)
    {
        shader.setFloat(handle, *(value.f32_));
    }

    void ShaderParamBind::bindVector2(lgraphics::ShaderRefBase& shader, HANDLE handle, Value& value)
    {
        shader.setVector2(handle, *(value.vec2_));
    }

    void ShaderParamBind::bindVector3(lgraphics::ShaderRefBase& shader, HANDLE handle, Value& value)
    {
        shader.setVector3(handle, *(value.vec3_));
    }

    void ShaderParamBind::bindVector4(lgraphics::ShaderRefBase& shader, HANDLE handle, Value& value)
    {
        shader.setVector4(handle, *(value.vec4_));
    }

    void ShaderParamBind::bindMatrix44(lgraphics::ShaderRefBase& shader, HANDLE handle, Value& value)
    {
        shader.setMatrix(handle, *(value.mat44_));
    }
}
