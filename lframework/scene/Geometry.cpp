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
        ,geometryFlags_(0)
        ,primitiveCount_(0)
    {
        data_[Index_Material] = 0xFFU;
        data_[Index_NumBones] = 0;
    }

    Geometry::Geometry(
        u32 primCount,
        u32 indexOffset,
        u8 materialIndex)
        :shaderVS_(NULL)
        ,shaderPS_(NULL)
        ,indexOffset_(indexOffset)
        ,numIndices_(primCount*3)
        ,geometryFlags_(0)
        ,primitiveCount_(primCount)
    {
        data_[Index_Material] = materialIndex;
        data_[Index_NumBones] = 0;
    }

    Geometry::Geometry(
        lgraphics::GeometryBuffer::pointer& geomBuffer,
        u32 primCount,
        u32 indexOffset,
        u8 materialIndex)
        :geomBuffer_(geomBuffer)
        ,shaderVS_(NULL)
        ,shaderPS_(NULL)
        ,indexOffset_(indexOffset)
        ,numIndices_(primCount*3)
        ,geometryFlags_(0)
        ,primitiveCount_(primCount)
    {
        data_[Index_Material] = materialIndex;
        data_[Index_NumBones] = 0;
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
        geometryFlags_.swap(rhs.geometryFlags_);
        lcore::swap(primitiveCount_, rhs.primitiveCount_);
        lcore::swap(data_[Index_Material], rhs.data_[Index_Material]);
    }

}
