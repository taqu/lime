/**
@file Geometry.cpp
@author t-sakai
@date 2009/08/10
*/
#include "Geometry.h"

namespace lscene
{
    //----------------------------------------------------
    //---
    //--- Geometry
    //---
    //----------------------------------------------------
    Geometry::Geometry()
        :shaderVS_(NULL)
        ,shaderPS_(NULL)
        ,indexOffset_(0)
        ,numIndices_(0)
        ,count_(0)
        ,materialIndex_(0xFFU)
        ,numBonesInPalette_(0)
        ,geometryFlags_(0)
    {
    }

    Geometry::Geometry(
        u16 count,
        u32 indexOffset,
        u8 materialIndex)
        :shaderVS_(NULL)
        ,shaderPS_(NULL)
        ,indexOffset_(indexOffset)
        ,numIndices_(count*3)
        ,count_(count)
        ,materialIndex_(materialIndex)
        ,numBonesInPalette_(0)
        ,geometryFlags_(0)
    {
    }

    Geometry::Geometry(
        lgraphics::GeometryBuffer::pointer& geomBuffer,
        u16 count,
        u32 indexOffset,
        u8 materialIndex)
        :geomBuffer_(geomBuffer)
        ,shaderVS_(NULL)
        ,shaderPS_(NULL)
        ,indexOffset_(indexOffset)
        ,numIndices_(count*3)
        ,count_(count)
        ,materialIndex_(materialIndex)
        ,numBonesInPalette_(0)
        ,geometryFlags_(0)
    {
    }

    Geometry::~Geometry()
    {
    }

    void Geometry::swap(Geometry& rhs)
    {
        geomBuffer_.swap(rhs.geomBuffer_);
        lcore::swap(shaderVS_, rhs.shaderVS_);
        lcore::swap(shaderPS_, rhs.shaderPS_);
        lcore::swap(indexOffset_, rhs.indexOffset_);
        lcore::swap(numIndices_, rhs.numIndices_);
        lcore::swap(count_, rhs.count_);
        lcore::swap(materialIndex_, rhs.materialIndex_);
        geometryFlags_.swap(rhs.geometryFlags_);
    }

}
