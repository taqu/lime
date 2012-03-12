#ifndef INC_LSCENE_IOHEADER_H__
#define INC_LSCENE_IOHEADER_H__
/**
@file IOHeader.h
@author t-sakai
@date 2010/05/02 create
*/
#include <lframework/scene/lscene.h>

namespace lscene
{
namespace io
{
    class Header
    {
    public:
        Header();
        ~Header();

        void set(
            u32 fileSize,
            u16 numMaterials,
            u16 numGeometries,
            u16 numBones,
            u16 numTrees);
    private:
        s32 magic_;
        s32 version_;
        u32 fileSize_;
        u16 numMaterials_;
        u16 numGeometries_;
        u16 numBones_;
        u16 numTrees_;
    };
}
}
#endif //INC_LSCENE_IOHEADER_H__
