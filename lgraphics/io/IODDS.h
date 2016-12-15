#ifndef INC_LGRAPHICS_IODDS_H__
#define INC_LGRAPHICS_IODDS_H__
/**
@file IODDS.h
@author t-sakai
@date 2016/05/08 create

*/
#include "Config.h"

#if defined(LGFX_DX9)
#include "../dx9/io/IODDS.h"

#elif defined(LGFX_DX11)
#include "../dx11/io/IODDS.h"

#elif defined(LGFX_GLES1)
#include "../gles1/io/IODDS.h"

#elif defined(LGFX_GLES2)
#include "../gles2/io/IODDS.h"
#endif

#endif //INC_LGRAPHICS_IODDS_H__
