#ifndef INC_LSCENE_LLOAD_TEXTURE_H__
#define INC_LSCENE_LLOAD_TEXTURE_H__
/**
@file load_texture.h
@author t-sakai
@date 2012/04/12 create
*/
#include "load.h"

namespace lscene
{
namespace lload
{
    class Texture
    {
    public:
        u32 type_;
        Char name_[MaxFileNameSize];
    };
}
}
#endif //INC_LSCENE_LLOAD_TEXTURE_H__
