#ifndef INC_LGRAPHICS_IOBMP_H_
#define INC_LGRAPHICS_IOBMP_H_
/**
@file IOBMP.h
@author t-sakai
@date 2016/05/08 create

*/
#include "Config.h"

#if defined(LGFX_DX9)
#include "../dx9/io/IOBMP.h"

#elif defined(LGFX_DX11)
#include "../dx11/io/IOBMP.h"

#elif defined(LGFX_GLES1)
#include "../gles1/io/IOBMP.h"

#elif defined(LGFX_GLES2)
#include "../gles2/io/IOBMP.h"
#endif

#endif //INC_LGRAPHICS_IOBMP_H_
