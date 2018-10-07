#ifndef INC_LGRAPHICS_SURFACEREF_H_
#define INC_LGRAPHICS_SURFACEREF_H_
/**
@file SurfaceRef.h
@author t-sakai
@date 2010/01/08 create
@date 2010/12/12 add gles2
*/
#include "Config.h"

#if defined(LGFX_DX9)
#include "dx9/SurfaceRef.h"

#elif defined(LGFX_GLES2)
#include "gles2/SurfaceRef.h"

#endif

#endif //INC_LGRAPHICS_SURFACEREF_H_
