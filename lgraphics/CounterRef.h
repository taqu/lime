#ifndef INC_LGRAPHICS_COUNTERREF_H_
#define INC_LGRAPHICS_COUNTERREF_H_
/**
@file CounterRef.h
@author t-sakai
@date 2014/10/10 create
*/
#include "Config.h"

#if defined(LGFX_DX9)

#elif defined(LGFX_DX11)
#include "dx11/CounterRef.h"

#elif defined(LGFX_GLES1)

#elif defined(LGFX_GLES2)
#endif

#endif //INC_LGRAPHICS_COUNTERREF_H_
