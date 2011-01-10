#ifndef INC_PMDUTIL_H__
#define INC_PMDUTIL_H__
/**
@file PmdUtil.h
@author t-sakai
@date 2010/07/04 create
*/
#include <vector>
#include <lcore/lcore.h>
#include "Pmd.h"

namespace lgraphics
{
    class Geometry;
}

namespace pmd
{
    typedef std::vector<lgraphics::Geometry*> GeomPtrVector;
    typedef std::vector<pmd::Vertex> PmdVertexVector;
    typedef std::vector<u16> IndexVector;

    void seperateGeometry(
        lcore::u32 numVertices,
        pmd::Vertex* pmdVertices,
        PmdVertexVector& vertices,
        lcore::u32 numIndices,
        lcore::u16* pmdIndices,
        lcore::u32 numMaterials,
        pmd::Material* materials,
        GeomPtrVector& retGeometries,
        u32 maxBones);
}
#endif //INC_PMDUTIL_H__
