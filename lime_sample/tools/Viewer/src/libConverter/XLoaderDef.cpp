/**
@file XLoaderDef.cpp
@author t-sakai
@date 2011/01/20 create

*/
#include "XLoaderDef.h"

namespace lx
{
    // Xファイルトークン
    //----------------------------------------------------------------------
    XToken getTokenType(const Char* str)
    {
        LASSERT(str != NULL);
        if(0 == strcmp(str, "template")){
            return XToken_Template;
        }
        return XToken_Other;
    }

    // template
    //----------------------------------------------------------------------
    // template names
    const Char* TemplateNames[Template_Num] =
    {
        "Boolean",
        "Boolean2d",
        "ColorRGB",
        "ColorRGBA",
        "Coords2d",
        "Frame",
        "FrameTransformMatrix",
        "Material",
        "Matrix4x4",
        "Mesh",
        "MeshFace",
        "MeshMaterialList",
        "MeshNormals",
        "MeshTextureCoords",
        "MeshVertexColors",
        "TextureFilename",
        "Vector",
        "VertexDuplicationIndices",
    };

    // テンプレートタイプ取得
    Template getTemplate(const Char* name)
    {
        LASSERT(name != NULL);
        for(s32 i=0; i<Template_Num; ++i){
            if(0 == strcmp(name, TemplateNames[i])){
                return static_cast<Template>(i);
            }
        }
        return Template_Unsupported;
    }

    //
    FaceArray::FaceArray()
        :numFaces_(0)
        ,numIndices_(0)
        ,toFaceTop_(NULL)
        ,indices_(32)
    {
    }

    FaceArray::~FaceArray()
    {
        release();
    }

    void FaceArray::create(u32 numFaces)
    {
        release();

        toFaceTop_ = LIME_NEW u32[numFaces];
        indices_.reserve(numFaces*4);
    }

    void FaceArray::release()
    {
        numFaces_ = 0;
        numIndices_ = 0;
        LIME_DELETE_ARRAY(toFaceTop_);
        indices_.clear();
    }

    FaceArray::Face FaceArray::add(u16 numIndices)
    {
        LASSERT(numIndices>=3);
        numIndices_ += (numIndices-2)*3;

        u32 size = indices_.size();
        for(u32 i=0; i<=numIndices; ++i){
            indices_.push_back(numIndices);
        }

        toFaceTop_[numFaces_] = size;
        ++numFaces_;
        return Face(&(indices_[size+1]));
    }
}
