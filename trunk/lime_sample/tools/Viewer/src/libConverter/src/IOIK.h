#ifndef INC_LTOOLS_IOIK_H__
#define INC_LTOOLS_IOIK_H__
/**
@file IOIK.h
@author t-sakai
@date 2010/07/29 create
*/
#include <lcore/lcore.h>
namespace ltools
{
    class IKChain;

    class IOIK
    {
    public:
        IOIK(){}
        ~IOIK(){}

        static bool read(lcore::istream& is, IKChain** ikChains, lcore::u32& numChains);
        static bool write(lcore::ostream& os, IKChain* ikChains, lcore::u32 numChains);
    };
}
#endif //INC_LTOOLS_IOIK_H__
