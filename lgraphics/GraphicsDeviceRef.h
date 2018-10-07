#ifndef INC_LGRAPHICS_GRAPHICSDEVICEREF_H_
#define INC_LGRAPHICS_GRAPHICSDEVICEREF_H_
/**
@file GraphicsDeviceRef.h
@author t-sakai
@date 2010/01/08 create
@date 2010/06/06 add gles1 implementation
@date 2010/12/12 add gles2
*/
#include "Config.h"

#if defined(LGFX_DX9)
#include "dx9/GraphicsDeviceRef.h"

#elif defined(LGFX_DX11)
#include "dx11/GraphicsDeviceRef.h"

#elif defined(LGFX_GLES1)
#include "gles1/GraphicsDeviceRef.h"

#elif defined(LGFX_GLES2)
#include "gles2/GraphicsDeviceRef.h"
#endif

#endif //INC_LGRAPHICS_GRAPHICSDEVICEREF_H_
