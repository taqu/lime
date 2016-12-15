#ifndef INC_LGRAPHICS_IOPNG_H__
#define INC_LGRAPHICS_IOPNG_H__
/**
@file IOPNG.h
@author t-sakai
@date 2016/05/08 create

*/
#include "Config.h"

#if defined(LGFX_DX9)
#include "../dx9/io/IOPNG.h"

#elif defined(LGFX_DX11)
#include "../dx11/io/IOPNG.h"

#elif defined(LGFX_GLES1)
#include "../gles1/io/IOPNG.h"

#elif defined(LGFX_GLES2)
#include "../gles2/io/IOPNG.h"
#endif

#endif //INC_LGRAPHICS_IOPNG_H__
