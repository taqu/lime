#ifndef INC_LOAD_GEOMETRY_H__
#define INC_LOAD_GEOMETRY_H__
/**
@file load_geometry.h
@author t-sakai
@date 2016/11/27 create
*/
#include <lframework/resource/load.h>
#include <lframework/resource/load_geometry.h>
#include <vector>
#include <string>
#include <lmath/geometry/Sphere.h>

namespace lfw
{
    static const s32 TriangleVertices = 3;
    static const s32 DimPosition = 3;
    static const s32 DimNormal = 3;
    static const s32 DimTangent = 3;
    static const s32 DimBinormal = 3;
    static const s32 DimColor = 1;
    static const s32 DimUV = 2;
    static const s32 DimBone = 2;

    static const s32 WorkDimBone = 8;

    struct Bone
    {
        bool hasEmpty() const;
        void setInvalid();
        void setZero();
        void correct();

        s32 count_;
        u16 indices_[WorkDimBone];
        f32 weights_[WorkDimBone];
    };

    class GeometryCVT : public LoadGeometry
    {
    public:
        typedef std::vector<f32> FloatVector;
        typedef std::vector<u16> U16Vector;
        typedef std::vector<u32> U32Vector;
        typedef std::vector<u16> IndexVector;
        typedef std::vector<Bone> BoneVector;

        FloatVector positions_;
        FloatVector normals_;
        FloatVector tangents_;
        FloatVector binormals_;
        U32Vector colors_;
        U16Vector texcoords_;
        BoneVector bones_;

        IndexVector indices_;
        //std::string name_;
    };
}
#endif //INC_LOAD_GEOMETRY_H__
