#ifndef INC_TEXTUREBUFFER_H__
#define INC_TEXTUREBUFFER_H__
/**
@file TextureBuffer.h
@author t-sakai
@date 2010/01/06 create

*/
#include "../lrendercore.h"

namespace lgraphics
{
    class Texture;
}

namespace lrender
{
    class TextureBuffer
    {
    public:
        TextureBuffer();
        TextureBuffer(u32 width, u32 height, BufferFormat format);
        ~TextureBuffer();

        void create(u32 width, u32 height, BufferFormat format);

        bool lock();
        void unlock();

        template<class T>
        const T& get(u32 x, u32 y) const;

        template<class T>
        void set(u32 x, u32 y, const T& value);

        u32 getWidth() const{ return width_;}
        u32 getHeight() const{ return height_;}

        void debugout(const char* filename);
    private:
        s32 pitch_;
        void *bits_;
        u32 width_;
        u32 height_;
        lgraphics::Texture *texture_;
        BufferFormat format_;
    };

    template<class T>
    const T& TextureBuffer::get(u32 x, u32 y) const
    {
        LASSERT(bits_ != NULL);
        LASSERT(0<=x && x<width_);
        LASSERT(0<=y && y<height_);
        const char* bits = static_cast<const char*>(bits_);
        bits += y*pitch_;
        return reinterpret_cast<const T&>( bits[x*sizeof(T)] );
    }

    template<class T>
    void TextureBuffer::set(u32 x, u32 y, const T& value)
    {
        LASSERT(bits_ != NULL);
        LASSERT(0<=x && x<width_);
        LASSERT(0<=y && y<height_);
        char* bits = static_cast<char*>(bits_);
        bits += y*pitch_;
        reinterpret_cast<T&>( bits[x*sizeof(T)] ) = value;
    }
}
#endif //INC_TEXTUREBUFFER_H__
