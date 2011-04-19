#ifndef INC_XLOADER_H__
#define INC_XLOADER_H__
/**
@file XLoader.h
@author t-sakai
@date 2011/01/20 create

*/
#include <lcore/liostream.h>
#include <lgraphics/api/GeometryBuffer.h>
#include "XLoaderDef.h"

namespace lscene
{
    class Object;
}

namespace lgraphics
{
    class Material;
}

namespace lx
{
#define LX_XTOKEN_CHECK_RETURN(token, expected, code, newCode) \
    if((expected) != (token)){ (code) = (newCode); return;}

#define LX_XTOKEN_CHECK_RETURN_VALUE(token, expected, code, newCode, value) \
    if((expected) != (token)){ (code) = (newCode); return value;}

#define LX_CODE_CHECK_RETURN(expected, code) \
    if((expected) != (code)){ return; }

#define LX_CODE_CHECK_RETURN_VALUE(expected, code, value) \
    if((expected) != (code)){ return value; }

#define LX_RETURN(code, errorCode) \
    (code) = (errorCode); return

#define LX_RETURN_VALUE(code, errorCode, value) \
    (code) = (errorCode); return (value)

    //----------------------------------------------------------------------
    //---
    //--- XLoader
    //---
    //----------------------------------------------------------------------
    /// 
    class XLoader
    {
    public:
        enum Error
        {
            Error_None =0,
            Error_FileOpen,
            Error_Format,
            Error_ParsingMaterial,
            Error_ParsingMesh,
            Error_ParsingMeshFace,
            Error_ParsingMeshMaterialList,
            Error_ParsingMeshNormals,
            Error_ParsingMeshTextureCoords,
            Error_ParsingMeshVertexColors,
            Error_ParsingTextureFilename,
            Error_ParsingVertexDuplicationIndices,

            Error_Array,
            Error_ParsingVector,
            Error_ParsingMatrix44,

            Error_ParsingULONG,
            Error_ParsingFloat,
            Error_ParsingUUID,
            Error_ParsingTemplateHeader,
            Error_Num,
        };

        enum VertexDeclType
        {
            VertexDecl_P =0, //頂点だけ
            VertexDecl_PN,   //法線
            VertexDecl_PT,   //テクスチャ座標
            VertexDecl_PNT,  //法線＋テクスチャ座標
            VertexDecl_Num,
        };

        XLoader();
        ~XLoader();

        bool open(const Char* filepath);
        void close();
        void load(lscene::Object& obj, const Char* directory, f32 scale, bool swapOrigin);
        Error getErrorCode() const{ return errorCode_;}

        void release();
    private:

        struct ProcessFuncBase
        {
            virtual void operator()() =0;
        };

        template<class T>
        struct ProcessFuncVectorArray;

        template<class T>
        struct ProcessFunc : public ProcessFuncBase
        {
            virtual void operator()(){}
        };

        template<class T> friend struct ProcessFuncVectorArray;
        template<class T> friend struct ProcessFunc;

        struct ProcessVec3Func;
        struct ProcessUVFunc;
        struct ProcessNormalFunc;

        void processMain();
        void processTemplate();

        void processMaterial();
        void processMesh();
        void processMeshFace(FaceArray& faces);
        void processMeshMaterialList();
        void processMeshNormals();
        void processMeshTextureCoords();
        void processMeshVertexColors();
        void processTextureFilename();
        void processVertexDuplicationIndices();

        void processUUID();

        void processVector(f32* v, u32 count);
        void processMatrix44(f32* matrix);

        void processVectorMember(f32* v, u32 count);

        void processUnsupported();

        void processTemplateHeader();


        bool processArray(u32 count, ProcessFuncBase& func);

        u32 getULong();
        u32 getULongMember();

        f32 getFloat();
        f32 getFloatMember();

        s32 skipSpace();

        /// 次トークン取得
        void getNextToken();

        /// 文字列リテラル取得
        void getStringLiteral();

        /// １行読み飛ばす
        void skipLine();


        VertexDeclType getDeclType() const;
        void createMesh();
        inline void createVertexDecl(lgraphics::VertexDeclarationRef& decl, VertexDeclType type);
        inline void createIndexBuffer(lgraphics::IndexBufferRef& ib, lscene::Object& obj);

        void createObject(lscene::Object& obj, const Char* directory, bool swapOrigin);

        Error errorCode_;
        XToken currentToken_;
        Char* currentStr_;

        static const u32 DefaultBufferSize = 32;
        static const u32 DefaultIncSize = 32;

        lconverter::S8Vector buffer_;

        f32 scale_;
        lcore::ifstream file_;

        Vec3Vector vertices_; //頂点リスト
        NormalVector normals_; //法線リスト
        UVVector uvs_; //ＵＶリスト

        FaceArray faces_; //面リスト
        FaceArray normalFaces_; //法線用面リスト

        lconverter::S8Vector materialIndices_; //マテリアルインデックスリスト

        lconverter::NameTextureMap nameTexMap_;

        u32 currentMaterial_;

        const char* directory_;
        lscene::Object* animObj_;

//デバッグ用ログ
#if defined(LIME_LIBCONVERTER_DEBUGLOG_ENABLE)
    public:
        lconverter::DebugLog debugLog_;
#endif

#if defined(_DEBUG)
        void debugOut(const char* file);
#else
        void debugOut(const char*){}
#endif
    };


    //----------------------------------------------------------------------
    //---
    //--- XLoader::Frame
    //---
    //----------------------------------------------------------------------
    template<class T>
    struct XLoader::ProcessFuncVectorArray : public XLoader::ProcessFuncBase
    {
        ProcessFuncVectorArray(XLoader* parent, T* v, u32 dim)
            :parent_(parent)
            ,v_(v)
            ,dim_(dim)
        {
            LASSERT(parent_ != NULL);
            LASSERT(v_ != NULL);
        }

        virtual void operator()()
        {
            // float; float; float;
            for(u32 i=0; i<dim_; ++i){
                parent_->getNextToken();
                LX_XTOKEN_CHECK_RETURN(
                    parent_->currentToken_,
                    XToken_Other,
                    parent_->errorCode_,
                    XLoader::Error_ParsingVector);
                (*v_)[i] = static_cast<f32>( atof(parent_->currentStr_) );

                parent_->getNextToken();
                LX_XTOKEN_CHECK_RETURN(
                    parent_->currentToken_,
                    XToken_Semicolon,
                    parent_->errorCode_,
                    XLoader::Error_ParsingVector);
            }
            ++v_; //次へ進めておく
        }

        XLoader* parent_;
        T* v_;
        u32 dim_;
    };
}
#endif //INC_XLOADER_H__
