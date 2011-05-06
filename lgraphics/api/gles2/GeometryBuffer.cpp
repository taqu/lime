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
        ,numStreams_(0)
    {
    }

    GeometryBuffer::GeometryBuffer(PrimitiveType type, const VertexDeclarationRef& decl, const VertexBufferRef& vb, const IndexBufferRef& ib)
        :refCount_(0)
        ,type_(type)
        ,numStreams_(1)
        ,decl_(decl)
        ,indexBuffer_(ib)
    {
        vertexBuffer_[0] = vb;
    }

    GeometryBuffer::~GeometryBuffer()
    {
        refCount_ = 0;
    }

    bool GeometryBuffer::addVertexBufferStream(VertexBufferRef& vb)
    {
        if(numStreams_>=LIME_MAX_VERTEX_STREAMS){
            return false;
        }
        vertexBuffer_[numStreams_] = vb;
        ++numStreams_;
        return true;
    }

    void GeometryBuffer::attach()
    {
        for(u32 i=0; i<numStreams_; ++i){
            vertexBuffer_[i].attach();
            decl_.attach(i);
        }

        if(indexBuffer_.valid()){
            indexBuffer_.attach();
        }
    }

    void GeometryBuffer::detach()
    {
        for(u32 i=0; i<numStreams_; ++i){
            decl_.detach(i);
        }

        //indexBuffer_.detach();
        //vertexBuffer_.detach();
    }

}
