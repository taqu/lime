/**
@file IOGeometry.cpp
@author t-sakai
@date 2010/05/04 create
*/
#include "IOGeometry.h"

#include <lgraphics/api/ShaderKey.h>
#include <lgraphics/io/IOShaderKey.h>

#include "../IOUtil.h"
#include "../SectionID.h"
#include "Geometry.h"
#include "ShaderBase.h"

using namespace lframework;

namespace lscene
{
    namespace
    {
        struct GeomHeader
        {
            u32 id_;
            u32 indexOffset_;
            u32 numIndices_;
            u16 primCount_;
            u8 materialIndex_;
            u8 numBonesInPalette_;
            u32 flags_;
        };
    }

    bool IOGeometry::read(lcore::istream& is, Geometry& geom)
    {
        GeomHeader header;
        lcore::io::read(is, header);


        Geometry tmp(header.primCount_, header.indexOffset_, header.materialIndex_);
        tmp.getFlags().set(header.flags_);

        geom.swap(tmp);

        return true;
    }

    bool IOGeometry::write(lcore::ostream& os, Geometry& geom)
    {
        GeomHeader header;

        header.id_ = GeometryID;
        header.primCount_ = static_cast<u16>( geom.getPrimitiveCount() );
        header.indexOffset_ = geom.getIndexOffset();
        header.numIndices_ = geom.getNumIndices();
        header.materialIndex_ = geom.getMaterialIndex();
        header.flags_ = geom.getFlags().get();

        lcore::io::write(os, header);

        return true;
    }
}
