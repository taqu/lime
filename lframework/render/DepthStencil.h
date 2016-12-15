#ifndef INC_LFRAMEWORK_DEPTHSTENCIL_H__
#define INC_LFRAMEWORK_DEPTHSTENCIL_H__
/**
@file DepthStencil.h
@author t-sakai
@date 2016/12/06 create
*/
#include "../lframework.h"
#include <lgraphics/TextureRef.h>

namespace lfw
{
    struct DepthStencil
    {
        lgfx::Texture2DRef texture_;
        lgfx::DepthStencilViewRef depthStencilView_;
        lgfx::ShaderResourceViewRef shaderResourceViewDepth_;
        lgfx::ShaderResourceViewRef shaderResourceViewStencil_;
    };
}
#endif //INC_LFRAMEWORK_DEPTHSTENCIL_H__
