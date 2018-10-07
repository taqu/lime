#ifndef INC_LSOUND_PLAYER_H_
#define INC_LSOUND_PLAYER_H_
/**
@file Player.h
@author t-sakai
@date 2015/07/06 create
*/
#include "lsound_api.h"

#if defined(LSOUND_API_WASAPI)
#include "Wasapi/Player.h"

#elif defined(_APPLE_)
#include "OpenAL/Player.h"

#elif defined(LSOUND_API_OPENSL)
#include "OpenSL/Player.h"
#endif

#endif //INC_LSOUND_PLAYER_H_
