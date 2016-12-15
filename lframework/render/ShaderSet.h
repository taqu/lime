#ifndef INC_LFRAMEWORK_SHADERSET_H__
#define INC_LFRAMEWORK_SHADERSET_H__
/**
@file ShaderSet.h
@author t-sakai
@date 2016/11/12 create
*/
#include "../lframework.h"
#include <lgraphics/ShaderRef.h>

namespace lgfx
{
    class VertexShaderRef;
    class DomainShaderRef;
    class HullShaderRef;
    class GeometryShaderRef;
    class PixelShaderRef;
}

namespace lfw
{
    struct ShaderSet
    {
        s32 id_;
        lgfx::VertexShaderRef vs_;
        lgfx::DomainShaderRef ds_;
        lgfx::HullShaderRef hs_;
        lgfx::GeometryShaderRef gs_;
        lgfx::PixelShaderRef ps_;
    };
}
#endif //INC_LFRAMEWORK_SHADERSET_H__
