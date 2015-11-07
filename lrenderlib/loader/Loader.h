#ifndef INC_LRENDER_LOADER_H__
#define INC_LRENDER_LOADER_H__
/**
@file Loader.h
@author t-sakai
@date 2013/05/13 create
*/
#include "../lrender.h"
#include "../shape/Shape.h"

namespace lrender
{
    class Loader
    {
    public:
        virtual ~Loader()
        {}

        virtual s32 loadFromFile(Shape::ShapeVector& shapes, const Char* filename) =0;

    protected:
        Loader()
        {}
    };
}
#endif //INC_LRENDER_LOADER_H__
