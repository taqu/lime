#ifndef INC_CONVERTER_H__
#define INC_CONVERTER_H__
/**
@file converter.h
@author t-sakai
@date 2012/10/30 create

*/
#include <lcore/CLibrary.h>
#include <fbxsdk.h>

#include <vector>
#include <lcore/liostream.h>
#include <lmath/Quaternion.h>

#include <lframework/scene/load/load.h>
#include <lframework/scene/load/load_node.h>
#include <lframework/scene/load/load_mesh.h>
#include <lframework/scene/load/load_geometry.h>
#include <lframework/scene/load/load_material.h>
#include <lframework/scene/load/load_joint.h>
#include <lframework/scene/load/load_texture.h>
#include <lframework/scene/anim/JointPose.h>
#include "load_geometry.h"

namespace lscene
{
namespace lload
{
    class Importer;
    class Scene;

    using lscene::lanim::JointPoseWithTime;

    class JointAnimationCVT
    {
    public:
        typedef std::vector<JointPoseWithTime> JointPoseVector;

        JointAnimationCVT()
        {
            name_[0] = '\0';
        }

        void add(f32 time, const lmath::Vector3& translation, const lmath::Quaternion& rotation);

        Char name_[MaxNameSize];
        JointPoseVector poses_;
    };

    class AnimationClipCVT
    {
    public:
        typedef std::vector<JointAnimationCVT> JointAnimationVector;

        AnimationClipCVT()
            :lastTime_(0.0f)
        {
            name_[0] = '\0';
        }

        void calcLastTime();

        Char name_[MaxNameSize];
        f32 lastTime_;

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

        Converter();
        ~Converter();

        bool process(FbxScene* scene);
        bool out(const Char* path);
        bool outAnimation(const Char* path);

        static bool acceptNodeAttribute(const FbxNodeAttribute* attribute);

        void traverseSkeletonNode(FbxNode* root);
        void traverseJoint(FbxNode* root);
        void traverseNode(FbxNode* node, s32 index);

        void pushNode(FbxNode* node, u8 parent);
        s16 pushGeometry(FbxNode* node);
        void pushJoint(FbxNode* node);

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
        //s32 findSkeletonLink(const FbxSkin* skin, const FbxNode* node) const;
        s16 findTexture(const FbxTexture* texture) const;

        void calcSphere(Mesh& mesh);

        void setVertexProperty(GeometryCVT& geometry);

        void traverseAnimation();
        void traverseAnimation(FbxAnimStack* animStack, FbxNode* node);
        void traverseAnimation(FbxAnimLayer* animLayer, FbxNode* node);
        void traverseAnimation(JointAnimationCVT& jointAnimation, FbxAnimLayer* animLayer, FbxNode* node);
        void pushAnimation(JointAnimationCVT& jointAnimation, FbxAnimCurve* animCurve, FbxNode* node);

        void pushAnimation2(JointAnimationCVT& jointAnimation, FbxAnimCurve* animCurve, FbxNode* node);

        void pushAnimation3(
            JointAnimationCVT& jointAnimation,
            FbxAnimCurve* animCurve,
            FbxNode* node,
            FbxAnimCurve* animTransXCurve,
            FbxAnimCurve* animTransYCurve,
            FbxAnimCurve* animTransZCurve,
            FbxAnimCurve* animRotXCurve,
            FbxAnimCurve* animRotYCurve,
            FbxAnimCurve* animRotZCurve);

        bool forceDDS_; //テクスチャの拡張子をddsへ変換

        FbxScene* scene_;
        FbxNodeVector skeletonNodes_;
        FbxTextureVector fbxTextures_;
        NodeVector nodes_;
        GeometryVector geometries_;
        MeshVector meshes_;
        MaterialVector materials_;
        JointVector joints_;
        TextureVector textures_;
        AnimationClipCVT animationClip_;
        FbxNode* boneRootNode_;

        lcore::ofstream log_;

        static void print(FbxScene* scene);

        static void print(FbxAnimStack* animStack, FbxNode* node);

        static void print(FbxAnimLayer* animLayer, FbxNode* node);

        static void printChannels(FbxAnimLayer* animLayer, FbxNode* node, void (*printCurve)(FbxAnimCurve* curve), void (*printListCurve)(FbxAnimCurve* curve, FbxProperty* prop));

        static void printCurve(FbxAnimCurve* curve);
        static void printListCurve(FbxAnimCurve* curve, FbxProperty* prop);
    };
}
}
#endif //INC_CONVERTER_H__
