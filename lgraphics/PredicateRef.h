#ifndef INC_LGRAPHICS_PREDICATEREF_H_
#define INC_LGRAPHICS_PREDICATEREF_H_
/**
@file PredicateRef.h
@author t-sakai
@date 2014/10/10 create
*/
#include "Config.h"

#if defined(LGFXDX9)

#elif defined(LGFX_DX11)
#include "dx11/PredicateRef.h"

#elif defined(LGFX_GLES1)

#elif defined(LGFX_GLES2)
#endif

#endif //INC_LGRAPHICS_PREDICATEREF_H_
