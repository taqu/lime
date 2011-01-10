#ifndef INC_LFRAMEWORK_ASCIIRENDER_H__
#define INC_LFRAMEWORK_ASCIIRENDER_H__
/**
@file ASCIIRender.h
@author t-sakai
@date 2010/02/21 create
*/
#include "TextRenderBase.h"
#include <lcore/Buffer.h>
#include <lmath/Vector3.h>
#include <lmath/Vector2.h>
#include <lmath/Color.h>
#include <lgraphics/api/Enumerations.h>
#include <lgraphics/api/VertexDeclarationRef.h>
#include <lgraphics/api/TextureRef.h>

#include "../2d/State2D.h"

namespace lgraphics
{
    class Texture;
}

namespace lframework
{
    class ASCIIRender : public TextRenderBase
    {
    public:
        ASCIIRender();
        virtual ~ASCIIRender();


        static bool create(
            ASCIIRender& render,
            const char* filename,
            u32 width,
            u32 height,
            u32 maxChars,
            lgraphics::Pool pool);

        inline void clear();
        void addString(f32 x, f32 y, f32 z, lmath::Color color, const char* str, u32 length);
        void addString(f32 x, f32 y, f32 z, lmath::Color color, const char* str);

        void draw();

        void swap(ASCIIRender& rhs);
    private:
        struct Vertex
        {
            lmath::Vector3 position_;
            lmath::Vector2 uv_;
            lmath::Color color_;
        };

        struct Entry
        {
            f32 u_[2];
            f32 v_[2];
        };


        ASCIIRender(u32 width, u32 height, u32 maxChars, const lgraphics::TextureRef& texture);

        inline void addChar(f32 x, f32 y, f32 z, lmath::Color color, s8 c);

        static const u32 CharBegin = 0x21U;
        static const u32 CharEnd = 0x7fU;
        static const u32 CharNum = (CharEnd - CharBegin + 1);
        static const u32 CharNumVertical = 10;
        static const u32 CharNumHorizontal = 10;

        State2D state2D_;

        u32 width_; /// １文字の幅
        u32 height_;/// １文字の高さ

        u32 maxChars_;
        u32 numChars_;

        lgraphics::TextureRef texture_;

        Entry *entries_;
        Vertex *vertices_;
        lcore::Buffer buffer_;

    };

    //-----------------------------------------
    inline void ASCIIRender::clear()
    {
        numChars_ = 0;
    }
}

#endif //INC_LFRAMEWORK_ASCIIRENDER_H__
