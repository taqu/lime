#ifndef INC_LRENDER_LOADERPLY_H__
#define INC_LRENDER_LOADERPLY_H__
/**
@file LoaderPly.h
@author t-sakai
@date 2010/01/31 create
*/

namespace lrender
{
    class Mesh;

    Mesh* loadPly(const char* filename);
}

#endif //INC_LRENDER_LOADERPLY_H__
