#ifndef INC_LGRAPHICS_ES2_VERTEXDECLARATIONREF_H__
#define INC_LGRAPHICS_ES2_VERTEXDECLARATIONREF_H__
/**
@file VertexDeclarationRef.h
@author t-sakai
@date 2010/06/06 create
@date 2010/12/12 modify for gles2
*/
#include "../../lgraphicscore.h"
#include "Enumerations.h"

namespace lgraphics
{
    struct VertexElement
    {
        enum ElementIndex
        {
            Elem_Type =0,
            Elem_Method,
            Elem_Usage,
            Elem_UsageIndex,
            Elem_Num,
        };

        u8 getType() const{ return element_[Elem_Type];}
        u8 getMethod() const{ return element_[Elem_Method];}
        u8 getUsage() const{ return element_[Elem_Usage];}
        u8 getUsageIndex() const{ return element_[Elem_UsageIndex];}

        u16 stream_;
        u16 offset_;
        u8 element_[Elem_Num];
    };

    class VertexDeclaration;
    class ShaderRefBase;

    class VertexDeclarationRef
    {
    public:
        static const u32 MAX_ELEMENTS = 64;

        VertexDeclarationRef()
            :vertexSize_(0)
            ,numElements_(0)
            ,declaration_(NULL)
        {
        }

        VertexDeclarationRef(const VertexDeclarationRef& rhs);
        ~VertexDeclarationRef()
        {
            destroy();
        }

        VertexDeclarationRef& operator=(const VertexDeclarationRef& rhs)
        {
            VertexDeclarationRef tmp(rhs);
            tmp.swap(*this);
            return *this;
        }

        void destroy();

        bool valid() const{ return (declaration_ != NULL); }

        void attach() const;
        void detach() const;

        u32 getVertexSize() const{ return vertexSize_;}
        u32 getNumElements() const{ return numElements_;}
        bool getDecl(VertexElement* decl);

        void swap(VertexDeclarationRef& rhs)
        {
            lcore::swap(vertexSize_, rhs.vertexSize_);
            lcore::swap(numElements_, rhs.numElements_);
            lcore::swap(declaration_, rhs.declaration_);
        }

        /**
        @brief 頂点属性をシェーダにバインド
        */
        void bindAttributes(u32 program);
    private:
        friend class VertexDeclCreator;
        friend class Shader;

        VertexDeclarationRef(u32 vertexSize, u32 numElements, VertexDeclaration* declaration)
            :vertexSize_(vertexSize)
            ,numElements_(numElements)
            ,declaration_(declaration)
        {
        }

        u32 vertexSize_;
        u32 numElements_;
        VertexDeclaration *declaration_;
    };


    class VertexDeclCreator
    {
    public:
        VertexDeclCreator(u32 elementNum);
        ~VertexDeclCreator();

        /**
        @return 要素のバイト数を返す
        */
        u16 add(u16 stream, u16 offset, DeclType type, DeclMethod method, DeclUsage usage, u8 usageIndex);
        void end(VertexDeclarationRef& declaration);
        void end(VertexDeclarationRef& declaration, u32 vertexSize);
    private:
        VertexDeclCreator(const VertexDeclCreator&);
        VertexDeclCreator& operator=(const VertexDeclCreator&);

        void declEnd()
        {
            add(0xFFU, 0, DeclType_UnUsed, DeclMethod_Default, DeclUsage_Position, 0);
        }
        VertexElement *elements_;
        u32 count_;
        u32 size_;
        u32 vertexSize_;
    };
}

//---------------------------------------------------
//---
//--- lcore::swap特殊化
//---
//---------------------------------------------------
namespace lcore
{
    template<>
    inline void swap<lgraphics::VertexDeclarationRef>(lgraphics::VertexDeclarationRef& l, lgraphics::VertexDeclarationRef& r)
    {
        l.swap(r);
    }
}

#endif //INC_LGRAPHICS_ES2_VERTEXDECLARATIONREF_H__
