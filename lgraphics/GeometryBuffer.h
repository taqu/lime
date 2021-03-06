﻿#ifndef INC_LGRAPHICS_GEOMETRYBUFFER_H_
#define INC_LGRAPHICS_GEOMETRYBUFFER_H_
/**
@file GeometryBuffer.h
@author t-sakai
@date 2010/01/08 create
@date 2010/07/10 add gles1 implementation
@date 2010/12/12 add gles2
*/
#include "Config.h"

#if defined(LGFX_DX9)
#include "dx9/GeometryBuffer.h"

#elif defined(LGFX_DX11)
#include "dx11/GeometryBuffer.h"

#elif defined(LGFX_GLES1)
#include "gles1/GeometryBuffer.h"

#elif defined(LGFX_GLES2)
#include "gles2/GeometryBuffer.h"
#endif

#endif //INC_LGRAPHICS_GEOMETRYBUFFER_H_
