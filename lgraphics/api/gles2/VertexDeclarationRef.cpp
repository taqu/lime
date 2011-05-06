/**
@file VertexDeclarationRef.cpp
@author t-sakai
@date 2010/06/06 create
@date 2010/12/12 modify for gles2
*/
#include "lgl2.h"
#include "lgl2ext.h"
#include <lmath/lmath.h>
#include "../../lgraphicscore.h"
#include "VertexDeclarationRef.h"
#include "GraphicsDeviceRef.h"
#include "ShaderRef.h"

namespace lgraphics
{
    namespace
    {

        s32 DeclTypeSizeTable[DeclType_Num] =
        {
            1,
            2,
            3,
            4,
            4,
            4,
            2,
            4,
            4,
            2,
            4,
            2,
            4,
            3,
            3,
            2,
            4,
        };

        GLenum DeclTypeTable[DeclType_Num] =
        {
            GL_FLOAT,
            GL_FLOAT,
            GL_FLOAT,
            GL_FLOAT,
            GL_UNSIGNED_BYTE,
            GL_UNSIGNED_BYTE,
            GL_SHORT,
            GL_SHORT,
            GL_UNSIGNED_BYTE,
            GL_SHORT,
            GL_SHORT,
            GL_UNSIGNED_SHORT,
            GL_UNSIGNED_SHORT,
            GL_UNSIGNED_BYTE,
            GL_UNSIGNED_BYTE,
            GL_HALF_FLOAT_OES,
            GL_HALF_FLOAT_OES,
        };

        GLboolean DeclMethodNormalizedTable[DeclMethod_Num] =
        {
            GL_FALSE,
            GL_TRUE,
        };

        static const s32 MaxUageNameLength = 16;
        const char* DeclUsageNameTable[DeclUsage_Num] =
        {
            "a_pos",
            "a_weights",
            "a_indices",
            "a_normal",
            "a_psize",
            "a_tex",
            "a_tangent",
            "a_binormal",
            "a_tess",
            "a_posT",
            "a_color",
            "a_fog",
            "a_depth",
            "a_sample",
        };

        u32 DeclUsageNameSizeTable[DeclUsage_Num] =
        {
            sizeof("a_pos") - 1,
            sizeof("a_weights") - 1,
            sizeof("a_indices") - 1,
            sizeof("a_normal") - 1,
            sizeof("a_psize") - 1,
            sizeof("a_tex") - 1,
            sizeof("a_tangent") - 1,
            sizeof("a_binormal") - 1,
            sizeof("a_tess") - 1,
            sizeof("a_posT") - 1,
            sizeof("a_color") - 1,
            sizeof("a_fog") - 1,
            sizeof("a_depth") - 1,
            sizeof("a_sample") - 1,
        };

    }

    //-------------------------------------------------
    //---
    //--- VertexDeclaration
    //---
    //-------------------------------------------------
    class VertexDeclaration
    {
    public:
        VertexDeclaration(VertexElement* elements)
            :count_(0)
            ,elements_(elements)
        {
        }

        ~VertexDeclaration()
        {
            LIME_DELETE_ARRAY(elements_);
        }

        void addRef()
        {
            ++count_;
        }

        void release()
        {
            if(--count_ == 0){
                LIME_DELETE_NONULL this;
            }
        }

        u32 count_;
        VertexElement *elements_;
    };


    //-------------------------------------------------
    //---
    //--- VertexDeclarationRef
    //---
    //-------------------------------------------------
    VertexDeclarationRef::VertexDeclarationRef(const VertexDeclarationRef& rhs)
        :vertexSize_(rhs.vertexSize_)
        ,declaration_(rhs.declaration_)
    {
        // 参照カウントを増やす
        if(declaration_ != NULL){
            declaration_->addRef();
        }

        for(u32 i=0; i<LIME_MAX_VERTEX_STREAMS; ++i){
            numElements_[i] = rhs.numElements_[i];
            streamElementsOffset_[i] = rhs.streamElementsOffset_[i];
        }
    }

    VertexDeclarationRef::VertexDeclarationRef(u32 vertexSize, u32 numElements, VertexDeclaration* declaration)
            :vertexSize_(vertexSize)
            ,declaration_(declaration)
    {
        for(u32 i=0; i<LIME_MAX_VERTEX_STREAMS; ++i){
            numElements_[i] = 0;
            streamElementsOffset_[i] = 0;
        }

        VertexElement *elements = declaration_->elements_;
        for(u32 i=0; i<numElements; ++i){
            LASSERT(0<=elements[i].stream_ && elements[i].stream_<LIME_MAX_VERTEX_STREAMS);
            ++numElements_[ elements[i].stream_ ];
        }

        u16 offset = 0;
        for(u32 i=0; i<LIME_MAX_VERTEX_STREAMS; ++i){
            streamElementsOffset_[i] = offset;
            offset += numElements_[i];
        }
    }

    void VertexDeclarationRef::destroy()
    {
        for(u32 i=0; i<LIME_MAX_VERTEX_STREAMS; ++i){
            numElements_[i] = 0;
            streamElementsOffset_[i] = 0;
        }

        LSAFE_RELEASE(declaration_);
    }

    void VertexDeclarationRef::attach(u32 stream) const
    {
        LASSERT(declaration_ != NULL);

        VertexElement *elements = declaration_->elements_;
        u32 elementsEnd = numElements_[stream] + streamElementsOffset_[stream];

        for(u32 i=streamElementsOffset_[stream]; i<elementsEnd; ++i){
            s32 size = DeclTypeSizeTable[elements[i].element_[VertexElement::Elem_Type] ];
            GLenum type = DeclTypeTable[elements[i].element_[VertexElement::Elem_Type] ];
            GLboolean normalized = DeclMethodNormalizedTable[ elements[i].element_[VertexElement::Elem_Method] ];

            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, size, type, normalized, vertexSize_, (const void*)elements[i].offset_);
        }
    }

    void VertexDeclarationRef::attach(u32 stream, const u8* buffer) const
    {
        LASSERT(declaration_ != NULL);
        LASSERT(buffer != NULL);

        VertexElement *elements = declaration_->elements_;
        u32 elementsEnd = numElements_[stream] + streamElementsOffset_[stream];

        for(u32 i=streamElementsOffset_[stream]; i<elementsEnd; ++i){
            s32 size = DeclTypeSizeTable[elements[i].element_[VertexElement::Elem_Type] ];
            GLenum type = DeclTypeTable[elements[i].element_[VertexElement::Elem_Type] ];
            GLboolean normalized = DeclMethodNormalizedTable[ elements[i].element_[VertexElement::Elem_Method] ];

            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, size, type, normalized, vertexSize_, buffer+elements[i].offset_);
        }
    }

    void VertexDeclarationRef::detach(u32 stream) const
    {
        u32 elementsEnd = numElements_[stream] + streamElementsOffset_[stream];

        for(u32 i=streamElementsOffset_[stream]; i<elementsEnd; ++i){
            glDisableVertexAttribArray(i);
        }
    }

    // 頂点属性をシェーダにバインド
    void VertexDeclarationRef::bindAttributes(u32 stream, u32 program)
    {
        char name[MaxUageNameLength];

        VertexElement *elements = declaration_->elements_;
        u32 elementsEnd = numElements_[stream] + streamElementsOffset_[stream];

        for(u32 i=streamElementsOffset_[stream]; i<elementsEnd; ++i){
            u8 usage = elements[i].element_[VertexElement::Elem_Usage];
            u32 size = DeclUsageNameSizeTable[usage];
            lcore::memcpy(name, DeclUsageNameTable[usage], size);

            name[ size ] = '0' + elements[i].element_[VertexElement::Elem_UsageIndex];
            name[ size + 1 ] = '\0';
            glBindAttribLocation(program, i, name);
        }
    }

    u32 VertexDeclarationRef::getNumElements() const
    {
        u32 numElements = 0;
        for(u32 i=0; i<LIME_MAX_VERTEX_STREAMS; ++i){
            numElements += numElements_[i];
        }
        return numElements;
    }

    bool VertexDeclarationRef::getDecl(VertexElement* decl)
    {
        if(declaration_ == NULL){
            return false;
        }

        u32 numElements = getNumElements();
        for(u32 i=0; i<=numElements; ++i){
            decl[i] = declaration_->elements_[i];
        }
        return true;
    }

    void VertexDeclarationRef::swap(VertexDeclarationRef& rhs)
    {
        lcore::swap(vertexSize_, rhs.vertexSize_);

        for(u32 i=0; i<LIME_MAX_VERTEX_STREAMS; ++i){
            lcore::swap(numElements_[i], rhs.numElements_[i]);
            lcore::swap(streamElementsOffset_[i], rhs.streamElementsOffset_[i]);
        }

        lcore::swap(declaration_, rhs.declaration_);
    }


    //-------------------------------------------------
    //---
    //--- VertexDeclCreator
    //---
    //-------------------------------------------------
    VertexDeclCreator::VertexDeclCreator(u32 elementNum)
        :elements_(NULL)
        ,count_(0)
        ,size_(elementNum + 1) //終端の番兵用に+1
        ,vertexSize_(0)
    {
        elements_ = LIME_NEW VertexElement[size_];
    }

    VertexDeclCreator::~VertexDeclCreator()
    {
        LIME_DELETE_ARRAY( elements_ );
    }

    u16 VertexDeclCreator::add(u16 stream, u16 offset, DeclType type, DeclMethod method, DeclUsage usage, u8 usageIndex)
    {
        LASSERT(count_ < size_);
        LASSERT(0<=type && type<=DeclType_UnUsed);
        LASSERT(0<=method && method<=DeclMethod_Normalize);
        LASSERT(0<=usage && usage<DeclUsage_Num);

        elements_[count_].stream_ = stream;
        elements_[count_].offset_ = offset;
        elements_[count_].element_[VertexElement::Elem_Type] = static_cast<u8>( type );
        elements_[count_].element_[VertexElement::Elem_Method] = static_cast<u8>( method );
        elements_[count_].element_[VertexElement::Elem_Usage] = static_cast<u8>( usage );
        elements_[count_].element_[VertexElement::Elem_UsageIndex] = usageIndex;

        ++count_;

        static const u16 bytes[DeclType_UnUsed] =
        {
            sizeof(f32),
            sizeof(lmath::Vector2),
            sizeof(lmath::Vector3),
            sizeof(lmath::Vector4),
            sizeof(u32),
            sizeof(u8) * 4,
            sizeof(u16) * 2,
            sizeof(u16) * 4,
            sizeof(u8) * 4,
            sizeof(u16) * 2,
            sizeof(u16) * 4,
            sizeof(u16) * 2,
            sizeof(u16) * 4,
            sizeof(u32),
            sizeof(u32),
            sizeof(f32),
            sizeof(f32) * 2,
        };
        vertexSize_ += (stream==0xFFU)? 0 : bytes[type];
        return bytes[type];
    }

    void VertexDeclCreator::end(VertexDeclarationRef& declaration)
    {
        LASSERT((count_+1) == size_);

        declEnd();

        VertexDeclaration *decl = LIME_NEW VertexDeclaration(elements_);
        if(decl == NULL){
            return;
        }
        decl->addRef();
        elements_ = NULL;

        VertexDeclarationRef tmp(vertexSize_, size_-1, decl);
        declaration.swap(tmp);
    }

    void VertexDeclCreator::end(VertexDeclarationRef& declaration, u32 vertexSize)
    {
        vertexSize_ = vertexSize;
        end(declaration);
    }
}
