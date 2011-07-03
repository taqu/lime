/**
@file GeometryBuffer.cpp
@author t-sakai
@date 2010/01/08 create

*/

#include "GeometryBuffer.h"

namespace lrender
{
    using namespace lgraphics;

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

    GeometryBuffer::GeometryBuffer(PrimitiveType type, VertexDeclaration& decl, VertexBufferUP& vb, IndexBufferUP& ib)
        :refCount_(0)
        ,type_(type)
    {
        decl_.swap(decl);
        vertexBuffer_.swap(vb);
        indexBuffer_.swap(ib);
    }

    GeometryBuffer::~GeometryBuffer()
    {
        refCount_ = 0;
    }

}

