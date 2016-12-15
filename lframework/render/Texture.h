#ifndef INC_LFRAMEWORK_TEXTURE_H__
#define INC_LFRAMEWORK_TEXTURE_H__
/**
@file Texture.h
@author t-sakai
@date 2016/11/23 create
*/
#include "../lframework.h"
#include <lgraphics/TextureRef.h>
#include <lgraphics/SamplerStateRef.h>
#include <lgraphics/ViewRef.h>

namespace lfw
{
    class Texture2D
    {
    public:
        lgfx::Texture2DRef texture_;
        lgfx::SamplerStateRef sampler_;
        lgfx::ShaderResourceViewRef srv_;
    };

    class Texture3D
    {
    public:
        lgfx::Texture3DRef texture_;
        lgfx::SamplerStateRef sampler_;
        lgfx::ShaderResourceViewRef srv_;
    };
}
#endif //INC_LFRAMEWORK_TEXTURE_H__
