/**
@file IOHeader.cpp
@author t-sakai
@date 2010/05/02 create
*/
#include "IOHeader.h"
#include "../Static.h"

namespace lscene
{
namespace io
{
    using namespace lframework;

    Header::Header()
        :magic_(ModelFileMagic)
        ,version_(ModelFileVersion)
        ,fileSize_(0)
        ,numMaterials_(0)
        ,numGeometries_(0)
        ,numBones_(0)
        ,numTrees_(0)
    {
    }

    Header::~Header()
    {
    }

    void Header::set(
            u32 fileSize,
            u16 numMaterials,
            u16 numGeometries,
            u16 numBones,
            u16 numTrees)
    {
        fileSize_ = fileSize;
        numMaterials_ = numMaterials;
        numGeometries_ = numGeometries;
        numBones_ = numBones;
        numTrees_ = numTrees;
    }
}
}
