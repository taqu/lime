#ifndef INC_CONVERTER_H__
#define INC_CONVERTER_H__
/**
@file converter.h
@author t-sakai
@date 2012/10/30 create

*/
#include <fbxsdk.h>

#include <vector>
#include <lmath/Quaternion.h>

#include "load.h"
#include "load_node.h"
#include "load_mesh.h"
#include "load_geometry.h"
#include "load_material.h"
#include "load_joint.h"
#include "load_texture.h"

namespace load
{
    class Importer;
    class Scene;

    class JointPoseWithFrame
    {
    public:
        u32 frameNo_;
        lmath::Vector3 translation_;
        lmath::Quaternion rotation_;
    };

    class JointAnimationCVT
    {
    public:
        typedef std::vector<JointPoseWithFrame> JointPoseVector;

        JointAnimationCVT()
        {
            name_[0] = '\0';
        }

        void add(u32 frame, const lmath::Vector3& translation, const lmath::Quaternion& rotation);

        Char name_[MaxNameSize];
        JointPoseVector poses_;
    };

    class AnimationClipCVT
    {
    public:
        typedef std::vector<JointAnimationCVT> JointAnimationVector;

        AnimationClipCVT()
            :lastFrame_(0.0f)
        {
            name_[0] = '\0';
        }

        void calcLastFrame();

        Char name_[MaxNameSize];
        f32 lastFrame_;

        JointAnimationVector jointAnims_;
    };


    class Manager
    {
    public:
        Manager();
        ~Manager();

        FbxIOSettings* getIOSettings(){ return manager_->GetIOSettings();}

        bool valid() const{ return NULL != manager_;}

        bool initialize(Importer& importer, const char* filename);

        bool import(Scene& scene, Importer& importer);
    private:
        Manager(const Manager&);
        Manager& operator=(const Manager&);

        FbxManager* manager_;
    };

    class Importer
    {
    public:
        Importer();
        Importer(FbxImporter* importer);
        ~Importer();

        bool valid() const{ return NULL != importer_;}

        void swap(Importer& rhs)
        {
            lcore::swap(importer_, rhs.importer_);
        }
    private:
        friend class Manager;

        Importer(const Importer&);
        Importer& operator=(const Importer&);

        FbxImporter* importer_;
    };

    class Scene
    {
    public:
        Scene();
        Scene(FbxScene* scene);
        ~Scene();

        bool valid() const{ return NULL != scene_;}

        void swap(Scene& rhs)
        {
            lcore::swap(scene_, rhs.scene_);
        }


        FbxScene* scene_;
    };


    class Converter
    {
    public:
        typedef std::vector<u16> U16Vector;
        typedef std::vector<Node> NodeVector;
        typedef std::vector<Mesh> MeshVector;
        typedef std::vector<GeometryCVT> GeometryVector;
        typedef std::vector<Material> MaterialVector;
        typedef std::vector<Joint> JointVector;
        typedef std::vector<Texture> TextureVector;
        typedef std::vector<FbxNode*> FbxNodeVector;
        typedef std::vector<FbxTexture*> FbxTextureVector;

        static const s32 TriangleVertices = 3;
        static const s32 DimPosition = 3;
        static const s32 DimNormal = 3;
        static const s32 DimTangent = 3;
        static const s32 DimBinormal = 3;
        static const s32 DimColor = 1;
        static const s32 DimUV = 2;
        static const s32 DimBone = 2;
        static const s32 DimBoneWeight = 2;

        Converter();
        ~Converter();

        bool process(FbxScene* scene);
        bool out(const Char* path);
        bool outAnimation(const Char* path);

        static bool acceptNodeAttribute(const FbxNodeAttribute* attribute);

        void traverseSkeletonNode(FbxNode* root);

        void traverseNode(FbxNode* node, s32 index);
        void pushNode(FbxNode* node, u8 parent);
        s16 pushGeometry(FbxNode* node);

        void getNormals(GeometryCVT& geometry, FbxMesh* mesh, FbxLayerElementNormal* elementNormal);
        void getTangents(GeometryCVT& geometry, FbxMesh* mesh, FbxLayerElementTangent* elementTangent);
        void getBinormals(GeometryCVT& geometry, FbxMesh* mesh, FbxLayerElementBinormal* elementBinormal);
        void getColors(GeometryCVT& geometry, FbxMesh* mesh, FbxLayerElementVertexColor* elementColor);
        void getUVs(GeometryCVT& geometry, FbxMesh* mesh, FbxGeometryElementUV* elementUV);
        u32 getSkinInfo(GeometryCVT& geometry, FbxSkin* skin);

        void pushMesh(Node& dst, FbxNode* src);
        void pushMaterial(FbxScene* scene);

        const Char* getAttributeType(const FbxNodeAttribute* attr);

        s32 findSkeletonNode(const FbxNode* node) const;
        s16 findTexture(const FbxTexture* texture) const;

        void calcSphere(Mesh& mesh);

        void setVertexProperty(GeometryCVT& geometry);

        void traverseAnimation(FbxScene* scene);
        void traverseAnimation(FbxAnimLayer* animLayer, FbxNode* node);
        void traverseAnimation(JointAnimationCVT& jointAnimation, FbxAnimLayer* animLayer, FbxNode* node);
        void pushAnimation(JointAnimationCVT& jointAnimation, FbxAnimCurve* animCurve, FbxNode* node);

        FbxNodeVector skeletonNodes_;
        FbxTextureVector fbxTextures_;
        NodeVector nodes_;
        GeometryVector geometries_;
        MeshVector meshes_;
        MaterialVector materials_;
        JointVector joints_;
        TextureVector textures_;
        AnimationClipCVT animationClip_;

        static void print(FbxScene* scene);

        static void print(FbxAnimStack* animStack, FbxNode* node);

        static void print(FbxAnimLayer* animLayer, FbxNode* node);

        static void printChannels(FbxAnimLayer* animLayer, FbxNode* node, void (*printCurve)(FbxAnimCurve* curve), void (*printListCurve)(FbxAnimCurve* curve, FbxProperty* prop));

        static void printCurve(FbxAnimCurve* curve);
        static void printListCurve(FbxAnimCurve* curve, FbxProperty* prop);
    };
}
#endif //INC_CONVERTER_H__
