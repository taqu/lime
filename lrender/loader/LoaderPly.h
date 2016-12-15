#ifndef INC_LRENDER_LOADERPLY_H__
#define INC_LRENDER_LOADERPLY_H__
/**
@file LoaderPly.h
@author t-sakai
@date 2013/05/13 create
*/
#include "Loader.h"

namespace lrender
{
    class LoaderPly : public Loader
    {
    public:
        LoaderPly();
        virtual ~LoaderPly();

        virtual s32 loadFromFile(Shape::ShapeVector& shapes, const Char* filename);
    private:
    };
}

#endif //INC_LRENDER_LOADERPLY_H__
