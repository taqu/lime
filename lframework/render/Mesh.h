#ifndef INC_LFRAMEWORK_MESH_H_
#define INC_LFRAMEWORK_MESH_H_
/**
@file Mesh.h
@author t-sakai
@date 2016/11/23 create
*/
#include "../lframework.h"
#include <lgraphics/Enumerations.h>
#include <lmath/geometry/Sphere.h>

namespace lfw
{
    struct ShaderSet;

    class Geometry;
    class Material;

    class Mesh
    {
    public:
        Mesh();
        ~Mesh();

        void create(
            lgfx::Primitive type,
            u32 indexOffset,
            u32 numIndices,
            Geometry* geometry,
            Material* material,
            const lmath::Sphere& sphere);

        inline lgfx::Primitive getType() const;
        inline u32 getNumPrimitives() const;
        inline u32 getIndexOffset() const;
        inline u32 getNumIndices() const;

        inline const Geometry* getGeometry() const;
        inline Geometry* getGeometry();
        inline const Material* getMaterial() const;
        inline Material* getMaterial();
        inline ShaderSet* getShaderSet();
        inline void setShaderSet(ShaderSet* shaderSet);

        inline void setSphere(const lmath::Sphere& s);
        inline const lmath::Sphere& getSphere() const;

        //inline u32 getNumSkinningMatrices() const;
        //inline void setNumSkinningMatrices(u32 numMatrices);
        //inline const lmath::Matrix34* getSkinningMatrices();
        //inline void setSkinningMatrices(const lmath::Matrix34* matrices);

        bool hasBoneIndex() const;
        //bool hasSkinning() const;
        bool hasUV() const;

        /// ÉXÉèÉbÉv
        void swap(Mesh& rhs);
    private:
        Mesh(const Mesh&);
        Mesh& operator=(const Mesh&);

        static u32 calcNumPrimitives(lgfx::Primitive type, u32 numIndices);

        lgfx::Primitive type_;
        u32 numPrimitives_;
        u32 indexOffset_;
        u32 numIndices_;
        //u32 numSkinningMatrices_;

        Geometry* geometry_;
        Material* material_;
        ShaderSet* shaderSet_;
        //const lmath::Matrix34* skinningMatrices_;
        //const lmath::DualQuaternion* dualQuaternions_;

        lmath::Sphere sphere_;
    };

    inline lgfx::Primitive Mesh::getType() const
    {
        return type_;
    }

    inline u32 Mesh::getNumPrimitives() const
    {
        return numPrimitives_;
    }

    inline u32 Mesh::getIndexOffset() const
    {
        return indexOffset_;
    }

    inline u32 Mesh::getNumIndices() const
    {
        return numIndices_;
    }

    inline const Geometry* Mesh::getGeometry() const
    {
        return geometry_;
    }

    inline Geometry* Mesh::getGeometry()
    {
        return geometry_;
    }

    inline const Material* Mesh::getMaterial() const
    {
        return material_;
    }

    inline Material* Mesh::getMaterial()
    {
        return material_;
    }

    inline ShaderSet* Mesh::getShaderSet()
    {
        return shaderSet_;
    }

    inline void Mesh::setShaderSet(ShaderSet* shaderSet)
    {
        shaderSet_ = shaderSet;
    }

    inline void Mesh::setSphere(const lmath::Sphere& s)
    {
        sphere_ = s;
    }

    inline const lmath::Sphere& Mesh::getSphere() const
    {
        return sphere_;
    }

    //inline u32 Mesh::getNumSkinningMatrices() const
    //{
    //    return numSkinningMatrices_;
    //}

    //inline void Mesh::setNumSkinningMatrices(u32 numMatrices)
    //{
    //    numSkinningMatrices_ = numMatrices;
    //}

    //inline const lmath::Matrix34* Mesh::getSkinningMatrices()
    //{
    //    return skinningMatrices_;
    //}

    //inline void Mesh::setSkinningMatrices(const lmath::Matrix34* matrices)
    //{
    //    skinningMatrices_ = matrices;
    //}
}
#endif //INC_LFRAMEWORK_MESH_H_
