#ifndef INC_LGRAPHICS_VERTEXBUFFERREF_H__
#define INC_LGRAPHICS_VERTEXBUFFERREF_H__
/**
@file VertexBufferRef.h
@author t-sakai
@date 2010/01/08 create
@date 2010/06/06 add gles1 implementation
@date 2010/12/12 add gles2
*/
#include "Config.h"

#if defined(LGFX_DX9)
#include "dx9/VertexBufferRef.h"

#elif defined(LGFX_DX11)
#include "dx11/VertexBufferRef.h"

#elif defined(LGFX_GLES1)
#include "gles1/VertexBufferRef.h"

#elif defined(LGFX_GLES2)
#include "gles2/VertexBufferRef.h"
#endif

#endif //INC_LGRAPHICS_VERTEXBUFFERREF_H__
