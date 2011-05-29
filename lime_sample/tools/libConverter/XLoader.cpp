/**
@file XLoader.cpp
@author t-sakai
@date 2011/01/20 create

*/

#include "XLoader.h"
#include <lcore/liostream.h>
#include <lmath/Matrix44.h>

#include <lgraphics/api/RenderStateRef.h>
#include <lgraphics/api/TextureRef.h>

#include <lframework/scene/Geometry.h>
#include <lframework/scene/Material.h>
#include <lframework/scene/Object.h>
#include <lframework/scene/shader/DefaultShader.h>

#include "converter.h"

using namespace lmath;
using namespace lconverter;
using namespace lgraphics;

namespace lx
{
namespace
{
#if 0
#define LX_DEBUG_OUT(str) std::cout << str << std::endl
#else
#define LX_DEBUG_OUT(str)
#endif


    u32 atoul(const Char* str)
    {
        LASSERT(str != NULL);
        while(*str == '+' || *str == '-'){
            ++str;
        }

        u32 ret = 0;
        while(*str != '\0'){
            if(!isdigit(*str)){
                break;
            }
            ret *= 10;
            ret += (*str - '0');
            ++str;
        }
        return ret;
    }

    // ユーティリティ
    template<class T>
    inline void read(lcore::istream& is, T& t)
    {
        is.read(reinterpret_cast<Char*>(&t), sizeof(T));
    }

    template<class T>
    inline void read(lcore::istream& is, T* t, u32 size)
    {
        is.read(reinterpret_cast<Char*>(t), size);
    }

    inline bool isSkip(int c)
    {
        return (isspace(c) || iscntrl(c));
    }

    XToken isBreak(int c)
    {
        switch(c)
        {
        case '{': return XToken_OBrace;
        case '}': return XToken_CBrace;
        case '(': return XToken_OParen;
        case ')': return XToken_CParen;
        case '[': return XToken_OBracket;
        case ']': return XToken_CBracket;
        case '<': return XToken_OAngle;
        case '>': return XToken_CAngle;
        case ',': return XToken_Comma;
        case ';': return XToken_Semicolon;
        default:
            break;
        };
        return XToken_Other;
    }

    struct VertexPN
    {
        lmath::Vector3 pos_;
        Normal normal_;
    };

    struct VertexPT
    {
        lmath::Vector3 pos_;
        UV uv_;
    };

    struct VertexPNT
    {
        lmath::Vector3 pos_;
        Normal normal_;
        UV uv_;
    };


    void createRenderState(RenderStateRef& state, bool alphaBlend)
    {
        //ステート設定はてきとう
        state.setZEnable(true);

        // アルファ設定
        //alphaBlend = false;
        if(alphaBlend){
            state.setCullMode(lgraphics::CullMode_None);

            state.setAlphaBlendEnable(true);
            state.setAlphaBlend(lgraphics::Blend_SrcAlpha, lgraphics::Blend_InvSrcAlpha);
            
            // Zバッファ設定
            state.setZWriteEnable(false);
            state.setAlphaTest(false);
            state.setAlphaTestFunc(lgraphics::Cmp_Greater);
        }else{
            state.setCullMode(lgraphics::CullMode_CCW);

            state.setAlphaBlendEnable(false);

            // Zバッファ設定
            state.setZWriteEnable(true);
            state.setAlphaTest(false);
            state.setAlphaTestFunc(lgraphics::Cmp_Greater);
        }
    }
}


    //----------------------------------------------------------------------
    //---
    //--- XLoader
    //---
    //----------------------------------------------------------------------
    /**
    @brief Vector3用ロード関数オブジェクト
    */
    struct XLoader::ProcessVec3Func : public XLoader::ProcessFuncBase
    {
        /**
        @param parent ... 親ローダ
        @param v ... ロード先配列。個数のチェックはしないので十分なサイズがあること
        */
        ProcessVec3Func(XLoader* parent, Vec3Vector* v)
            :parent_(parent)
            ,v_(v)
        {
            LASSERT(parent_ != NULL);
            LASSERT(v_ != NULL);
        }

        virtual void operator()()
        {
            Vector3 v;
            for(u32 i=0; i<3; ++i){
                parent_->getNextToken();
                LX_XTOKEN_CHECK_RETURN(
                    parent_->currentToken_,
                    XToken_Other,
                    parent_->errorCode_,
                    XLoader::Error_ParsingVector);
                v[i] = static_cast<f32>( atof(parent_->currentStr_) );

                parent_->getNextToken();
                LX_XTOKEN_CHECK_RETURN(
                    parent_->currentToken_,
                    XToken_Semicolon,
                    parent_->errorCode_,
                    XLoader::Error_ParsingVector);
            }
            v_->push_back(v);
        }

        XLoader* parent_;
        Vec3Vector* v_;
    };

    /**
    @brief UV用ロード関数オブジェクト
    */
    struct XLoader::ProcessUVFunc : public XLoader::ProcessFuncBase
    {
        /**
        @param parent ... 親ローダ
        @param v ... ロード先配列。個数のチェックはしないので十分なサイズがあること
        */
        ProcessUVFunc(XLoader* parent, UVVector* v)
            :parent_(parent)
            ,v_(v)
        {
            LASSERT(parent_ != NULL);
            LASSERT(v_ != NULL);
        }

        virtual void operator()()
        {
            f32 tmp;
            for(u32 i=0; i<2; ++i){
                parent_->getNextToken();
                if(XToken_Comma == parent_->currentToken_){ //カンマならもうひとつ
                    parent_->getNextToken();
                }

                LX_XTOKEN_CHECK_RETURN(
                    parent_->currentToken_,
                    XToken_Other,
                    parent_->errorCode_,
                    XLoader::Error_ParsingVector);
                tmp = static_cast<f32>( atof(parent_->currentStr_) );

#if defined(LX_UV_TYPE_PACKED_S16)
                tmp = texAddress(tmp, lconverter::TexAddress_Wrap);

                v_->push_back( F32ToS16Clamp( tmp ) );
#else
                v_->push_back( tmp );
#endif

                parent_->getNextToken();
                LX_XTOKEN_CHECK_RETURN(
                    parent_->currentToken_,
                    XToken_Semicolon,
                    parent_->errorCode_,
                    XLoader::Error_ParsingVector);
            }
        }

        XLoader* parent_;
        UVVector* v_;
    };

    /**
    @brief Normal用ロード関数オブジェクト
    */
    struct XLoader::ProcessNormalFunc : public XLoader::ProcessFuncBase
    {
        /**
        @param parent ... 親ローダ
        @param v ... ロード先配列。個数のチェックはしないので十分なサイズがあること
        */
        ProcessNormalFunc(XLoader* parent, NormalVector* v)
            :parent_(parent)
            ,v_(v)
        {
            LASSERT(parent_ != NULL);
            LASSERT(v_ != NULL);
        }

        virtual void operator()()
        {
            Normal normal;
            for(u32 i=0; i<3; ++i){
                parent_->getNextToken();
                LX_XTOKEN_CHECK_RETURN(
                    parent_->currentToken_,
                    XToken_Other,
                    parent_->errorCode_,
                    XLoader::Error_ParsingVector);
                normal.elem_[i] = F32ToS16( static_cast<f32>( atof(parent_->currentStr_) ) );

                parent_->getNextToken();
                LX_XTOKEN_CHECK_RETURN(
                    parent_->currentToken_,
                    XToken_Semicolon,
                    parent_->errorCode_,
                    XLoader::Error_ParsingVector);
            }
            v_->push_back(normal);
        }

        XLoader* parent_;
        NormalVector* v_;
    };

    /**
    @brief f32用ロード関数オブジェクト
    */
    template<>
    struct XLoader::ProcessFunc<f32> : public ProcessFuncBase
    {
        /**
        @param parent ... 親ローダ
        @param v ... ロード先配列。個数のチェックはしないので十分なサイズがあること
        */
        ProcessFunc(XLoader* parent, f32* v)
            :parent_(parent)
            ,v_(v)
        {
            LASSERT(parent_ != NULL);
            LASSERT(v_ != NULL);
        }

        virtual void operator()()
        {
            // float
            parent_->getNextToken();
            LX_XTOKEN_CHECK_RETURN(
                parent_->currentToken_,
                XToken_Other,
                parent_->errorCode_,
                XLoader::Error_Array);
            *v_ = static_cast<f32>( atof(parent_->currentStr_) );

            ++v_; //次へ進めておく
        }

        XLoader* parent_;
        f32* v_;
    };

    /**
    @brief u16用ロード関数オブジェクト
    */
    template<>
    struct XLoader::ProcessFunc<S8Vector> : public ProcessFuncBase
    {
        /**
        @param parent ... 親ローダ
        @param v ... ロード先配列。個数のチェックはしないので十分なサイズがあること
        */
        ProcessFunc(XLoader* parent, S8Vector* v)
            :parent_(parent)
            ,v_(v)
        {
            LASSERT(parent_ != NULL);
            LASSERT(v_ != NULL);
        }

        virtual void operator()()
        {
            parent_->getNextToken();
            LX_XTOKEN_CHECK_RETURN(
                parent_->currentToken_,
                XToken_Other,
                parent_->errorCode_,
                XLoader::Error_Array);

            v_->push_back( static_cast<s8>( atoul(parent_->currentStr_) ) );
        }

        XLoader* parent_;
        S8Vector* v_;
    };

    /**
    @brief インデックス配列用ロード関数オブジェクト
    */
    template<>
    struct XLoader::ProcessFunc<FaceArray> : public ProcessFuncBase
    {
        /**
        @param parent ... 親ローダ
        @param v ... ロード先配列
        */
        ProcessFunc(XLoader* parent, FaceArray* v)
            :parent_(parent)
            ,v_(v)
        {
            LASSERT(parent_ != NULL);
            LASSERT(v_ != NULL);
        }

        void operator()()
        {
            parent_->processMeshFace(*v_);
        }

        XLoader* parent_;
        FaceArray* v_;
    };


    bool XLoader::processArray(u32 count, ProcessFuncBase& func)
    {
        u32 i=0;
        for(; i<count; ++i){
            func();
            LX_CODE_CHECK_RETURN_VALUE(Error_None, errorCode_, false);

            getNextToken();
            if(XToken_Comma == currentToken_){
            }else if(XToken_Semicolon == currentToken_){
                break;
            }else{
                return false;
            }
        }

        return (i == (count-1));
    }

    //-----------------------------------------------------------
    XLoader::XLoader()
        :errorCode_(Error_None)
        ,currentToken_(XToken_Other)
        ,currentStr_(NULL)
        ,scale_(1.0f)
        ,vertices_(DefaultIncSize)
        ,normals_(DefaultIncSize)
        ,uvs_(DefaultIncSize)
        ,currentMaterial_(0)
        ,animObj_(NULL)
    {
    }

    //-----------------------------------------------------------
    XLoader::~XLoader()
    {
        release();
        for(NameTextureMap::size_type pos = nameTexMap_.begin();
            pos != nameTexMap_.end();
            pos = nameTexMap_.next(pos))
        {
            TextureRef *tex = nameTexMap_.getValue(pos);
            LIME_DELETE(tex);
        }

        NameTextureMap empty(0);
        empty.swap(nameTexMap_);
    }

    //-----------------------------------------------------------
    bool XLoader::open(const Char* filepath)
    {
        LASSERT(filepath != NULL);
        file_.open(filepath, lcore::ios::binary);

        return file_.is_open();
    }

    //-----------------------------------------------------------
    void XLoader::close()
    {
        file_.close();
    }

    //-----------------------------------------------------------
    void XLoader::load(lscene::Object& obj, const Char* directory, f32 scale, bool swapOrigin)
    {
        LASSERT(directory != NULL);
        LASSERT(file_.is_open());
        LASSERT(scale>0.0f);

        errorCode_ = Error_None;
        buffer_.clear();
        buffer_.reserve(DefaultBufferSize);
        currentToken_ = XToken_Other;
        currentStr_ = NULL;
        directory_ = directory;

        scale_ = scale;

        u32 size = 0;
        file_.seekg(0, lcore::ios::end);
        size = file_.tellg();
        file_.seekg(0, lcore::ios::beg);

        //ヘッダチェック
        u32 header[4];
        read(file_, header, sizeof(u32)*4);
        if(header[0] != LIME_X_MAGIC){ //マジック
            errorCode_ = Error_Format;
            return;
        }
        if(header[1] != LIME_X_VERSION3032){ //バージョン
            errorCode_ = Error_Format;
            return;
        }

        if(header[2] != LIME_X_TEXT){ //テキストorバイナリ
            errorCode_ = Error_Format;
            return;
        }
        //if(header[3] != LIME_X_FLOAT_BITS_32){ //浮動小数点数ビット数
        //    errorCode_ = Error_Format;
        //    return;
        //}

        animObj_ = &obj;
        processMain();

        if(vertices_.size() == 0){
            errorCode_ = Error_Format;
            return;
        }

        if(errorCode_ != Error_None){
            return;
        }
        createObject(obj, directory, swapOrigin);
    }

    void XLoader::release()
    {
        close();
    }

    //-----------------------------------------------------------
    void XLoader::processMain()
    {
        getNextToken();

        while(!file_.eof() && currentStr_!=NULL){
            switch(currentToken_)
            {
            case XToken_Template:
                processTemplate();
                break;

            default:
                {
                    Template nextTemplate = getTemplate(currentStr_);
                    switch(nextTemplate)
                    {
                    case Template_Material:             processMaterial(); break;
                    case Template_Mesh:                 processMesh(); break;
                    case Template_MeshMaterialList:     processMeshMaterialList(); break;
                    case Template_MeshNormals:          processMeshNormals(); break;
                    case Template_MeshTextureCoords:    processMeshTextureCoords(); break;
                    case Template_MeshVertexColors:     processMeshVertexColors(); break;
                    case Template_VertexDuplicationIndices: processVertexDuplicationIndices(); break;
                    default:
                        processUnsupported();
                        break;
                    };
                }
                break;
            };
            LX_CODE_CHECK_RETURN(Error_None, errorCode_);
            getNextToken();
        }
    }

    //-----------------------------------------------------------
    void XLoader::processTemplate()
    {
        while(!file_.eof()){
            getNextToken();

            if(XToken_CBrace == currentToken_){
                break;
            }
        }
    }

    //-----------------------------------------------------------
    void XLoader::processMaterial()
    {
        processTemplateHeader();
        LX_CODE_CHECK_RETURN(Error_None, errorCode_);

        lscene::Material& material = animObj_->getMaterial(currentMaterial_);

        processVectorMember(&material.diffuse_[0], 4);
        LX_CODE_CHECK_RETURN(Error_None, errorCode_);

        material.specular_[3] = getFloatMember();
        LX_CODE_CHECK_RETURN(Error_None, errorCode_);

        processVectorMember(&material.specular_[0], 3);
        LX_CODE_CHECK_RETURN(Error_None, errorCode_);

        processVectorMember(&material.emissive_[0], 3);
        LX_CODE_CHECK_RETURN(Error_None, errorCode_);

        //その他オプション
        while(!file_.eof()){
            getNextToken();

            if(XToken_CBrace == currentToken_){
                break;
            }

            LX_XTOKEN_CHECK_RETURN(currentToken_, XToken_Other, errorCode_, Error_ParsingMaterial);

            Template nextTemplate = getTemplate(currentStr_);

            switch(nextTemplate)
            {
            case Template_TextureFilename: processTextureFilename(); break;
            default:
                processUnsupported();
                break;
            };
            LX_CODE_CHECK_RETURN(Error_None, errorCode_);
        }

        u32 shader = lscene::shader::ShaderName[lscene::shader::Shader_Phong];
        material.setShaderID( shader );

#if defined(LIME_GLES2)
        //頂点でライティング計算
        material.getFlags().setFlag(lscene::Material::MatFlag_LightingVS);
#endif
        //透過
        f32 emissivePower = material.emissive_.lengthSqr();
        if( material.diffuse_._w > 0.999f){
        }else{

            //発光が0でなければ、フラグ立てる
            if(false == lmath::isEqual(emissivePower, 0.0f))
            {
                material.getFlags().setFlag(lscene::Material::MatFlag_Emissive);
            }
        }

        ++currentMaterial_;
    }


    //-----------------------------------------------------------
    void XLoader::processMesh()
    {
        processTemplateHeader();
        LX_CODE_CHECK_RETURN(Error_None, errorCode_);

        //ベクトル配列のパース
        {
            u32 numVertices = getULongMember();
            LX_CODE_CHECK_RETURN(Error_None, errorCode_);

            vertices_.clear();
            vertices_.reserve(numVertices);
            ProcessVec3Func func(this, &vertices_);
            if(false == processArray(numVertices, func)){
                LX_RETURN(errorCode_, Error_ParsingMesh);
            }

        }

        //インデックス配列パース
        {
            u32 numFaces = getULongMember();
            LX_CODE_CHECK_RETURN(Error_None, errorCode_);

            faces_.create(numFaces);

            ProcessFunc<FaceArray> func(this, &faces_);
            if(false == processArray(numFaces, func)){
                LX_RETURN(errorCode_, Error_ParsingMesh);
            }
        }

        //その他オプション
        while(!file_.eof()){
            getNextToken();

            if(XToken_CBrace == currentToken_){
                break;
            }

            LX_XTOKEN_CHECK_RETURN(currentToken_, XToken_Other, errorCode_, Error_ParsingMesh);

            Template nextTemplate = getTemplate(currentStr_);

            switch(nextTemplate)
            {
            case Template_Material:         processMaterial(); break;
            case Template_MeshMaterialList: processMeshMaterialList(); break;
            case Template_MeshNormals:      processMeshNormals(); break;
            case Template_MeshTextureCoords: processMeshTextureCoords(); break;
            //case Template_MethVertexColors: processMeshVertexColors(); break;
            //case Template_VertexDuplicationIndices: processVertexDuplicationIndices(); break;
            default:
                processUnsupported();
                break;
            };
            LX_CODE_CHECK_RETURN(Error_None, errorCode_);
        }
    }

    //-----------------------------------------------------------
    void XLoader::processMeshFace(FaceArray& faces)
    {
        //インデックス配列のパース
        u32 numIndices = getULongMember();
        LX_CODE_CHECK_RETURN(Error_None, errorCode_);

        LASSERT(numIndices>=3); //最低３つ
        LASSERT(numIndices<=0xFFFFU);

        u32 numPolygons = numIndices - 2;
        if(numPolygons>MaxPolygonsInFace){ //面あたりのポリゴン数制限
            LX_RETURN(errorCode_, Error_ParsingMeshFace);
        }

        FaceArray::Face face = faces.add( static_cast<u16>(numIndices) );

        for(u32 i=0; i<numIndices; ++i){
            getNextToken();
            LX_XTOKEN_CHECK_RETURN(currentToken_, XToken_Other, errorCode_, Error_ParsingMeshFace);

            face.indices_[i] = static_cast<u16>( atoul(currentStr_) );

            getNextToken();
            if(currentToken_ == XToken_Comma){
            }else if(currentToken_ == XToken_Semicolon){
                return;
            }else{
                LX_RETURN(errorCode_, Error_ParsingMeshFace);
            }
        }
    }


    //-----------------------------------------------------------
    void XLoader::processMeshMaterialList()
    {
        processTemplateHeader();
        LX_CODE_CHECK_RETURN(Error_None, errorCode_);

        u32 numMaterials = getULongMember();
        LX_CODE_CHECK_RETURN(Error_None, errorCode_);

        currentMaterial_ = 0;
        if(animObj_->getNumMaterials()<numMaterials){
            lscene::Object tmpObj(numMaterials, numMaterials);
            animObj_->swap(tmpObj);
        }

        {
            u32 numMaterialIndices = getULongMember();
            LX_CODE_CHECK_RETURN(Error_None, errorCode_);
            materialIndices_.clear();
            materialIndices_.reserve(numMaterialIndices);

            ProcessFunc<S8Vector> func(this, &materialIndices_);
            if(false == processArray(numMaterialIndices, func)){
                LX_RETURN(errorCode_, Error_ParsingMeshMaterialList);
            }

            getNextToken();
            LX_XTOKEN_CHECK_RETURN(currentToken_, XToken_Semicolon, errorCode_, Error_ParsingMeshMaterialList);
        }

        //その他オプション
        while(!file_.eof()){
            getNextToken();

            if(XToken_CBrace == currentToken_){
                return;
            }

            LX_XTOKEN_CHECK_RETURN(currentToken_, XToken_Other, errorCode_, Error_ParsingMeshMaterialList);

            Template nextTemplate = getTemplate(currentStr_);

            switch(nextTemplate)
            {
            case Template_Material: processMaterial(); break;
            default:
                processUnsupported();
                break;
            };
            LX_CODE_CHECK_RETURN(Error_None, errorCode_);
        }
    }

    //-----------------------------------------------------------
    void XLoader::processMeshNormals()
    {
        processTemplateHeader();
        LX_CODE_CHECK_RETURN(Error_None, errorCode_);

        {
            u32 numNormals = getULongMember();
            LX_CODE_CHECK_RETURN(Error_None, errorCode_);

            normals_.clear();
            normals_.reserve(numNormals);
            ProcessNormalFunc func(this, &normals_);
            if(false == processArray(numNormals, func)){
                LX_RETURN(errorCode_, Error_ParsingMeshNormals);
            }
        }

        //インデックス配列パース
        {
            u32 numFaceNormals = getULongMember();
            LX_CODE_CHECK_RETURN(Error_None, errorCode_);

            normalFaces_.create(numFaceNormals);

            ProcessFunc<FaceArray> func(this, &normalFaces_);
            if(false == processArray(numFaceNormals, func)){
                LX_RETURN(errorCode_, Error_ParsingMeshNormals);
            }
        }

        getNextToken();
        LX_XTOKEN_CHECK_RETURN(currentToken_, XToken_CBrace, errorCode_, Error_ParsingMeshNormals);
    }

    //-----------------------------------------------------------
    void XLoader::processMeshTextureCoords()
    {
        processTemplateHeader();
        LX_CODE_CHECK_RETURN(Error_None, errorCode_);

        {
            u32 numCoords = getULongMember();
            LX_CODE_CHECK_RETURN(Error_None, errorCode_);

            uvs_.clear();
            uvs_.reserve(2*numCoords);
            ProcessUVFunc func(this, &uvs_);

            u32 i=0;
            for(; i<numCoords; ++i){
                func();
                LX_CODE_CHECK_RETURN(Error_None, errorCode_);
            }

            getNextToken();
            LX_XTOKEN_CHECK_RETURN(currentToken_, XToken_Semicolon, errorCode_, Error_ParsingMeshTextureCoords);

            if(i < (numCoords-1)){
                LX_RETURN(errorCode_, Error_ParsingMeshTextureCoords);
            }
        }
        getNextToken();
        LX_XTOKEN_CHECK_RETURN(currentToken_, XToken_CBrace, errorCode_, Error_ParsingMeshTextureCoords);
    }

    //-----------------------------------------------------------
    void XLoader::processMeshVertexColors()
    {
        //processTemplateHeader();
        //LX_CODE_CHECK_RETURN(Error_None, errorCode_);
    }

    //-----------------------------------------------------------
    void XLoader::processTextureFilename()
    {
        LASSERT(directory_ != NULL);

        processTemplateHeader();
        LX_CODE_CHECK_RETURN(Error_None, errorCode_);

        getStringLiteral();
        if(currentToken_ != XToken_CString){
            LX_RETURN(errorCode_, Error_ParsingTextureFilename);
        }

        //文字列はエスケープ文字等ちゃんと解析すべきか？
        LASSERT(currentStr_ != NULL);

        Char* texName = currentStr_;

        strSJISToUTF8(texName); //UTF8へ変換
        u32 length = lcore::strlen(texName); //何度もstrlenしている

        lgraphics::SamplerState sampler;
        lgraphics::TextureRef* texRef = lconverter::loadTexture(texName, length, directory_, nameTexMap_, sampler);
        if(texRef){
            lscene::Material& material = animObj_->getMaterial(currentMaterial_);
            material.setTextureNum(1);
            material.setTexture(0, *texRef);
            //テクスチャがあったのでフラグ立てる
            material.getFlags().setFlag(lscene::Material::MatFlag_TexAlbedo);

            if(lconverter::Config::getInstance().isAlphaTest()
                && (material.diffuse_._w > 0.999f))
            {

                s32 format = texRef->getFormat();

                if(lgraphics::Buffer_A8R8G8B8 == format
                    || lgraphics::Buffer_A8B8G8R8 == format
                    || lgraphics::Buffer_A4R4G4B4 == format
                    || lgraphics::Buffer_A4B4G4R4 == format)
                {
                    material.getFlags().setFlag( lscene::Material::MatFlag_AlphaTest );
                    material.getRenderState().setAlphaTest(true);
                }
            }

            lgraphics::SamplerState& dstSampler = material.getSamplerState(0);
            dstSampler = sampler;
        }

        getNextToken();
        LX_XTOKEN_CHECK_RETURN(currentToken_, XToken_Semicolon, errorCode_, Error_ParsingTextureFilename);

        getNextToken();
        LX_XTOKEN_CHECK_RETURN(currentToken_, XToken_CBrace, errorCode_, Error_ParsingTextureFilename);
    }

    //-----------------------------------------------------------
    void XLoader::processVertexDuplicationIndices()
    {
        //processTemplateHeader();
        //LX_CODE_CHECK_RETURN(Error_None, errorCode_);
    }

    //-----------------------------------------------------------
    void XLoader::processVector(f32* v, u32 count)
    {
        LASSERT(v != NULL);

        for(u32 i=0; i<count; ++i){
            getNextToken();
            LX_XTOKEN_CHECK_RETURN(currentToken_, XToken_Other, errorCode_, Error_ParsingVector);
            v[i] = static_cast<f32>( atof(currentStr_) );

            getNextToken();
            LX_XTOKEN_CHECK_RETURN(currentToken_, XToken_Semicolon, errorCode_, Error_ParsingVector);
        }
    }

    //-----------------------------------------------------------
    void XLoader::processVectorMember(f32* v, u32 count)
    {
        processVector(v, count);
        LX_CODE_CHECK_RETURN(Error_None, errorCode_);

        getNextToken();
        LX_XTOKEN_CHECK_RETURN(currentToken_, XToken_Semicolon, errorCode_, Error_ParsingVector);
    }

    //-----------------------------------------------------------
    void XLoader::processMatrix44(f32* matrix)
    {
        // array float[16];
        ProcessFunc<f32> func(this, matrix);
        if(false == processArray(16, func)){
            LX_RETURN(errorCode_, Error_ParsingMatrix44);
        }
    }

    //-----------------------------------------------------------
    void XLoader::processUUID()
    {
        getNextToken();
        if(XToken_OAngle != currentToken_){
            LX_RETURN(errorCode_, Error_ParsingUUID);
        }

        getNextToken();
        if(XToken_Other != currentToken_){
            LX_RETURN(errorCode_, Error_ParsingUUID);
        }

        getNextToken();
        if(XToken_CAngle != currentToken_){
            LX_RETURN(errorCode_, Error_ParsingUUID);
        }
    }

    //-----------------------------------------------------------
    void XLoader::processUnsupported()
    {
        processTemplateHeader();
        LX_CODE_CHECK_RETURN(Error_None, errorCode_);

        u32 depth = 1;
        while(!file_.eof()){
            getNextToken();

            if(XToken_OBrace == currentToken_){
                ++depth;
                continue;
            }

            if(XToken_CBrace == currentToken_){
                --depth;
                if(depth==0){
                    return;
                }
                continue;
            }
        }
    }

    //-----------------------------------------------------------
    void XLoader::processTemplateHeader()
    {
        getNextToken();
        if(currentToken_ == XToken_Other){
            getNextToken();
        }
        LX_XTOKEN_CHECK_RETURN(currentToken_, XToken_OBrace, errorCode_, Error_ParsingTemplateHeader);
    }

    //-----------------------------------------------------------
    u32 XLoader::getULong()
    {
        getNextToken();
        if(currentToken_ != XToken_Other){
            LX_RETURN_VALUE(errorCode_, Error_ParsingULONG, 0);
        }
        return atoul(currentStr_);
    }

    //-----------------------------------------------------------
    u32 XLoader::getULongMember()
    {
        getNextToken();
        if(currentToken_ != XToken_Other){
            LX_RETURN_VALUE(errorCode_, Error_ParsingULONG, 0);
        }
        u32 ret = atoul(currentStr_);

        getNextToken();
        LX_XTOKEN_CHECK_RETURN_VALUE(currentToken_, XToken_Semicolon, errorCode_, Error_ParsingULONG, ret);
        return ret;
    }

    //-----------------------------------------------------------
    f32 XLoader::getFloat()
    {
        getNextToken();
        if(currentToken_ != XToken_Other){
            LX_RETURN_VALUE(errorCode_, Error_ParsingFloat, 0.0f);
        }
        return static_cast<f32>( atof(currentStr_) );
    }

    //-----------------------------------------------------------
    f32 XLoader::getFloatMember()
    {
        getNextToken();
        if(currentToken_ != XToken_Other){
            LX_RETURN_VALUE(errorCode_, Error_ParsingFloat, 0.0f);
        }
        f32 ret = static_cast<f32>( atof(currentStr_) );

        getNextToken();
        LX_XTOKEN_CHECK_RETURN_VALUE(currentToken_, XToken_Semicolon, errorCode_, Error_ParsingFloat, ret);
        return ret;
    }

    //-----------------------------------------------------------
    s32 XLoader::skipSpace()
    {
        s32 c = 0;

        // 空白スキップ
        while(!file_.eof()){
            c = file_.get();
            if(false == isSkip(c)){

                //コメント行処理
                if(c == '#'){
                    skipLine();
                }else if(c == '/'){
                    buffer_.push_back( static_cast<Char>(c) );
                    c = file_.get();
                    if(c == '/'){
                        buffer_.clear();
                        skipLine();
                    }else{
                        break;
                    }
                }else{
                    break;
                }
            }
        }
        return c;
    }

    //-----------------------------------------------------------
    // 次トークン取得
    void XLoader::getNextToken()
    {
        buffer_.clear();

        s32 c = skipSpace();

        // 1文字のトークン処理
        //-------------------------------------
        currentToken_ = isBreak(c);
        if('.' == c){
            currentToken_ = XToken_Dot;
        }

        buffer_.push_back( static_cast<Char>(c) );

        if(currentToken_ != XToken_Other){
            buffer_.push_back('\0');
            currentStr_ = &(buffer_[0]);

            LX_DEBUG_OUT(currentStr_);
            return;
        }

        // 区切り文字まで文字列ロード
        //--------------------------------------
        while(!file_.eof()){
            c = file_.get();
            if(isSkip(c)){
                break;
            }else if(XToken_Other != isBreak(c)){
                file_.seekg(-1, lcore::ios::cur); //ひとつ戻る
                break;
            }

            buffer_.push_back( static_cast<Char>(c) );
        }

        buffer_.push_back('\0');
        if(buffer_.size() <= 1){
            currentToken_ = XToken_None;
            currentStr_ = NULL;
        }else{
            currentStr_ = &(buffer_[0]);
            currentToken_ = getTokenType(currentStr_);

            LX_DEBUG_OUT(currentStr_);
        }
    }

    //-----------------------------------------------------------
    void XLoader::getStringLiteral()
    {
        buffer_.clear();

        s32 c = skipSpace();

        buffer_.clear();

        // 1文字のトークン処理
        //-------------------------------------
        if('\"' != c){
            currentToken_ = XToken_None;
            return;
        }


        // 区切り文字まで文字列ロード
        //--------------------------------------
        while(!file_.eof()){
            c = file_.get();

            if(c == '\"'){
                break;
            }

            buffer_.push_back( static_cast<Char>(c) );
        }

        buffer_.push_back('\0');
        if(buffer_.size() <= 0){
            currentToken_ = XToken_None;
            currentStr_ = NULL;
        }else{
            currentStr_ = &(buffer_[0]);
            currentToken_ = XToken_CString;
        }
    }

    //-----------------------------------------------------------
    // １行読み飛ばす
    void XLoader::skipLine()
    {
        s32 c = file_.get();
        while(c != '\a' && c != '\r'){
            c = file_.get();
        }
        while(c == '\a' || c == '\r'){
            c = file_.get();
        }
        file_.seekg(-1, lcore::ios::cur); //ひとつ戻る
    }

    //-----------------------------------------------------------
    XLoader::VertexDeclType XLoader::getDeclType() const
    {
        if(normals_.size()>0 && uvs_.size()>0){
            return VertexDecl_PNT;
        }else if(normals_.size()>0){
            return VertexDecl_PN;
        }else if(uvs_.size()>0){
            return VertexDecl_PT;
        }
        return VertexDecl_P;
    }

    inline void XLoader::createVertexDecl(lgraphics::VertexDeclarationRef& decl, VertexDeclType type)
    {
        switch(type)
        {
        case VertexDecl_P:
            {
                lgraphics::VertexDeclCreator creator(1);
                creator.add(0, 0, lgraphics::DeclType_Float3, lgraphics::DeclMethod_Default, lgraphics::DeclUsage_Position, 0);
                creator.end( decl );
            }
            break;

        case VertexDecl_PN:
            {
                lgraphics::VertexDeclCreator creator(2);
                u16 offset = 0;
                offset += creator.add(0, offset, lgraphics::DeclType_Float3, lgraphics::DeclMethod_Default, lgraphics::DeclUsage_Position, 0);
                offset += creator.add(0, offset, lgraphics::DeclType_Short4N, lgraphics::DeclMethod_Normalize, lgraphics::DeclUsage_Normal, 0);
                creator.end( decl );
            }
            break;

        case VertexDecl_PT:
            {
                lgraphics::VertexDeclCreator creator(2);
                u16 offset = 0;
                offset += creator.add(0, offset, lgraphics::DeclType_Float3, lgraphics::DeclMethod_Default, lgraphics::DeclUsage_Position, 0);

#if defined(LX_UV_TYPE_PACKED_S16)
                offset += creator.add(0, offset, lgraphics::DeclType_Short2N, lgraphics::DeclMethod_Normalize, lgraphics::DeclUsage_Texcoord, 0);
#else
                offset += creator.add(0, offset, lgraphics::DeclType_Float2, lgraphics::DeclMethod_Default, lgraphics::DeclUsage_Texcoord, 0);
#endif

                creator.end( decl );
            }
            break;

        case VertexDecl_PNT:
            {
                lgraphics::VertexDeclCreator creator(3);
                u16 offset = 0;
                offset += creator.add(0, offset, lgraphics::DeclType_Float3, lgraphics::DeclMethod_Default, lgraphics::DeclUsage_Position, 0);
                offset += creator.add(0, offset, lgraphics::DeclType_Short4N, lgraphics::DeclMethod_Normalize, lgraphics::DeclUsage_Normal, 0);

#if defined(LX_UV_TYPE_PACKED_S16)
                offset += creator.add(0, offset, lgraphics::DeclType_Short2N, lgraphics::DeclMethod_Normalize, lgraphics::DeclUsage_Texcoord, 0);
#else
                offset += creator.add(0, offset, lgraphics::DeclType_Float2, lgraphics::DeclMethod_Default, lgraphics::DeclUsage_Texcoord, 0);
#endif

                creator.end( decl );
            }
            break;

        default:
            LASSERT(false);
            break;
        };
    }


    inline void XLoader::createIndexBuffer(lgraphics::IndexBufferRef& ib, lscene::Object& obj)
    {
        u32 numIndices = faces_.getTotalIndices();
        if(numIndices<=0){
            ib = lgraphics::IndexBufferRef();
            return;
        }

        LASSERT(obj.getNumMaterials() == obj.getNumGeometries());

        U16Vector tmpIndices(32);
        tmpIndices.reserve(numIndices);

        u32 indexOffset = 0;
        u32 numIndicesInMaterial = 0;
        u32 numFaces = faces_.getNumFaces();
        u32 numPoly;
        FaceArray::Face face(NULL);

        lgraphics::RenderStateRef renderState;
        lgraphics::RenderStateRef renderStateAlpha;
        createRenderState(renderState, false);
        createRenderState(renderStateAlpha, true);

        LASSERT(numFaces == materialIndices_.size());

        for(u32 i=0; i<obj.getNumMaterials(); ++i){
            numIndicesInMaterial = 0;


            for(u32 j=0; j<numFaces; ++j){
                if( i != static_cast<u32>(materialIndices_[j]) ){
                    continue;
                }

                face = faces_.getFace(j);
                LASSERT(faces_.getNumIndices(j)>=3); //3頂点以上かならずある
                numPoly = faces_.getNumIndices(j) - 2;
                for(u32 k=1; k<=numPoly; ++k){
                    tmpIndices.push_back(face[0]);
                    tmpIndices.push_back(face[k]);
                    tmpIndices.push_back(face[k+1]);
                    numIndicesInMaterial += 3;
                }
            }

            obj.getGeometry(i).setMaterialIndex( static_cast<u8>(i) );
            obj.getGeometry(i).setIndexOffset(indexOffset);
            obj.getGeometry(i).setNumIndices(numIndicesInMaterial);
            obj.getGeometry(i).setPrimitiveCount(numIndicesInMaterial/3);

            //アルファブレンドするか
            if( obj.getMaterial(i).diffuse_._w > 0.999f){
                obj.getMaterial(i).setRenderState(renderState);
            }else{
                obj.getMaterial(i).setRenderState(renderStateAlpha);
            }

            indexOffset += numIndicesInMaterial;
        }

        ib = lgraphics::IndexBuffer::create(indexOffset, lgraphics::Pool_Default, lgraphics::Usage_None);
#if defined(LIME_GLES2)
        ib.blit( &(tmpIndices[0]) );
#else
        ib.blit( &(tmpIndices[0]), 0, sizeof(u16)*indexOffset );
#endif
    }


    //-----------------------------------------------------------
    void XLoader::createObject(lscene::Object& obj, const Char* directory, bool /*swapOrigin*/)
    {
#if defined(LIME_GLES2)
#define LIME_XLOADER_CREATEOBJECT_BLIT_BUFFER(vb, ptr, offset, size) vb.blit(reinterpret_cast<void*>(ptr))
#else
#define LIME_XLOADER_CREATEOBJECT_BLIT_BUFFER(vb, ptr, offset, size) vb.blit(reinterpret_cast<void*>(ptr), offset, size)
#endif

        LASSERT(directory != NULL);

        //頂点のスケーリング
        if(lmath::isEqual(scale_, 1.0f) == false){
            for(u32 i=0; i<vertices_.size(); ++i){
                vertices_[i] *= scale_;
            }
        }

        lgraphics::VertexBufferRef vb;
        lgraphics::IndexBufferRef ib;

        VertexDeclType declType = getDeclType();

        lgraphics::VertexDeclarationRef decl;
        createVertexDecl(decl, declType);
        createIndexBuffer(ib, obj);

        bool hasNormal = false;
        switch(declType)
        {
        case VertexDecl_P:
            {
                vb = lgraphics::VertexBuffer::create(sizeof(lmath::Vector3), vertices_.size(), lgraphics::Pool_Default, lgraphics::Usage_None);

                LIME_XLOADER_CREATEOBJECT_BLIT_BUFFER(vb, &(vertices_[0]), 0, sizeof(lmath::Vector3)*vertices_.size());
            }
            break;

        case VertexDecl_PN:
            {
                hasNormal = true;

                vb = lgraphics::VertexBuffer::create(sizeof(VertexPN), vertices_.size(), lgraphics::Pool_Default, lgraphics::Usage_None);
                VertexPN *vertices = LIME_NEW VertexPN[ vertices_.size() ];

                //頂点コピー
                for(u32 i=0; i<vertices_.size(); ++i){
                    vertices[i].pos_ = vertices_[i];
                }

                //法線コピー
                LASSERT(faces_.getNumFaces() == normalFaces_.getNumFaces());

                u32 numFaces = normalFaces_.getNumFaces();
                u32 numIndices = 0;
                FaceArray::Face face;
                FaceArray::Face normalFace;
                for(u32 i=0; i<numFaces; ++i){
                    LASSERT(faces_.getNumIndices(i) == normalFaces_.getNumIndices(i));
                    numIndices = faces_.getNumIndices(i);
                    face = faces_.getFace(i);
                    normalFace = normalFaces_.getFace(i);
                    for(u32 j=0; j<numIndices; ++j){
                        vertices[ face[j] ].normal_ = normals_[ normalFace[j] ];
                    }
                }

                LIME_XLOADER_CREATEOBJECT_BLIT_BUFFER(vb, vertices, 0, sizeof(VertexPN)*vertices_.size());
                LIME_DELETE_ARRAY(vertices);
            }
            break;

        case VertexDecl_PT:
            {
                vb = lgraphics::VertexBuffer::create(sizeof(VertexPT), vertices_.size(), lgraphics::Pool_Default, lgraphics::Usage_None);
                VertexPT *vertices = LIME_NEW VertexPT[ vertices_.size() ];
                //頂点コピー
                for(u32 i=0; i<vertices_.size(); ++i){
                    vertices[i].pos_ = vertices_[i];
                }

                //UVコピー
                LASSERT(2*vertices_.size() >= uvs_.size());

                for(u32 i=0; i<vertices_.size(); ++i){
                    vertices[i].uv_.elem_[0] = uvs_[2*i];
                    vertices[i].uv_.elem_[1] = uvs_[2*i+1];
                }

                LIME_XLOADER_CREATEOBJECT_BLIT_BUFFER(vb, vertices, 0, sizeof(VertexPT)*vertices_.size());
                LIME_DELETE_ARRAY(vertices);
            }
            break;

        case VertexDecl_PNT:
            {
                hasNormal = true;

                vb = lgraphics::VertexBuffer::create(sizeof(VertexPNT), vertices_.size(), lgraphics::Pool_Default, lgraphics::Usage_None);
                VertexPNT *vertices = LIME_NEW VertexPNT[vertices_.size()];
                //頂点コピー
                for(u32 i=0; i<vertices_.size(); ++i){
                    vertices[i].pos_ = vertices_[i];
                }

                //法線コピー
                LASSERT(faces_.getNumFaces() == normalFaces_.getNumFaces());

                u32 numFaces = normalFaces_.getNumFaces();
                u32 numIndices = 0;
                FaceArray::Face face;
                FaceArray::Face normalFace;
                for(u32 i=0; i<numFaces; ++i){
                    LASSERT(faces_.getNumIndices(i) == normalFaces_.getNumIndices(i));
                    numIndices = faces_.getNumIndices(i);
                    face = faces_.getFace(i);
                    normalFace = normalFaces_.getFace(i);
                    for(u32 j=0; j<numIndices; ++j){
                        vertices[ face[j] ].normal_ = normals_[ normalFace[j] ];
                    }
                }

                //UVコピー
                LASSERT(2*vertices_.size() >= uvs_.size());
                for(u32 i=0; i<vertices_.size(); ++i){
                    vertices[i].uv_.elem_[0] = uvs_[2*i];
                    vertices[i].uv_.elem_[1] = uvs_[2*i+1];
                }

                LIME_XLOADER_CREATEOBJECT_BLIT_BUFFER(vb, vertices, 0, sizeof(VertexPNT)*vertices_.size());
                LIME_DELETE_ARRAY(vertices);
            }
            break;

        default:
            LASSERT(false);
            break;
        };

        lgraphics::GeometryBuffer::pointer geomBufferPtr( LIME_NEW lgraphics::GeometryBuffer(lgraphics::Primitive_TriangleList, decl, vb, ib) );

        for(u32 i=0; i<obj.getNumGeometries(); ++i){
            obj.getGeometry(i).setGeometryBuffer(geomBufferPtr);
        }

//デバッグ用ログ
#if defined(LIME_LIBCONVERTER_DEBUGLOG_ENABLE)
        debugLog_.addNumVertices(vertices_.size());
        debugLog_.addNumBatches( obj.getNumGeometries() );
#endif


#undef LIME_XLOADER_CREATEOBJECT_BLIT_BUFFER
    }


#if 0
    void XLoader::debugOut(const char* file)
    {
        LASSERT(file != NULL);
        lcore::ofstream out(file, std::ios::binary);
        if(false == out.is_open()){
            return;
        }

        lcore::vector_arena<Frame*> stack(16);
        Frame* tmp = rootNode_;
        //兄弟をスタックに積む
        Frame* sibling = tmp->getSibling();
        while(sibling != NULL){
            stack.push_back(sibling);
            sibling = sibling->getSibling();
        }

        while(tmp != NULL){
            //フレーム出力
            debugOutFrame(out, *tmp);

            if(tmp->child_){
                tmp = tmp->child_;
                //兄弟をスタックに積む
                sibling = tmp->getSibling();
                while(sibling != NULL){
                    stack.push_back(sibling);
                    sibling = sibling->getSibling();
                }
                continue;
            }

            //トラックバック
            if(stack.size()<=0){
                break;
            }
            tmp = stack.back();
            stack.pop_back();
        }
    }
#endif
}
