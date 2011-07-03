#ifndef INC_LOADERPMD_H__
#define INC_LOADERPMD_H__
/**
@file LoaderPmd.h
@author t-sakai
@date 2010/05/17 create
*/
#include <lcore/lcore.h>
namespace lgraphics
{
    class AnimObject;
}

namespace pmd
{
    struct PMDIK;

    class LoaderPmd
    {
    public:
        struct IKPack
        {
            IKPack();
            ~IKPack();

            void release();

            lcore::u32 size_;
            PMDIK *iks_;
        };

        static lgraphics::AnimObject* load(const char* filepath, IKPack* ikPack = NULL, bool swapOrigin = false);
    };
}
#endif //INC_LOADERPMD_H__
