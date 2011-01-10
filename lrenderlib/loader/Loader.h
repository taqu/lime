#ifndef INC_LRENDER_LOADER_H__
#define INC_LRENDER_LOADER_H__
/**
@file Loader.h
@author t-sakai
@date 2010/01/07 create
*/
#include "../lrendercore.h"

namespace lrender
{
    class Mesh;

    class Loader
    {
    public:
        Loader();
        ~Loader();

        bool readFile(const char* filename);

        Mesh* releaseMesh();
    private:
        class Impl;
        Impl *impl_;
    };
}
#endif //INC_LRENDER_LOADER_H__
