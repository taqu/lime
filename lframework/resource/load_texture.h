#ifndef INC_LFRAMEWORK_LOADTEXTURE_H_
#define INC_LFRAMEWORK_LOADTEXTURE_H_
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
#endif //INC_LFRAMEWORK_LOADTEXTURE_H_
