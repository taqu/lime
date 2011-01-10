/**
@file GeometryBuffer.cpp
@author t-sakai
@date 2010/07/10 create
*/
#include "GeometryBuffer.h"

namespace lgraphics
{
    //----------------------------------------------------
    //---
    //--- GeometryBuffer
    //---
    //----------------------------------------------------
    GeometryBuffer::GeometryBuffer()
        :refCount_(0)
        ,type_(lgraphics::Primitive_TriangleList)
    {
    }

    GeometryBuffer::GeometryBuffer(PrimitiveType type, const VertexDeclarationRef& decl, const VertexBufferRef& vb, const IndexBufferRef& ib)
        :refCount_(0)
        ,type_(type)
        ,decl_(decl)
        ,vertexBuffer_(vb)
        ,indexBuffer_(ib)
    {
    }

    GeometryBuffer::~GeometryBuffer()
    {
        refCount_ = 0;
    }

    void GeometryBuffer::attach()
    {
        vertexBuffer_.attach();
        if(indexBuffer_.valid()){
            indexBuffer_.attach();
        }

        decl_.attach();
    }

    void GeometryBuffer::attach(u32 /*stride*/)
    {
        vertexBuffer_.attach();
        if(indexBuffer_.valid()){
            indexBuffer_.attach();
        }

        decl_.attach();
    }

    void GeometryBuffer::detach()
    {
        decl_.detach();
        indexBuffer_.detach();
        vertexBuffer_.detach();
    }

}
