/**
@file TextureBuffer.cpp
@author t-sakai
@date 2010/01/06 create

*/
#include "TextureBuffer.h"
#include <lgraphics/api/Texture.h>

namespace lrender
{
    TextureBuffer::TextureBuffer()
        :texture_(NULL)
        ,pitch_(-1)
        ,bits_(NULL)
    {
    }

    TextureBuffer::TextureBuffer(u32 width, u32 height, BufferFormat format)
        :texture_(NULL)
        ,pitch_(-1)
        ,bits_(NULL)
    {
        create(width, height, format);
    }

    TextureBuffer::~TextureBuffer()
    {
        LIME_DELETE(texture_);
    }

    void TextureBuffer::create(u32 width, u32 height, BufferFormat format)
    {
        LASSERT(texture_ == NULL);

        lgraphics::BufferFormat gformat = static_cast<lgraphics::BufferFormat>( toGraphicsFormat(format) );
        texture_ = lgraphics::Texture::create(width, height, 1, lgraphics::Usage_None, gformat, lgraphics::Pool_Managed, 0);

        width_ = width;
        height_ = height;
        format_ = format;
    }

    bool TextureBuffer::lock()
    {
        LASSERT(texture_ != NULL);
        lgraphics::Texture::LockedRect rect;
        bool ret = texture_->lock(0, rect, lgraphics::Lock_None);

        if(ret){
            pitch_ = rect.pitch_;
            bits_ = rect.bits_;
        }
        return ret;
    }

    void TextureBuffer::unlock()
    {
        LASSERT(texture_ != NULL);

        texture_->unlock(0);
        pitch_ = -1;
        bits_ = NULL;
    }

    void TextureBuffer::debugout(const char* filename)
    {
        LASSERT(texture_ != NULL);
        texture_->debugout(filename);
    }
}
