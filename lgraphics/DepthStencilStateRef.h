#ifndef INC_LGRAPHICS_DEPTHSTENCILSTATEREF_H_
#define INC_LGRAPHICS_DEPTHSTENCILSTATEREF_H_
/**
@file DepthStencilStateRef.h
@author t-sakai
@date 2017/01/15 create
*/
#include "Config.h"

#if defined(LGFX_DX9)

#elif defined(LGFX_DX11)
#include "dx11/DepthStencilStateRef.h"

#elif defined(LGFX_GLES1)

#elif defined(LGFX_GLES2)
#endif

#endif //INC_LGRAPHICS_DEPTHSTENCILSTATEREF_H_
