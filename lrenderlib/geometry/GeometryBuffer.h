#ifndef INC_LRENDER_GEOMETRYBUFFER_H__
#define INC_LRENDER_GEOMETRYBUFFER_H__
/**
@file GeometryBuffer.h
@author t-sakai
@date 2010/01/08 create

*/
#include "../lrendercore.h"
#include <lcore/smart_ptr/intrusive_ptr.h>

#include <lgraphics/api/VertexBuffer.h>
#include <lgraphics/api/IndexBuffer.h>
#include <lgraphics/api/VertexDeclaration.h>

namespace lgraphics
{
    class VertexDeclaration;
    class VertexBufferUP;
    class IndexBufferUP;
}

namespace lrender
{
    class GeometryBuffer
    {
    public:
        typedef smart_ptr::intrusive_ptr<GeometryBuffer> pointer;

        GeometryBuffer();
        GeometryBuffer(
            lgraphics::PrimitiveType type,
            lgraphics::VertexDeclaration& decl,
            lgraphics::VertexBufferUP& vb,
            lgraphics::IndexBufferUP& ib);

        ~GeometryBuffer();

        lgraphics::PrimitiveType getType() const{ return type_;}

        lgraphics::VertexBufferUP& getVertexBuffer(){ return vertexBuffer_;}
        const lgraphics::VertexBufferUP& getVertexBuffer() const{ return vertexBuffer_;}

        lgraphics::IndexBufferUP& getIndexBuffer(){ return indexBuffer_;}
        const lgraphics::IndexBufferUP& getIndexBuffer() const{ return indexBuffer_;}

        lgraphics::VertexDeclaration& getDecl() { return decl_;}


        bool hasIndex() const{ return indexBuffer_.isNull() == false;}
    private:
        friend inline void intrusive_ptr_addref(GeometryBuffer* ptr);
        friend inline void intrusive_ptr_release(GeometryBuffer* ptr);

        void addRef()
        {
            ++refCount_;
        }
        void release()
        {
            if(--refCount_ == 0){
                LIME_DELETE_NONULL this;
            }
        }

        s32 refCount_;

        lgraphics::PrimitiveType type_;
        lgraphics::VertexDeclaration decl_;
        lgraphics::VertexBufferUP vertexBuffer_;
        lgraphics::IndexBufferUP indexBuffer_;
    };

    inline void intrusive_ptr_addref(GeometryBuffer* ptr)
    {
        ptr->addRef();
    }

    inline void intrusive_ptr_release(GeometryBuffer* ptr)
    {
        ptr->release();
    }
}

#endif //INC_LRENDER_GEOMETRYBUFFER_H__
