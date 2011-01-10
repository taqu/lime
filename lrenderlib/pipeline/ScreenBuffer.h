#ifndef INC_SCREENBUFFER_H__
#define INC_SCREENBUFFER_H__
/**
@file ScreenBuffer.h
@author t-sakai
@date 2010/01/06 create

*/
#include "../lrendercore.h"

namespace lgraphics
{
    class SurfaceOffscreen;
}

namespace lrender
{
    class ScreenBuffer
    {
    public:
        ScreenBuffer();
        ScreenBuffer(u32 width, u32 height, BufferFormat format);
        ~ScreenBuffer();

        void create(u32 width, u32 height, BufferFormat format);

    private:
        s32 pitch_;
        void *bits_;
        u32 width_;
        u32 height_;
        lgraphics::SurfaceOffscreen *surface_;
        BufferFormat format_;
    };
}

#endif //INC_SCREENBUFFER_H__
