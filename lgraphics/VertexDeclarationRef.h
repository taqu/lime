#ifndef INC_LGRAPHICS_VERTEXDECLARATIONREF_H_
#define INC_LGRAPHICS_VERTEXDECLARATIONREF_H_
/**
@file VertexDeclarationRef.h
@author t-sakai
@date 2010/01/08 create
@date 2010/06/06 add gles1 implementation
@date 2010/12/12 add gles2
*/
#include "Config.h"

#if defined(LGFX_DX9)
#include "dx9/VertexDeclarationRef.h"

#elif defined(LGFX_GLES1)
#include "gles1/VertexDeclarationRef.h"

#elif defined(LGFX_GLES2)
#include "gles2/VertexDeclarationRef.h"
#endif

#endif //INC_LGRAPHICS_VERTEXDECLARATIONREF_H_
