#ifndef INC_LFONT_H__
#define INC_LFONT_H__
/**
@file lfont.h
@author t-sakai
@date 2009/05/13 create
*/
//#include <list>
//#include <lmath/lmath.h>
#include <lcore/lcore.h>

//--- internal objects
struct FT_FaceRec_;
struct FT_GlyphSlotRec_;

namespace lfont
{
    using lcore::s8;
    using lcore::s16;
    using lcore::s32;
    using lcore::u8;
    using lcore::u16;
    using lcore::u32;
    using lcore::f32;
    using lcore::f64;

    enum FaceFlag
    {
        FaceFlag_Scalable   = (0x01U << 0), /// アウトライン
        FaceFlag_FixedSize  = (0x01U << 1), /// ビットマップ
        FaceFlag_FixedWidth = (0x01U << 2), /// 
        FaceFlag_SFNT       = (0x01U << 3), /// TrueTypeかOpenTypeの場合
        FaceFlag_Horizontal = (0x01U << 4), /// 水平メトリクス。多くのフォントが持つ
        FaceFlag_Vertical   = (0x01U << 5), /// 垂直メトリクス。一部のフォントが持つ
        FaceFlag_Kerning    = (0x01U << 6), /// カーニング
        FaceFlag_FastGlyphs = (0x01U << 7), /// 使わない
        FaceFlag_MutipleMasters = (0x01U << 8), /// マスタが複数ある。マスタ間を補間することができる
        FaceFlag_GlyphNames = (0x01U << 9), /// グリフ名がある
        FaceFlag_ExternalStream = (0x01U << 10), /// 使わない
        FaceFlag_Hinter     = (0x01U << 11), /// ヒンティング
        FaceFlag_CIDKeyed   = (0x01U << 12), /// 
        FaceFlag_Tricky     = (0x01U << 13), /// 
    };

    enum StyleFlag
    {
        Style_Italic = (0x01U << 0),
        Style_Bold = (0x01U << 1),
    };

    const char* getErrorMessage(s32 error_no);

    struct BitmapSize
    {
        BitmapSize()
        {
        }

        BitmapSize(
            s16 height,
            s16 width,
            s32 size,
            s32 xppem,
            s32 yppem)
            :height_(height)
            ,width_(width)
            ,size_(size)
            ,x_ppem_(xppem)
            ,y_ppem_(yppem)
        {
        }

        s16 height_;
        s16 width_;
        s32 size_;
        s32 x_ppem_;
        s32 y_ppem_;
    };

    struct BBox
    {
        s32 xMin_, yMin_;
        s32 xMax_, yMax_;
    };
    //-----------------------------------------------------
    //---
    //--- FontSystem
    //---
    //-----------------------------------------------------
    class FontSystem
    {
    public:
        static bool initialize();
        static void terminate();

    private:
         FontSystem();
         FontSystem(const FontSystem&);
        ~FontSystem();
        FontSystem& operator=(const FontSystem&);
    };

    //-----------------------------------------------------
    //---
    //--- Glyph
    //---
    //-----------------------------------------------------
    class Glyph
    {
    public:
        s32 getWidth() const;
        s32 getHeight() const;
        s32 getPitch() const;

        s32 getLeft() const;
        s32 getTop() const;
        
        s32 getAdvanceX() const;
        s32 getAdvanceY() const;

        u8 operator()(s32 x, s32 y);
        u8 operator()(s32 x, s32 y) const;
    private:
        friend class FontFace;
        Glyph();
        Glyph(FT_GlyphSlotRec_* glyph);
        ~Glyph();

        Glyph& operator=(const Glyph&);

        FT_GlyphSlotRec_ *impl_;
    };

    //-----------------------------------------------------
    //---
    //--- FontFace
    //---
    //-----------------------------------------------------
    class FontFace
    {
    public:
        ~FontFace();

        s32 getNumFaces() const;
        s32 getFaceIndex() const;

        FaceFlag getFaceFlag() const;
        StyleFlag getStyleFlag() const;

        s32 getNumGlyphs() const;

        const char* getFamilyName() const;
        const char* getStyleName() const;

        s32 getNumFixedSize() const;
        BitmapSize getAvailableSize(s32 index) const;

#if 0
        //s32 getNumCharMaps() const;
        //const CharMap& getCharMap(s32 index) const;


        //----------------------------------------
        const BBox& getBBox() const;
        u16 getUnitsPerEm() const;
        s16 getAscender() const;
        s16 getDescender() const;
        s16 getHeight() const;

        s16 getMaxAdvanceWidth() const;
        s16 getMaxAdvanceHeight() const;

        s16 getUnderlinePosition() const;
        s16 getUnderlineThickness() const;


        //----------------------------------------
#endif
        bool setCharSizeInPixel(u32 w, u32 h);
        /**
        @param w ... 26.6固定少数
        @param h ... 26.6固定少数
        @param hres ... 水平解像度(dpi)
        @param vres ... 垂直解像度(dpi)
        */
        bool setCharSize(s32 w, s32 h, u32 hres, u32 vres);
        bool loadChar(u32 char_code, bool mono=false);

        const Glyph& getGlyph() const{ return glyph_;}
    private:
        friend class Font;
        FontFace(FT_FaceRec_* face);

        FT_FaceRec_* face_;
        Glyph glyph_;
    };

    //-----------------------------------------------------
    //---
    //--- Font
    //---
    //-----------------------------------------------------
    class Font
    {
    public:
        /**
        @brief フォントファイルロード
        @param filepath ... フォントファイルパス
        @param faceIndex ... ロードしたいフェイスインデックス。大きすぎるとエラー。0は常に有効
        */
        static FontFace* load(const char* filepath, s32 faceIndex, s32& error_no);

        /**
        @brief メモリからフォントロード
        @param filepath ... フォントファイルパス
        @param faceIndex ... ロードしたいフェイスインデックス。大きすぎるとエラー。0は常に有効
        */
        static FontFace* load(const u8* bytes, s32 size, s32 faceIndex, s32& error_no);
    };

#if 0
    //-----------------------------------------------------
    //---
    //--- FontRenderer
    //---
    //-----------------------------------------------------
    class FontRenderer
    {
        FontRenderer(u32 maxChars);
    public:
        ~FontRenderer();

        static FontRenderer* create(u32 maxChars, u32 screenWidth, u32 screenHeight, Font* font);

        void draw();
        void clear();

        void print(u32 row, u32 column, const s8* text, u32 color=0xFFFFFFFFU);

    private:
        void putChar(u32 row, u32 column, u32 color, u8 c);

        static void setState(lgraphics::Texture* texture, const lmath::Matrix44& projection);

        u32 maxNum_;
        Font *font_;
        lgraphics::PrimitiveRender *primitiveRender_;
        lgraphics::RenderState *state_;

        u32 currentNum_;

        f32 uvw_, uvh_;
        lmath::Vector2 uvoffset_; //半テクセルずらし
        u32 halfWidth_, halfHeight_;
    };
#endif
}

#endif //INC_LFONT_H__
