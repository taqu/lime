﻿#ifndef INC_LGRAPHICS_RENDERSTATEREF_H__
#define INC_LGRAPHICS_RENDERSTATEREF_H__
/**
@file RenderStateRef.h
@author t-sakai
@date 2010/01/08 create
@date 2010/12/12 add gles2
*/
#include "Config.h"

#if defined(LGFX_DX9)
#include "dx9/RenderStateRef.h"

#elif defined(LGFX_GLES1)
#include "gles1/RenderStateRef.h"

#elif defined(LGFX_GLES2)
#include "gles2/RenderStateRef.h"
#endif

#endif //INC_LGRAPHICS_RENDERSTATEREF_H__