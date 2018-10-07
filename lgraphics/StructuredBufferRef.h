#ifndef INC_LGRAPHICS_STRUCTUREDBUFFERREF_H_
#define INC_LGRAPHICS_STRUCTUREDBUFFERREF_H_
/**
@file StructuredBufferRef.h
@author t-sakai
@date 2015/12/22 create
*/
#include "Config.h"

#if defined(LGFX_DX9)

#elif defined(LGFX_DX11)
#include "dx11/StructuredBufferRef.h"

#elif defined(LGFX_GLES1)

#elif defined(LGFX_GLES2)
#endif

#endif //INC_LGRAPHICS_STRUCTUREDBUFFERREF_H_
