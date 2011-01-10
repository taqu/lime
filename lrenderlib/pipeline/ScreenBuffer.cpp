/**
@file ScreenBuffer.cpp
@author t-sakai
@date 2010/01/06 create

*/
#include "ScreenBuffer.h"
#include <lgraphics/dx/Surface.h>

namespace lrender
{
    ScreenBuffer::ScreenBuffer()
        :surface_(NULL)
        ,pitch_(-1)
        ,bits_(NULL)
    {
    }

    ScreenBuffer::ScreenBuffer(u32 width, u32 height, BufferFormat format)
        :pitch_(-1)
        ,bits_(NULL)
    {
        create(width, height, format);
    }

    ScreenBuffer::~ScreenBuffer()
    {
        LIME_DELETE(surface_);
    }

    void ScreenBuffer::create(u32 width, u32 height, BufferFormat format)
    {
        LASSERT(surface_ == NULL);

        lgraphics::BufferFormat gformat = toGraphicsFormat(format);
        surface_ = lgraphics::SurfaceOffscreen::create(width, height, gformat);

        width_ = width;
        height_ = height;
        format_ = format;
    }

    //bool ScreenBuffer::lock()
    //{
    //    LASSERT(surface_ != NULL);
    //    lgraphics::LockedRect rect;
    //    bool ret = surface_->lock(rect);

    //    if(ret){
    //        pitch_ = rect.pitch_;
    //        bits_ = rect.bits_;
    //    }
    //    return ret;
    //}

    //void ScreenBuffer::unlock()
    //{
    //    LASSERT(surface_ != NULL);

    //    surface_->unlock();
    //    pitch_ = -1;
    //    bits_ = NULL;
    //}
}
