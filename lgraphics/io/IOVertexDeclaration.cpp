/**
@file IOVertexDeclaration.cpp
@author t-sakai
@date 2010/05/06 create
*/
#include "IOVertexDeclaration.h"
#include "../api/VertexDeclarationRef.h"
#include "SectionID.h"

namespace lgraphics
{
namespace io
{
    namespace
    {
        struct DeclHeader
        {
            u32 id_;
            u32 vertexSize_;
            u32 numElements_;
        };
    }

    bool IOVertexDeclaration::read(lcore::istream& is, VertexDeclarationRef& decl)
    {
        DeclHeader declHeader;
        lcore::io::read(is, declHeader);
        VertexElement elem[VertexDeclarationRef::MAX_ELEMENTS];
        lcore::io::read(is, elem, sizeof(VertexElement)*declHeader.numElements_);

        VertexDeclCreator creator(declHeader.numElements_);
        for(u32 i=0; i<declHeader.numElements_; ++i){

            creator.add( elem[i].stream_,
                elem[i].offset_,
                static_cast<DeclType>(elem[i].getType() ),
                static_cast<DeclMethod>(elem[i].getMethod() ),
                static_cast<DeclUsage>(elem[i].getUsage() ),
                elem[i].getUsageIndex() );
        }

        creator.end(decl);
        return true;
    }


    bool IOVertexDeclaration::write(lcore::ostream& os, VertexDeclarationRef& decl)
    {
        VertexElement elem[VertexDeclarationRef::MAX_ELEMENTS];
        bool ref = decl.getDecl(elem);
        if(ref == false){
            return false;
        }

        DeclHeader declHeader;
        declHeader.id_ = format::VertexDeclarationID;
        declHeader.vertexSize_ = decl.getVertexSize();
        declHeader.numElements_ = decl.getNumElements()-1; //終端を引く

        lcore::io::write(os, declHeader);

        for(u32 i=0; i<declHeader.numElements_; ++i){
            lcore::io::write(os, elem[i]);
        }
        return true;
    }
}
}
