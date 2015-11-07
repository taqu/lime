#ifndef INC_LSCENE_LRENDER_TEXTURE_H__
#define INC_LSCENE_LRENDER_TEXTURE_H__
/**
@file Texture.h
@author t-sakai
@date 2015/05/05 create
*/
#include "render.h"
#include <lgraphics/api/TextureRef.h>
#include <lgraphics/api/SamplerStateRef.h>
#include <lgraphics/api/ViewRef.h>

namespace lscene
{
namespace lrender
{
    class Texture2D
    {
    public:
        lgraphics::Texture2DRef texture_;
        lgraphics::SamplerStateRef sampler_;
        lgraphics::ShaderResourceViewRef srv_;
    };

    class Texture3D
    {
    public:
        lgraphics::Texture3DRef texture_;
        lgraphics::SamplerStateRef sampler_;
        lgraphics::ShaderResourceViewRef srv_;
    };
}
}
#endif //INC_LSCENE_LRENDER_TEXTURE_H__
