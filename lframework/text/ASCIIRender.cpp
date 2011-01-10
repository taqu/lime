/**
@file ASCIIRender.cpp
@author t-sakai
@date 2010/02/21 create
*/
#include "ASCIIRender.h"
#include <lgraphics/lgraphics.h>
#include <lgraphics/api/GraphicsDeviceRef.h>
#include <lgraphics/api/VertexDeclarationRef.h>

#include <lfont/lfont.h>

using namespace lgraphics;

namespace lframework
{
    namespace
    {
        void drawToBuffer(u8* buffer, const lfont::Glyph& glyph, s32 x, s32 y, s32 pitch, u32 maxLeft, u32 maxTop)
        {
            s32 offsetx = glyph.getLeft() - maxLeft;
            s32 offsety = maxTop - glyph.getTop();
            if(offsetx<0){
                offsetx = 0;
            }
            if(offsety<0){
                offsety = 0;
            }

            x += offsetx;
            y += offsety;

            s32 xmax = x + glyph.getWidth();
            s32 ymax = y + glyph.getHeight();

            //s32 p = 0;
            //s32 q = 0;
            for(s32 j=y, p=0; j<ymax; ++j, ++p){
                for(s32 i=x, q=0; i<xmax; ++i, ++q){
                    s32 index = (j*pitch + i); //１ピクセル１バイト
                    u8 c = glyph(q, p);
                    buffer[index] = c;
                }
            }
        }

    }

    bool ASCIIRender::create(
            ASCIIRender& render,
            const char* filename,
            u32 width,
            u32 height,
            u32 maxChars,
            lgraphics::Pool pool)
    {
        LASSERT(filename != NULL);
        LASSERT(width > 0);
        LASSERT(height> 0);
        LASSERT(maxChars > 0);

        s32 error_no = 0;
        lfont::FontFace *face = lfont::Font::load(filename, 0, error_no);
        if(face == NULL){
//#if defined(_DEBUG)
//            LogInfo(lfont::getErrorMessage(error_no));
//#endif
            return false;
        }

        // 縦横１０個の文字がレンダリングできる２の累乗のサイズを計算
        //-----------------------------------------------------------
        const f32 l2 = (1.0f/lmath::log(2.0f));
        u32 tw = CharNumHorizontal * width;
        s32 nw = lmath::ceil(lmath::log( static_cast<f32>(tw) ) * l2);
        tw = static_cast<u32>( lmath::pow(2.0f, static_cast<f32>(nw)) );
        LASSERT(tw>1);

        u32 th = CharNumVertical * height;
        s32 nh = lmath::ceil(lmath::log( static_cast<f32>(th) ) * l2);
        th =  static_cast<u32>( lmath::pow(2.0f, static_cast<f32>(nh)) );
        LASSERT(th>1);

        Usage usage = (pool == Pool_Default)? Usage_Dynamic : Usage_None;
        TextureRef texture = Texture::create(
            tw, th, 1, usage, lgraphics::Buffer_A8, pool);

        if(texture.valid() == false){
            LIME_DELETE(face);
            return false;
        }


        // バッファ確保のため、仮に作成
        ASCIIRender tmpRender(width, height, maxChars, texture);

        Entry *entries = tmpRender.entries_;

        TextureRef::LockedRect lockedRect;
        if(texture.lock(0, lockedRect) == false){
            LIME_DELETE(face);
            return false;
        }

        face->setCharSize(width<<6, height<<6, 72, 72);

        //-------------------------------------------------------
        bool ret = face->loadChar('_', false);
        LASSERT(ret);
        u32 maxLeft = face->getGlyph().getLeft();
        ret = face->loadChar('|', false);
        LASSERT(ret);
        u32 maxTop = face->getGlyph().getTop();

        u8 *buffer = static_cast<u8*>(lockedRect.bits_);
        s32 x = 0;
        s32 y = 0;
        s32 i=0;
        s32 halfw = ((width&0x01U) == 0)? (width >> 1) : (width >> 1) + 1;


        f32 invW = 1.0f/(tw-1);
        f32 invH = 1.0f/(th-1);

        f32 unitU = invW * (halfw-1);
        f32 unitV = invH * (height-1);

        for(u32 c=CharBegin; c<CharEnd; ++c){
            bool ret = face->loadChar(c, false);
            if(ret){
                drawToBuffer(buffer, face->getGlyph(), x, y, lockedRect.pitch_, maxLeft, maxTop);
            }

            entries[i].u_[0] = x*invW;
            entries[i].u_[1] = entries[i].u_[0] + unitU;
            entries[i].v_[0] = y*invH;
            entries[i].v_[1] = entries[i].v_[0] + unitV;

            //x += face->getGlyph().getAdvanceX();
            //y += face->getGlyph().getAdvanceY();

            x += halfw;
            if(x>=static_cast<s32>(tw-halfw)){
                x = 0;
                y += height;
            }
            ++i;
        }

        texture.unlock(0);

        render.swap(tmpRender);

        LIME_DELETE(face);
        return true;
    }


    ASCIIRender::ASCIIRender()
        :width_(0)
        ,height_(0)
        ,maxChars_(0)
        ,numChars_(0)
        ,vertices_(NULL)
        ,entries_(NULL)
    {
    }

    ASCIIRender::ASCIIRender(u32 width, u32 height, u32 maxChars, const lgraphics::TextureRef& texture)
        :width_(width)
        ,height_(height)
        ,maxChars_(maxChars)
        ,numChars_(0)
        ,texture_(texture)
    {
        LASSERT(width_>0);
        LASSERT(height_>0);
        LASSERT(maxChars_>0);
        LASSERT(texture_.valid());

        //バッファ確保
        u32 numVertices = maxChars_ * 6;
        u32 vertexBufferSize = numVertices * sizeof(Vertex); //1文字６頂点
        u32 entryBufferSize = CharNum * sizeof(Entry);

        buffer_.reset(vertexBufferSize + entryBufferSize);

        vertices_ = buffer_.allocate<Vertex>(0, numVertices);
        entries_  = buffer_.allocate<Entry>(vertexBufferSize, CharNum);
    }

    ASCIIRender::~ASCIIRender()
    {
        if(entries_ != NULL){
            buffer_.destruct<Entry>(entries_, CharNum);
            entries_ = NULL;
        }

        if(vertices_ != NULL){
            buffer_.destruct<Vertex>(vertices_, maxChars_*6);
            vertices_ = NULL;
        }
    }

    void ASCIIRender::addString(f32 x, f32 y, f32 z, lmath::Color color, const char* str, u32 length)
    {
        for(u32 i=0; i<length; ++i){
            switch(str[i])
            {
            case ' ':
                break;

            case '\t':
                break;

            default:
                addChar(x, y, z, color, str[i]);
                break;
            }
            x += width_;
        }
    }

    void ASCIIRender::addString(f32 x, f32 y, f32 z, lmath::Color color, const char* str)
    {
        u32 i = 0;
        while(numChars_<maxChars_){
            if(str[i] == '\0'){
                break;
            }
            switch(str[i])
            {
            case ' ':
                break;

            case '\t':
                break;

            default:
                addChar(x, y, z, color, str[i]);
                break;
            }

            x += width_;
            ++i;
        }
    }

    void ASCIIRender::draw()
    {
        lgraphics::GraphicsDeviceRef &device = lgraphics::Graphics::getDevice();

        texture_.attach(0);
        state2D_.apply();

        device.drawUP(Primitive_TriangleList, numChars_*2, reinterpret_cast<const void*>(vertices_), sizeof(Vertex));
    }

    void ASCIIRender::swap(ASCIIRender& rhs)
    {
        std::swap(width_, rhs.width_);
        std::swap(height_, rhs.height_);
        std::swap(maxChars_, rhs.maxChars_);
        std::swap(numChars_, rhs.numChars_);
        std::swap(texture_, rhs.texture_);
        std::swap(vertices_, rhs.vertices_);
        std::swap(entries_, rhs.entries_);

        buffer_.swap(rhs.buffer_);
    }

    inline void ASCIIRender::addChar(f32 x, f32 y, f32 z, lmath::Color color, s8 c)
    {
        LASSERT(CharBegin<=c && c<=CharEnd);
        if(maxChars_<=numChars_){
            return;
        }

        u32 index = c-CharBegin;
        u32 v = numChars_*6;

        //---------------------------------------------
        vertices_[v].position_.set(x, y, z);
        vertices_[v].uv_.set(entries_[index].u_[0], entries_[index].v_[0]);
        vertices_[v].color_ = color;

        vertices_[v + 1].position_.set(x + width_, y, z);
        vertices_[v + 1].uv_.set(entries_[index].u_[1], entries_[index].v_[0]);
        vertices_[v + 1].color_ = color;

        vertices_[v + 2].position_.set(x, y + height_, z);
        vertices_[v + 2].uv_.set(entries_[index].u_[0], entries_[index].v_[1]);
        vertices_[v + 2].color_ = color;

        //---------------------------------------------
        vertices_[v + 3].position_.set(x + width_, y, z);
        vertices_[v + 3].uv_.set(entries_[index].u_[1], entries_[index].v_[0]);
        vertices_[v + 3].color_ = color;

        vertices_[v + 4].position_.set(x + width_, y + height_, z);
        vertices_[v + 4].uv_.set(entries_[index].u_[1], entries_[index].v_[1]);
        vertices_[v + 4].color_ = color;

        vertices_[v + 5].position_.set(x, y + height_, z);
        vertices_[v + 5].uv_.set(entries_[index].u_[0], entries_[index].v_[1]);
        vertices_[v + 5].color_ = color;

        ++numChars_;
    }
}
