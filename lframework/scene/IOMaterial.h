#ifndef INC_LSCENE_IOMATERIAL_H__
#define INC_LSCENE_IOMATERIAL_H__
/**
@file IOMaterial.h
@author t-sakai
@date 2010/05/06 create

*/
#include <lcore/liostream.h>

namespace lscene
{
    class Material;

    class IOMaterial
    {
    public:
        IOMaterial(){}
        ~IOMaterial(){}

        static bool read(lcore::istream& is, Material& material);
        static bool write(lcore::ostream& os, Material& material);

    };
}
#endif //INC_LSCENE_IOMATERIAL_H__
