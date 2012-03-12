#ifndef INC_LGRAPHICS_SECTIONID_H__
#define INC_LGRAPHICS_SECTIONID_H__
/**
@file SectionID.h
@author t-sakai
@date 2010/05/02 create
*/
#include <lgraphics/lgraphicscore.h>

namespace lframework
{
namespace format
{
#define MAKE_ID(name, id)\
    static const unsigned int name = id;

    MAKE_ID(ObjectID, LIME_MAKE_FOURCC('o', 'b', 'j', ' '))

    MAKE_ID(GeometryID, LIME_MAKE_FOURCC('g', 'e', 'o', 'm'))
    MAKE_ID(MaterialID, LIME_MAKE_FOURCC('m', 'a', 't', 'e'))
    MAKE_ID(AnimTreeID, LIME_MAKE_FOURCC('a', 't', 'r', 'e'))

    MAKE_ID(GeometryBufferID, LIME_MAKE_FOURCC('g', 'b', 'u', 'f'))
    MAKE_ID(VertexDeclarationID, LIME_MAKE_FOURCC('v', 'd', 'e', 'c'))
    MAKE_ID(VertexBufferID, LIME_MAKE_FOURCC('v', 'b', 'u', 'f'))
    MAKE_ID(IndexBufferID, LIME_MAKE_FOURCC('i', 'b', 'u', 'f'))

    MAKE_ID(ShaderKeyID, LIME_MAKE_FOURCC('s', 'h', 'k', 'y'))

    MAKE_ID(SamplerStateID, LIME_MAKE_FOURCC('s', 'a', 'm', 'p'))
    MAKE_ID(RenderStateID, LIME_MAKE_FOURCC('r', 'e', 'n', 'd'))


    // Animation Database
    MAKE_ID(AnimDatabaseID, LIME_MAKE_FOURCC('a', 'n', 'i', 'm'))
#undef MAKE_ID
}
}
#endif //INC_LGRAPHICS_SECTIONID_H__
