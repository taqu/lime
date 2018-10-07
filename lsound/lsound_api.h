#ifndef INC_LSOUND_LSOUND_API_H_
#define INC_LSOUND_LSOUND_API_H_
/**
@file lsound_api.h
@author t-sakai
@date 2015/07/06 create
*/
#if defined(_WIN32)
#define LSOUND_API_WASAPI
//#define LSOUND_API_OPENAL

#elif defined(_APPLE_)
#define LSOUND_API_OPENAL

#elif defined(ANDROID)
#define LSOUND_API_OPENSL
#endif
#endif //INC_LSOUND_LSOUND_API_H_
