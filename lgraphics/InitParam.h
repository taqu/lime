#ifndef INC_LGRAPHICS_INITPARAM_H__
#define INC_LGRAPHICS_INITPARAM_H__
/**
@file InitParam.h
@author t-sakai
@date 2010/03/24 create
@date 2010/06/06 add gles1 implementation
@date 2010/12/12 add gles2
*/
#include "Config.h"

#if defined(LGFX_DX9)
#include "dx9/InitParam.h"

#elif defined(LGFX_DX11)
#include "dx11/InitParam.h"

#elif defined(LGFX_GLES1)
#include "gles1/InitParam.h"

#elif defined(LGFX_GLES2)
#include "gles2/InitParam.h"
#endif

#endif //INC_LGRAPHICS_INITPARAM_H__
