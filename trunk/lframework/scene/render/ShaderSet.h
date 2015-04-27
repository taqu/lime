#ifndef INC_LSCENE_LRENDER_SHADERSET_H__
#define INC_LSCENE_LRENDER_SHADERSET_H__
/**
@file ShaderSet.h
@author t-sakai
@date 2014/12/16 create
*/
#include "render.h"

namespace lgraphics
{
    class VertexShaderRef;
    class GeometryShaderRef;
    class PixelShaderRef;
    class ComputeShaderRef;
}

namespace lscene
{
namespace lrender
{
    struct ShaderSet
    {
        s32 key_;
        lgraphics::VertexShaderRef* vs_;
        lgraphics::GeometryShaderRef* gs_;
        lgraphics::PixelShaderRef* ps_;

        lgraphics::VertexShaderRef* lodVS_;
        lgraphics::GeometryShaderRef* lodGS_;
        lgraphics::PixelShaderRef* lodPS_;
    };
}
}
#endif //INC_LSCENE_LRENDER_SHADERSET_H__
