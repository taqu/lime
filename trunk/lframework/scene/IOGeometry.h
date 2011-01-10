#ifndef INC_LSCENE_IOGEOMETRY_H__
#define INC_LSCENE_IOGEOMETRY_H__
/**
@file IOGeometry.h
@author t-sakai
@date 2010/05/04 create
*/
#include <lcore/liofwd.h>

namespace lscene
{
    class Geometry;

    class IOGeometry
    {
    public:
        IOGeometry(){}
        ~IOGeometry(){}

        static bool read(lcore::istream& is, Geometry& geom);
        static bool write(lcore::ostream& os, Geometry& geom);
    };
}
#endif //INC_LSCENE_IOGEOMETRY_H__
