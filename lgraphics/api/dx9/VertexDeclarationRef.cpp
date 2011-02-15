/**
@file VertexDeclarationRef.cpp
@author t-sakai
@date 2009/04/30
*/
#include "../../lgraphics.h"
#include <lmath/lmath.h>
#include "GraphicsDeviceRef.h"

#include "VertexDeclarationRef.h"

namespace lgraphics
{
    //-------------------------------------------------
    //---
    //--- VertexDeclarationRef
    //---
    //-------------------------------------------------
    VertexDeclarationRef::VertexDeclarationRef(const VertexDeclarationRef& rhs)
        :vertexSize_(rhs.vertexSize_)
        ,numElements_(rhs.numElements_)
        ,declaration_(rhs.declaration_)
    {
        // COMなので参照カウントを増やす
        if(declaration_ != NULL){
            declaration_->AddRef();
        }
    }

    void VertexDeclarationRef::destroy()
    {
        numElements_ = 0;
        SAFE_RELEASE(declaration_);
    }

    void VertexDeclarationRef::attach() const
    {
        GraphicsDeviceRef &device = Graphics::getDevice();
        device.setVertexDeclaration( const_cast<IDirect3DVertexDeclaration9*>(declaration_) );
    }

    bool VertexDeclarationRef::getDecl(VertexElement* decl)
    {
        LASSERT(declaration_ != NULL);
        HRESULT hr = declaration_->GetDeclaration((D3DVERTEXELEMENT9*)decl, &numElements_);
        return SUCCEEDED(hr);
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

        elements_[count_].stream_ = stream;
        elements_[count_].offset_ = offset;
        elements_[count_].type_ = static_cast<u8>(type);
        elements_[count_].method_ = static_cast<u8>(method);
        elements_[count_].usage_ = static_cast<u8>(usage);
        elements_[count_].usageIndex_ = usageIndex;

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
            sizeof(f64),
        };
        vertexSize_ += (stream==0xFFU)? 0 : bytes[type];
        return bytes[type];
    }

    void VertexDeclCreator::end(VertexDeclarationRef& declaration)
    {
        LASSERT((count_+1) == size_);

        declEnd();

        GraphicsDeviceRef &device = Graphics::getDevice();

        IDirect3DVertexDeclaration9 *decl = device.createVertexDeclaration( reinterpret_cast<D3DVERTEXELEMENT9*>(elements_) );
        if(decl == NULL){
            return;
        }

        VertexDeclarationRef tmp(vertexSize_, size_, decl);
        declaration.swap(tmp);
    }
}
