/**
@file Mesh.cpp
@author t-sakai
@date 2016/11/23 create
*/
#include "render/Mesh.h"
#include "render/Geometry.h"
#include "resource/load.h"

namespace lfw
{
    Mesh::Mesh()
        :type_(lgfx::Primitive_TriangleList)
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
        lgfx::Primitive type,
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
        return 0 != (geometry_->getVFlag() & (VElem_Bone | VElem_BoneWeight));
    }

    bool Mesh::hasUV() const
    {
        LASSERT(NULL != geometry_);
        return 0 != (geometry_->getVFlag() & VElem_Texcoord);
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

    u32 Mesh::calcNumPrimitives(lgfx::Primitive type, u32 numIndices)
    {
        u32 numPrimitives = 0;
        switch(type)
        {
        case lgfx::Primitive_PointList:
            numPrimitives = numIndices;
            break;

        case lgfx::Primitive_LineList:
            numPrimitives = numIndices/2;
            break;

        case lgfx::Primitive_LineStrip:
            numPrimitives = (numIndices<1)? 0 : numIndices - 1;
            break;

        case lgfx::Primitive_TriangleList:
            numPrimitives = numIndices/3;
            break;

        case lgfx::Primitive_TriangleStrip:
            numPrimitives = (numIndices<2)? 0 : numIndices - 2;
            break;

        case lgfx::Primitive_PatchList1:
        case lgfx::Primitive_PatchList2:
        case lgfx::Primitive_PatchList3:
        case lgfx::Primitive_PatchList4:
        case lgfx::Primitive_PatchList5:
        case lgfx::Primitive_PatchList6:
        case lgfx::Primitive_PatchList7:
        case lgfx::Primitive_PatchList8:
        case lgfx::Primitive_PatchList9:
        case lgfx::Primitive_PatchList10:
        case lgfx::Primitive_PatchList11:
        case lgfx::Primitive_PatchList12:
        case lgfx::Primitive_PatchList13:
        case lgfx::Primitive_PatchList14:
        case lgfx::Primitive_PatchList15:
        case lgfx::Primitive_PatchList16:
        case lgfx::Primitive_PatchList17:
        case lgfx::Primitive_PatchList18:
        case lgfx::Primitive_PatchList19:
        case lgfx::Primitive_PatchList20:
        case lgfx::Primitive_PatchList21:
        case lgfx::Primitive_PatchList22:
        case lgfx::Primitive_PatchList23:
        case lgfx::Primitive_PatchList24:
        case lgfx::Primitive_PatchList25:
        case lgfx::Primitive_PatchList26:
        case lgfx::Primitive_PatchList27:
        case lgfx::Primitive_PatchList28:
        case lgfx::Primitive_PatchList29:
        case lgfx::Primitive_PatchList30:
        case lgfx::Primitive_PatchList31:
        case lgfx::Primitive_PatchList32:
            {
                u32 numPerPrimitive = (type-lgfx::Primitive_PatchList1)+1;
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
