#ifndef INC_LFRAMEWORK_LOADTEXTURE_H__
#define INC_LFRAMEWORK_LOADTEXTURE_H__
/**
@file load_texture.h
@author t-sakai
@date 2016/11/23 create
*/
#include "load.h"

namespace lfw
{
    class LoadTexture
    {
    public:
        u8 type_;
        u8 filterType_;
        u8 addressUVW_;
        u8 compFunc_;
        f32 borderColor_;
        Char name_[MaxFileNameSize];
    };
}
#endif //INC_LFRAMEWORK_LOADTEXTURE_H__
