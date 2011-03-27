#ifndef INC_XLOADERDEF_H__
#define INC_XLOADERDEF_H__
/**
@file XLoaderDef.h
@author t-sakai
@date 2011/01/20 create

*/
#include <lcore/lcore.h>
#include <lmath/Vector3.h>
#include "converter.h"

namespace lx
{
#define LX_NEW LIME_NEW
#define LX_PLACEMENT_NEW(ptr) LIME_PLACEMENT_NEW(ptr)
#define LX_DELETE(ptr) LIME_DELETE(ptr)
#define LX_DELETE_ARRAY(ptr) LIME_DELETE_ARRAY(ptr)

    using lcore::s8;
    using lcore::s16;
    using lcore::s32;

    using lcore::u8;
    using lcore::u16;
    using lcore::u32;

    using lcore::f32;

    using lcore::Char;

    struct Normal
    {
        s16 elem_[4];
    };

    struct UV
    {
        s16 elem_[2];
    };

    // Xファイルヘッダ情報
    //--------------------------------------------------------------
    /// Xファイルマジックナンバー
#define LIME_X_MAGIC \
  ((u32)'x' + ((u32)'o' << 8) + ((u32)'f' << 16) + ((u32)' ' << 24))

    /// バージョン
#define LIME_X_VERSION3032 \
  ((u32)'0' + ((u32)'3' << 8) + ((u32)'0' << 16) + ((u32)'2' << 24))

    /// バイナリフォーマット
#define LIME_X_BINARY \
  ((u32)'b' + ((u32)'i' << 8) + ((u32)'n' << 16) + ((u32)' ' << 24))

    /// テキストフォーマット
#define LIME_X_TEXT   \
  ((u32)'t' + ((u32)'x' << 8) + ((u32)'t' << 16) + ((u32)' ' << 24))

    /// 浮動小数点のサイズ。32ビット
#define LIME_X_FLOAT_BITS_32 \
  ((u32)'0' + ((u32)'0' << 8) + ((u32)'3' << 16) + ((u32)'2' << 24))

    /// 浮動小数点のサイズ。64ビット
#define LIME_X_FLOAT_BITS_64 \
  ((u32)'0' + ((u32)'0' << 8) + ((u32)'6' << 16) + ((u32)'4' << 24))


    // XファイルGUI
    //-------------------------------------------------------------------
    /// GUID
    struct GUID
    {
        u32 data1_;
        u16 data2_;
        u16 data3_;
        u8 data4_[8];
    };
#if defined(LIME_DEFINE_GUID)
#undef LIME_DEFINE_GUID
#endif

#if defined(LIME_GUID_IMPL)
    LIME_DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)\
        const GUID name = {l, w1, w2, b1, {b2, b3, b4, b5, b6, b7, b8}}

#else
#define LIME_DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)\
    extern const GUID name
#endif

    /// GUIDの比較
    inline bool isEqualGUID(const GUID& guid0, const GUID& guid1)
    {
        return (0 == memcmp(&guid0, &guid1, sizeof(GUID)));
    }


    // Xファイルトークン
    //----------------------------------------------------------------------
    enum XToken
    {
        XToken_Name = 0,
        XToken_String,
        XToken_Integer,
        XToken_GUID,
        XToken_IntegerList,
        XToken_FloatList,

        // stand-alone tokes
        XToken_OBrace,
        XToken_CBrace,
        XToken_OParen,
        XToken_CParen,
        XToken_OBracket,
        XToken_CBracket,
        XToken_OAngle,
        XToken_CAngle,
        XToken_Dot,
        XToken_Comma,
        XToken_Semicolon,

        XToken_Template,
        XToken_WORD,
        XToken_DWORD,
        XToken_FLOAT,
        XToken_DOUBLE,
        XToken_CHAR,
        XToken_UCHAR,
        XToken_SWORD,
        XToken_SDWORD,
        XToken_VOID,
        XToken_LPSTR,
        XToken_Unicode,
        XToken_CString,
        XToken_Array,
        XToken_Binary,
        XToken_BinaryResource,

        XToken_Num,

        XToken_None,
        XToken_Other = 0xFFU,
    };

    XToken getTokenType(const Char* str);


    // template
    //----------------------------------------------------------------------
    // template names
    enum Template
    {
        Template_Boolean = 0,
        Template_Boolean2d,
        Template_ColorRGB,
        Template_ColorRGBA,
        Template_Coords2d,
        Template_Frame,
        Template_FrameTransformMatrix,
        Template_Material,
        Template_Matrix4x4,
        Template_Mesh,
        Template_MeshFace,
        Template_MeshMaterialList,
        Template_MeshNormals,
        Template_MeshTextureCoords,
        Template_MeshVertexColors,
        Template_TextureFilename,
        Template_Vector,
        Template_VertexDuplicationIndices,

        Template_Num,
        Template_Unsupported,
    };

    extern const Char* TemplateNames[Template_Num];

    /// テンプレートタイプ取得
    Template getTemplate(const Char* name);


    //
    typedef lcore::vector_arena<lmath::Vector3> Vec3Vector;
    typedef lcore::vector_arena<Normal> NormalVector;

    static const u32 MaxVertices = (0xFFFFU);
    //static const u32 MaxIndices = (0x0FFFFFFFU + 1);
    static const u32 MaxIndices = (0xFFFFFFFFU);
    static const u32 MaxPolygonsInFace = (0xFU + 1);

    inline void getIndex(u32 face, u32& numPolygons, u32& index)
    {
        numPolygons = face >> 24;
        index = (face & 0x0FFFFFFFU);
    }

    inline void setIndex(u32 numPolygons, u32 index, u32& face)
    {
        face = (numPolygons<<24 | (index & 0x0FFFFFFFU));
    }

    //
    class FaceArray
    {
    public:
        struct Face
        {
            explicit Face(u16* indices=NULL)
                :indices_(indices)
            {}

            u16 operator[](u32 index) const{ return indices_[index];}

            u16* indices_;
        };

        FaceArray();
        ~FaceArray();

        void create(u32 numFaces);
        Face add(u16 numIndices);

        u32 getNumFaces() const{ return numFaces_;}
        u32 getNumIndices(u32 faceIndex) const{ return indices_[ toFaceTop_[faceIndex] ];}
        u32 getTotalIndices() const{ return numIndices_;}

        Face getFace(u32 faceIndex){ return Face( &(indices_[toFaceTop_[faceIndex] + 1]) );}
    private:
        void release();

        u32 numFaces_;
        u32 numIndices_;
        u32* toFaceTop_;
        lconverter::U16Vector indices_;
    };
}
#endif //INC_XLOADERDEF_H__
