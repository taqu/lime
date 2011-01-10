/**
@file lfont.cpp
@author t-sakai
@date 2009/05/13
*/
#include <lcore/lcore.h>

#include "lfont.h"
#include <ft2build.h>
#include FT_FREETYPE_H 
#include FT_GLYPH_H
#include FT_ERRORS_H

namespace lfont
{
    namespace
    {
#if defined(_DEBUG)
#undef __FTERRORS_H__
#define FT_ERRORDEF( e, v, s )  { e, s },
#define FT_ERROR_START_LIST     {
#define FT_ERROR_END_LIST       { 0, 0 } };
        struct FtError
        {
            int          err_code;
            const char*  err_msg;
        };
        static const FtError ft_errors[] =
#include FT_ERRORS_H      
#endif
    }

    const char* getErrorMessage(s32 error_no)
    {
#if defined(_DEBUG)
        LASSERT(0<=error_no && error_no < sizeof(ft_errors)/sizeof(FtError));
        return ft_errors[error_no].err_msg;
#else
        static const char* NULL_STR = "";
        return NULL_STR;
#endif
    }


    //-----------------------------------------------------
    //---
    //--- FreeTypeLib
    //---
    //-----------------------------------------------------
    /// FreeType2 ライブラリ
    class FreeTypeLib
    {

    public:
        static bool initialize();
        static void terminate();

        static FT_Library getInstance()
        {
            LASSERT(library_ != NULL);
            return library_;
        }
    private:
        FreeTypeLib();
        ~FreeTypeLib();
        FreeTypeLib& operator=(const FreeTypeLib&);

        static FT_Library library_;
    };

    FT_Library FreeTypeLib::library_ = NULL;

    bool FreeTypeLib::initialize()
    {
        LASSERT(library_ == NULL);
        FT_Error error = FT_Init_FreeType(&library_);
        return (error == 0);
    }

    void FreeTypeLib::terminate()
    {
        LASSERT(library_ != NULL);
        FT_Done_FreeType(library_);
        library_ = NULL;
    }


    //-----------------------------------------------------
    //---
    //--- Glyph
    //---
    //-----------------------------------------------------
    Glyph::Glyph()
        :impl_(NULL)
    {
    }

    Glyph::Glyph(FT_GlyphSlotRec_* glyph)
        :impl_(glyph)
    {
        LASSERT(impl_ != NULL);
    }

    Glyph::~Glyph()
    {
        impl_ = NULL;
    }

    s32 Glyph::getLeft() const
    {
        LASSERT(impl_ != NULL);
        return impl_->bitmap_left;
    }

    s32 Glyph::getTop() const
    {
        LASSERT(impl_ != NULL);
        return impl_->bitmap_top;
    }

    s32 Glyph::getWidth() const
    {
        LASSERT(impl_ != NULL);
        return impl_->bitmap.width;
    }

    s32 Glyph::getHeight() const
    {
        LASSERT(impl_ != NULL);
        return impl_->bitmap.rows;
    }

    s32 Glyph::getPitch() const
    {
        LASSERT(impl_ != NULL);
        return impl_->bitmap.pitch;
    }

    s32 Glyph::getAdvanceX() const
    {
        LASSERT(impl_ != NULL);
        return (impl_->advance.x >> 6); //26.6固定少数
    }

    s32 Glyph::getAdvanceY() const
    {
        LASSERT(impl_ != NULL);
        return (impl_->advance.y >> 6); //26.6固定少数
    }

    u8 Glyph::operator()(s32 x, s32 y)
    {
        LASSERT(0<=x && x<getWidth());
        LASSERT(0<=y && y<getHeight());

        static const s32 w = impl_->bitmap.pitch;
        return impl_->bitmap.buffer[y*w + x];
    }

    u8 Glyph::operator()(s32 x, s32 y) const
    {
        LASSERT(0<=x && x<getWidth());
        LASSERT(0<=y && y<getHeight());

        const s32 w = impl_->bitmap.pitch;
        return impl_->bitmap.buffer[y*w + x];
    }

    //-----------------------------------------------------
    //---
    //--- FontFace
    //---
    //-----------------------------------------------------
    FontFace::FontFace(FT_FaceRec_* face)
        :face_(face)
        ,glyph_(face->glyph)
    {
        LASSERT(face_ != NULL);
    }

    FontFace::~FontFace()
    {
        LASSERT(face_ != NULL);
        FT_Done_Face(face_);
        face_ = NULL;
    }

    s32 FontFace::getNumFaces() const
    {
        return face_->num_faces;
    }

    s32 FontFace::getFaceIndex() const
    {
        return face_->face_index;
    }

    FaceFlag FontFace::getFaceFlag() const
    {
        return static_cast<FaceFlag>(face_->face_flags);
    }

    StyleFlag FontFace::getStyleFlag() const
    {
        return static_cast<StyleFlag>(face_->style_flags);
    }

    s32 FontFace::getNumGlyphs() const
    {
        return face_->num_glyphs;
    }

    const char* FontFace::getFamilyName() const
    {
        return face_->family_name;
    }

    const char* FontFace::getStyleName() const
    {
        return face_->style_name;
    }

    s32 FontFace::getNumFixedSize() const
    {
        return face_->num_fixed_sizes;
    }

    BitmapSize FontFace::getAvailableSize(s32 index) const
    {
        LASSERT(0<=index && index<face_->num_fixed_sizes);
        const FT_Bitmap_Size &size = face_->available_sizes[index];

        return BitmapSize(size.height, size.width, size.size, size.x_ppem, size.y_ppem);
    }

    bool FontFace::setCharSizeInPixel(u32 w, u32 h)
    {
        FT_Error error = FT_Set_Pixel_Sizes(face_, w, h);
        return (error == 0);
    }

    bool FontFace::setCharSize(s32 w, s32 h, u32 hres, u32 vres)
    {
        FT_Error error = FT_Set_Char_Size(face_, w, h, hres, vres);
        return (error == 0);
    }

    bool FontFace::loadChar(u32 char_code, bool mono)
    {
        LASSERT(face_ != NULL);

        s32 flag = (mono)? FT_LOAD_RENDER | FT_LOAD_MONOCHROME : FT_LOAD_RENDER;
        FT_Error error = FT_Load_Char(face_, char_code, flag);

        return (error == 0);
    }
#if 0
    //-----------------------------------------------------
    //---
    //--- Utility Function
    //---
    //-----------------------------------------------------
    void draw_bitmap(
        Glyph& glyph,
        u8 cnum,
        u32 tw,
        u32 th,
        u32 texPixelSize,
        u32 bottomPadding,
        u32* image)
    {
        u32 pixelSize = texPixelSize - bottomPadding;
        u32 x_min = glyph.getLeft();
        u32 y_min = texPixelSize - glyph.getTop() - bottomPadding;
        u32 x_max = x_min + glyph.getWidth();
        u32 y_max = y_min + glyph.getHeight();

        // サイズをはみ出した分をずらす
        if(x_max > pixelSize){
            u32 diffx = x_max - pixelSize;
            x_min -= diffx;
            x_max -= diffx;
        }

        if(y_max > texPixelSize){
            u32 diffy = y_max - texPixelSize;
            y_min -= diffy;
            y_max -= diffy;
        }

        int cx = cnum / 16 - 2;
        int cy = cnum % 16;

        cx *= pixelSize;
        cy *= texPixelSize;

        for(u32 i=x_min, p = 0;i<x_max; ++i, ++p){
            for(u32 j=y_min, q = 0; j<y_max; ++j, ++q){
                u8 uc = glyph.get(p, q);
                u32 &pix = image[(cy+j)*tw + cx+i];
                pix = 0x00FFFFFFU;
                //pix |= (uc << 0);
                //pix |= (uc << 8);
                //pix |= (uc << 16);
                pix |= (uc << 24);
            }
        }
    }
#endif
    //-----------------------------------------------------
    //---
    //--- FontSystem
    //---
    //-----------------------------------------------------
    bool FontSystem::initialize()
    {
        return FreeTypeLib::initialize();
    }

    void FontSystem::terminate()
    {
        FreeTypeLib::terminate();
    }


    //-----------------------------------------------------
    //---
    //--- Font
    //---
    //-----------------------------------------------------
    // フォントファイルロード
    FontFace* Font::load(const char* filepath, s32 faceIndex, s32& error_no)
    {
        FT_Face face = NULL;
        error_no = FT_New_Face(
            FreeTypeLib::getInstance(),
            filepath,
            faceIndex,
            &face);

        if(error_no != FT_Err_Ok){
            return NULL;
        }

        return LIME_NEW FontFace(face);
    }

    // メモリからフォントロード
    FontFace* Font::load(const u8* bytes, s32 size, s32 faceIndex, s32& error_no)
    {
        FT_Face face = NULL;
        error_no = FT_New_Memory_Face(
            FreeTypeLib::getInstance(),
            bytes,
            size,
            faceIndex,
            &face);

        if(error_no != FT_Err_Ok){
            return NULL;
        }

        return LIME_NEW FontFace(face);
    }

#if 0
    Font::Font(u32 pixelWidth, u32 pixelHeight)
        :pixelWidth_(pixelWidth),
        pixelHeight_(pixelHeight)
    {
    }

    Font::~Font()
    {
    }

    Font* Font::create(const char* fontfile, u32 pixelSize)
    {
        if(fontfile == NULL){
            return NULL;
        }

        FreeTypeCore core;
        if(core.isInit() == false){
            return NULL;
        }

        FontFace face(core.get(), fontfile, 0);
        if(face.isInit() == false){
            return NULL;
        }

        face.setPixelSize(pixelSize, pixelSize);

        u32 bottomPadding = static_cast<u32>(pixelSize * 0.3f);
        u32 texPixelSize = pixelSize + bottomPadding;

        f64 lw2 = lcore::ceil( lcore::log2(pixelSize*(8-2)) );
        f64 lh2 = lcore::ceil( lcore::log2(texPixelSize*16) );
        u32 width = static_cast<u32>(lcore::pow(2.0, lw2) + 0.5);
        u32 height = static_cast<u32>(lcore::pow(2.0, lh2) + 0.5);

        lcore::ScopedPtr<Texture> texPtr( Texture::create(
            width,
            height,
            1,
            lgraphics::Usage_None,
            lgraphics::Buffer_A8R8G8B8,
            lgraphics::Pool_Managed,
            lgraphics::TexType_Diffuse));
        if(texPtr == false){
            return NULL;
        }

        Texture::LockedRect rect;
        bool isLock = texPtr->lock(0, rect);
        if(isLock == false){
            return NULL;
        }

        u32 *image = static_cast<u32*>(rect.bits_);
        for(u32 j=0; j<height; ++j){
            for(u32 i=0; i<width; ++i){
                image[j*width + i] = 0;
            }
        }

        u32 start = 33; //ascii code of space
        u32 end = 126; //ascii code of ~
        for(u32 c = start; c<=end; ++c){
            // set transformation
            //FT_Set_Transform( face, &matrix, &pen );

            // load glyph image into the slot (erase previous one)
            Glyph glyph;
            bool isLoad = face.loadChar(c, glyph);
            if(isLoad == false){
                continue;
            }

            draw_bitmap(glyph,
                c,
                width,
                height,
                texPixelSize,
                bottomPadding,
                image);
        }

        texPtr->unlock(0);

        Font *font = LIME_NEW Font(pixelSize, texPixelSize);
        if(font == NULL){
            return NULL;
        }

        font->texture_ = texPtr.release();

        return font;
    }


    void Font::debugoutTexture(const char* filename)
    {
        texture_->debugout(filename);
    }


    //-----------------------------------------------------
    //---
    //--- FontRenderer
    //---
    //-----------------------------------------------------
    FontRenderer::FontRenderer(u32 maxChars)
        :maxNum_(maxChars),
        primitiveRender_(NULL),
        state_(NULL),
        currentNum_(0),
        uvw_(0.0f),
        uvh_(0.0f),
        halfWidth_(0),
        halfHeight_(0)
    {
    }

    FontRenderer::~FontRenderer()
    {
        LIME_DELETE(primitiveRender_);
        LIME_DELETE(state_);
    }

    FontRenderer* FontRenderer::create(u32 maxChars, u32 screenWidth, u32 screenHeight, Font* font)
    {
        if(font == NULL){
            LASSERT(false);
            return NULL;
        }

        lcore::ScopedPtr<lgraphics::PrimitiveRender> primPtr( lgraphics::PrimitiveRender::create(maxChars*2, NULL) );
        
        lgraphics::RenderState::begin();
        lmath::Matrix44 projection;
        projection.ortho(
            static_cast<f32>(screenWidth),
            static_cast<f32>(screenHeight),
            0.1f,
            1.0f);

        FontRenderer::setState(font->texture_, projection);

        lgraphics::RenderState state;
        lgraphics::RenderState::end(state);

        if(!primPtr){
            return NULL;
        }

        FontRenderer *render = LIME_NEW FontRenderer(maxChars);
        if(render == NULL){
            return NULL;
        }

        render->font_ = font;
        render->primitiveRender_ = primPtr.release();
        render->state_ = LIME_NEW lgraphics::RenderState;
        render->state_->swap(state);

        render->uvw_ = static_cast<f32>(font->getPixelWidth()) / font->texture_->getWidth();
        render->uvh_ = static_cast<f32>(font->getPixelHeight()) / font->texture_->getHeight();

        render->halfWidth_ = screenWidth >> 1;
        render->halfHeight_ = screenHeight >> 1;

        render->uvoffset_.set(0.5f / font->texture_->getWidth(), 0.5f / font->texture_->getHeight());

        return render;
    }

    void FontRenderer::draw()
    {
        primitiveRender_->unlock();
        state_->apply();
        primitiveRender_->drawNoStateChange();
    }

    void FontRenderer::clear()
    {
        currentNum_ = 0;
        primitiveRender_->lock();
    }

    void FontRenderer::print(u32 row, u32 column, const s8* text, u32 color)
    {
        LASSERT(text != NULL);

        while(*text != '\0'){
            if(maxNum_ <= currentNum_){
                return;
            }
            putChar(row, column, color, (u8)*text);
            ++column;
            ++text;
        }
    }

    void FontRenderer::putChar(u32 row, u32 column, u32 color, u8 c)
    {
        u32 pixelWidth = font_->getPixelWidth();
        u32 pixelHeight = font_->getPixelHeight();
        u32 cx = c / 16 - 2;
        u32 cy = c % 16;

        f32 pixx = static_cast<f32>( column * pixelWidth) - halfWidth_;
        f32 pixy = halfHeight_ - static_cast<f32>( row * pixelHeight);
        f32 uvx = static_cast<f32>(cx) * uvw_;// - uvoffset_._x; //半テクセルずらし
        f32 uvy = static_cast<f32>(cy) * uvh_;// + uvoffset_._y; //半テクセルずらし

        lgraphics::PrimitiveVertex buff[6] =
        {
            PrimitiveVertex(lmath::Vector3(pixx, pixy, 0.1f), color, lmath::Vector2(uvx, uvy)),
            PrimitiveVertex(lmath::Vector3(pixx+pixelWidth, pixy, 0.1f), color, lmath::Vector2(uvx+uvw_, uvy)),
            PrimitiveVertex(lmath::Vector3(pixx, pixy-pixelHeight, 0.1f), color, lmath::Vector2(uvx, uvy+uvh_)),

            PrimitiveVertex(lmath::Vector3(pixx, pixy-pixelHeight, 0.1f), color, lmath::Vector2(uvx, uvy+uvh_)),
            PrimitiveVertex(lmath::Vector3(pixx+pixelWidth, pixy, 0.1f), color, lmath::Vector2(uvx+uvw_, uvy)),
            PrimitiveVertex(lmath::Vector3(pixx+pixelWidth, pixy-pixelHeight, 0.1f), color, lmath::Vector2(uvx+uvw_, uvy+uvh_)),
        };
        //lgraphics::PrimitiveVertex buff[6] =
        //{
        //    {lmath::Vector3(-(f32)halfWidth_, halfHeight_, 0.1f), color, lmath::Vector2(0.0f, 0.0f)},
        //    {lmath::Vector3(-(f32)halfWidth_, -(f32)halfHeight_, 0.1f), color, lmath::Vector2(0.0f, 1.0f)},
        //    {lmath::Vector3(halfWidth_, halfHeight_, 0.1f), color, lmath::Vector2(1.0f, 0.0f)},

        //    {lmath::Vector3(halfWidth_, halfHeight_, 0.1f), color, lmath::Vector2(1.0f, 0.0f)},
        //    {lmath::Vector3(-(f32)halfWidth_, -(f32)halfHeight_, 0.1f), color, lmath::Vector2(0.0f, 1.0f)},
        //    {lmath::Vector3(halfWidth_, -(f32)halfHeight_, 0.1f), color, lmath::Vector2(1.0f, 1.0f)},
        //};

        primitiveRender_->add(buff[0], buff[1], buff[2]);
        primitiveRender_->add(buff[3], buff[4], buff[5]);

        ++currentNum_;
    }


    void FontRenderer::setState(lgraphics::Texture* texture, const lmath::Matrix44& projection)
    {
        LASSERT(texture != NULL);

        RenderState::setZEnable(false);
        RenderState::setZWriteEnable(false);
        RenderState::setAlphaTest(false);

        RenderState::setAlphaBlendEnable(true);

        RenderState::setAlphaBlendSrc(Blend_SrcAlpha);
        RenderState::setAlphaBlendDst(Blend_InvSrcAlpha);

        //Disable Lighting
        RenderState::setLighting(false);

        RenderState::setCullMode(lgraphics::CullMode_CCW);

        IDirect3DDevice9 *d3ddevice = Graphics::getInstance().getDevice().getD3DDevice();

        d3ddevice->SetVertexShader(NULL);
        d3ddevice->SetPixelShader(NULL);
        d3ddevice->SetFVF(lgraphics::PRIMITIVE_FVF);

        d3ddevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        d3ddevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        d3ddevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

        d3ddevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        d3ddevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        d3ddevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

        d3ddevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
        d3ddevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);

        texture->attach(0);

        lmath::Matrix44 mat;
        mat.identity();
        d3ddevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&mat);
        d3ddevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&projection);
        d3ddevice->SetTransform(D3DTS_WORLDMATRIX(0), (D3DMATRIX*)&mat);
    }
#endif

}
