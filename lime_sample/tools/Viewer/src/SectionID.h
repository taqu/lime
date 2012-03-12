#ifndef INC_LFRAMEWORK_SECTIONID_H__
#define INC_LFRAMEWORK_SECTIONID_H__
/**
@file SectionID.h
@author t-sakai
@date 2010/05/02 create
*/
#include <lframework/lframework.h>

namespace lframework
{
#define MAKE_ID(name, id)\
    static const unsigned int name = id;

    MAKE_ID(ObjectID, LIME_MAKE_FOURCC('o', 'b', 'j', ' '))

    MAKE_ID(GeometryID, LIME_MAKE_FOURCC('g', 'e', 'o', 'm'))
    MAKE_ID(MaterialID, LIME_MAKE_FOURCC('m', 'a', 't', 'e'))
    MAKE_ID(AnimTreeID, LIME_MAKE_FOURCC('a', 't', 'r', 'e'))

    // Animation Database
    MAKE_ID(AnimDatabaseID, LIME_MAKE_FOURCC('a', 'n', 'i', 'm'))
#undef MAKE_ID
}
#endif //INC_LFRAMEWORK_SECTIONID_H__
