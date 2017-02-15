#ifndef INC_LFRAMEWORK_RENDERTARGET_H__
#define INC_LFRAMEWORK_RENDERTARGET_H__
/**
@file RenderTarget.h
@author t-sakai
@date 2016/12/06 create
*/
#include "../lframework.h"
#include <lgraphics/TextureRef.h>

namespace lfw
{
    struct RenderTarget
    {
        lgfx::Texture2DRef texture_;
        lgfx::RenderTargetViewRef renderTargetView_;
        lgfx::ShaderResourceViewRef shaderResourceView_;
    };

    struct RenderTargetUAV
    {
        lgfx::Texture2DRef texture_;
        lgfx::RenderTargetViewRef renderTargetView_;
        lgfx::ShaderResourceViewRef shaderResourceView_;
        lgfx::UnorderedAccessViewRef unorderedAccessView_;
    };
}
#endif //INC_LFRAMEWORK_RENDERTARGET_H__
