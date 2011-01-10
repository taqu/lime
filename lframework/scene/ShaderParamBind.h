#ifndef INC_LSCENE_SHADERPARAMBIND_H__
#define INC_LSCENE_SHADERPARAMBIND_H__
/**
@file ShaderParamBind.h
@author t-sakai
@date 2009/08/16 create
*/
#include "lscene.h"
#include <lgraphics/lgraphicscore.h>

namespace lmath
{
    class Vector2;
    class Vector3;
    class Vector4;
    class Matrix44;
}

namespace lgraphics
{
    class ShaderRefBase;
}

namespace lscene
{
    class ShaderParamBind
    {
        union Value
        {
            const f32 *f32_;
            const lmath::Vector2 *vec2_;
            const lmath::Vector3 *vec3_;
            const lmath::Vector4 *vec4_;
            const lmath::Matrix44 *mat44_;
        };

    public:
        typedef void(*BindFunc)(lgraphics::ShaderRefBase& shader, lgraphics::HANDLE handle, Value&);        

        ShaderParamBind()
            :bind_(NULL)
        {
        }

        ~ShaderParamBind()
        {
        }

        void set(lgraphics::HANDLE handle, const f32* value)
        {
            bind_ = bindFloat;
            value_.f32_ = value;
            handle_ = handle;
        }

        void set(lgraphics::HANDLE handle, const lmath::Vector2* value)
        {
            bind_ = bindVector2;
            value_.vec2_ = value;
            handle_ = handle;
        }

        void set(lgraphics::HANDLE handle, const lmath::Vector3* value)
        {
            bind_ = bindVector3;
            value_.vec3_ = value;
            handle_ = handle;
        }

        void set(lgraphics::HANDLE handle, const lmath::Vector4* value)
        {
            bind_ = bindVector4;
            value_.vec4_ = value;
            handle_ = handle;
        }

        void set(lgraphics::HANDLE handle, const lmath::Matrix44* value)
        {
            bind_ = bindMatrix44;
            value_.mat44_ = value;
            handle_ = handle;
        }

        void bind(lgraphics::ShaderRefBase& shader)
        {
            LASSERT(bind_ != NULL);
            bind_(shader, handle_, value_);
        }

    private:
        static void bindFloat(lgraphics::ShaderRefBase& shader, lgraphics::HANDLE handle, Value& value);
        static void bindVector2(lgraphics::ShaderRefBase& shader, lgraphics::HANDLE handle, Value& value);
        static void bindVector3(lgraphics::ShaderRefBase& shader, lgraphics::HANDLE handle, Value& value);
        static void bindVector4(lgraphics::ShaderRefBase& shader, lgraphics::HANDLE handle, Value& value);
        static void bindMatrix44(lgraphics::ShaderRefBase& shader, lgraphics::HANDLE handle, Value& value);

        BindFunc bind_;
        Value value_;
        lgraphics::HANDLE handle_;
    };
}

#endif //INC_LSCENE_SHADERPARAMBIND_H__
