/**
@file Mesh.cpp
@author t-sakai
@date 2013/02/26 create

*/
#include "Mesh.h"
#include "Geometry.h"
#include "../load/load.h"

namespace lscene
{
namespace lrender
{
    Mesh::Mesh()
        :type_(lgraphics::Primitive_TriangleList)
        ,numPrimitives_(0)
        ,numIndices_(0)
        //,numSkinningMatrices_(0)
        ,geometry_(NULL)
        ,material_(NULL)
        ,shaderSet_(NULL)
        //,skinningMatrices_(NULL)
    {
    }

    Mesh::~Mesh()
    {
    }

    void Mesh::create(
        lgraphics::Primitive type,
        u32 indexOffset,
        u32 numIndices,
        Geometry* geometry,
        Material* material,
        const lmath::Sphere& sphere)
    {
        LASSERT(NULL != geometry);
        LASSERT(NULL != material);

        type_ = type;
        numPrimitives_ = calcNumPrimitives(type, numIndices);
        indexOffset_ = indexOffset;
        numIndices_ = numIndices;
        geometry_ = geometry;
        material_ = material;
        sphere_ = sphere;
    }

    bool Mesh::hasBoneIndex() const
    {
        LASSERT(NULL != geometry_);
        return 0 != (geometry_->getVFlag() & (lload::VElem_Bone | lload::VElem_BoneWeight));
    }

    bool Mesh::hasUV() const
    {
        LASSERT(NULL != geometry_);
        return 0 != (geometry_->getVFlag() & lload::VElem_Texcoord);
    }

    //bool Mesh::hasSkinning() const
    //{
    //    return (0<numSkinningMatrices_ && (geometry_->getVFlag() & lload::VElem_Bone));
    //}

    // ÉXÉèÉbÉv
    void Mesh::swap(Mesh& rhs)
    {
        lcore::swap(type_, rhs.type_);
        lcore::swap(numPrimitives_, rhs.numPrimitives_);
        lcore::swap(indexOffset_, rhs.indexOffset_);
        lcore::swap(numIndices_, rhs.numIndices_);
        //lcore::swap(numSkinningMatrices_, rhs.numSkinningMatrices_);

        lcore::swap(geometry_, rhs.geometry_);
        lcore::swap(material_, rhs.material_);
        lcore::swap(shaderSet_, rhs.shaderSet_);
        //lcore::swap(skinningMatrices_, rhs.skinningMatrices_);

        sphere_.swap(rhs.sphere_);
    }

    u32 Mesh::calcNumPrimitives(lgraphics::Primitive type, u32 numIndices)
    {
        u32 numPrimitives = 0;
        switch(type)
        {
        case lgraphics::Primitive_PointList:
            numPrimitives = numIndices;
            break;

        case lgraphics::Primitive_LineList:
            numPrimitives = numIndices/2;
            break;

        case lgraphics::Primitive_LineStrip:
            numPrimitives = (numIndices<1)? 0 : numIndices - 1;
            break;

        case lgraphics::Primitive_TriangleList:
            numPrimitives = numIndices/3;
            break;

        case lgraphics::Primitive_TriangleStrip:
            numPrimitives = (numIndices<2)? 0 : numIndices - 2;
            break;

        case lgraphics::Primitive_PatchList1:
        case lgraphics::Primitive_PatchList2:
        case lgraphics::Primitive_PatchList3:
        case lgraphics::Primitive_PatchList4:
        case lgraphics::Primitive_PatchList5:
        case lgraphics::Primitive_PatchList6:
        case lgraphics::Primitive_PatchList7:
        case lgraphics::Primitive_PatchList8:
        case lgraphics::Primitive_PatchList9:
        case lgraphics::Primitive_PatchList10:
        case lgraphics::Primitive_PatchList11:
        case lgraphics::Primitive_PatchList12:
        case lgraphics::Primitive_PatchList13:
        case lgraphics::Primitive_PatchList14:
        case lgraphics::Primitive_PatchList15:
        case lgraphics::Primitive_PatchList16:
        case lgraphics::Primitive_PatchList17:
        case lgraphics::Primitive_PatchList18:
        case lgraphics::Primitive_PatchList19:
        case lgraphics::Primitive_PatchList20:
        case lgraphics::Primitive_PatchList21:
        case lgraphics::Primitive_PatchList22:
        case lgraphics::Primitive_PatchList23:
        case lgraphics::Primitive_PatchList24:
        case lgraphics::Primitive_PatchList25:
        case lgraphics::Primitive_PatchList26:
        case lgraphics::Primitive_PatchList27:
        case lgraphics::Primitive_PatchList28:
        case lgraphics::Primitive_PatchList29:
        case lgraphics::Primitive_PatchList30:
        case lgraphics::Primitive_PatchList31:
        case lgraphics::Primitive_PatchList32:
            {
                u32 numPerPrimitive = (type-lgraphics::Primitive_PatchList1)+1;
                numPrimitives = numIndices/numPerPrimitive;
            }
            break;

        default:
            LASSERT(false);
            break;
        }
        return numPrimitives;
    }
}
}
