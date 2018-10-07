#ifndef INC_LGRAPHICS_IOJPG_H_
#define INC_LGRAPHICS_IOJPG_H_
/**
@file IOJPG.h
@author t-sakai
@date 2016/11/23 create
*/
#include "Config.h"

#if defined(LGFX_DX9)
#include "../dx9/io/IOJPG.h"

#elif defined(LGFX_DX11)
#include "../dx11/io/IOJPG.h"

#elif defined(LGFX_GLES1)
#include "../gles1/io/IOJPG.h"

#elif defined(LGFX_GLES2)
#include "../gles2/io/IOJPG.h"
#endif

#endif //INC_LGRAPHICS_IOJPG_H_
