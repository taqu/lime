/**
@file IOGeometryBuffer.cpp
@author t-sakai
@date 2010/05/05 create
*/
#include "IOGeometryBuffer.h"
#include "IOVertexDeclaration.h"
#include "IOVertexBuffer.h"
#include "IOVertexBufferUP.h"
#include "IOIndexBuffer.h"

#include "SectionID.h"
#include "../api/VertexDeclarationRef.h"
#include "../api/VertexBufferRef.h"
#include "../api/IndexBufferRef.h"
#include "../api/GeometryBuffer.h"

namespace lgraphics
{
namespace io
{
    namespace
    {
        struct GeomBufferHeader
        {
            u32 id_;
            u32 type_;
        };
    }

    bool IOGeometryBuffer::read(lcore::istream& is, GeometryBuffer::pointer& buffer)
    {
#if defined(LIME_GLES1) //GLES1の場合、ユーザメモリの頂点バッファも作成
        GeomBufferHeader header;
        lcore::io::read(is, header);

        VertexDeclarationRef decl;
        VertexBufferUPRef vbUP;
        IndexBufferRef ib;

        IOVertexDeclaration::read(is, decl);
        IOVertexBufferUP::read(is, vbUP);
        IOIndexBuffer::read(is, ib);

        VertexBufferRef vb = VertexBuffer::create(vbUP, false, false);

        buffer =LIME_NEW GeometryBuffer(
            static_cast<PrimitiveType>(header.type_),
            decl,
            vb,
            vbUP,
            ib );

#else
        GeomBufferHeader header;
        lcore::io::read(is, header);

        VertexDeclarationRef decl;
        VertexBufferRef vb;
        IndexBufferRef ib;

        IOVertexDeclaration::read(is, decl);
        IOVertexBuffer::read(is, vb);
        IOIndexBuffer::read(is, ib);


        buffer =LIME_NEW GeometryBuffer(
            static_cast<PrimitiveType>(header.type_),
            decl,
            vb,
            ib );
#endif
        return true;
    }

    bool IOGeometryBuffer::write(lcore::ostream& os, GeometryBuffer::pointer& buffer)
    {
#if defined(LIME_GLES1) //GLES1の場合、ユーザメモリの頂点バッファも作成
        LASSERT(false); //整合性がとれないなあ
        GeomBufferHeader header;

        header.id_ = format::GeometryID;
        header.type_ = static_cast<u32>(buffer->getType());

        lcore::io::write(os, header);

        IOVertexDeclaration::write(os, buffer->getDecl());
        IOVertexBufferUP::write(os, buffer->getVertexBufferUP());
        IOIndexBuffer::write(os, buffer->getIndexBuffer());

#else
        GeomBufferHeader header;

        header.id_ = format::GeometryBufferID;
        header.type_ = static_cast<u32>(buffer->getType());

        lcore::io::write(os, header);

        IOVertexDeclaration::write(os, buffer->getDecl());
        IOVertexBuffer::write(os, buffer->getVertexBuffer(0));
        IOIndexBuffer::write(os, buffer->getIndexBuffer());
#endif
        return true;
    }
}
}
