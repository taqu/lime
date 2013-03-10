#ifndef INC_CONVERTER_H__
#define INC_CONVERTER_H__
/**
@file converter.h
@author t-sakai
@date 2012/10/30 create

*/
#include <fbxsdk.h>

#include <vector>

#include "load.h"
#include "load_node.h"
#include "load_mesh.h"
#include "load_geometry.h"
#include "load_material.h"

namespace load
{
    class Importer;
    class Scene;

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
        struct SubMesh
        {
            SubMesh()
                :indexOffset_(0)
                ,numTriangles_(0)
                ,materialIndex_(-1)
            {}

            s32 indexOffset_;
            s32 numTriangles_;
            s32 materialIndex_;
        };

        typedef std::vector<Node> NodeVector;
        typedef std::vector<Mesh> MeshVector;
        typedef std::vector<GeometryCVT> GeometryVector;
        typedef std::vector<MaterialCVT> MaterialVector;
        typedef std::vector<SubMesh> SubMeshVector;

        static const s32 TriangleVertices = 3;
        static const s32 VertexStride = 3;
        static const s32 NormalStride = 3;
        static const s32 UVStride = 2;

        Converter();
        ~Converter();

        bool process(FbxScene* scene);
        bool out(const Char* path);

        void traverseNode(FbxNode* node, s32 index);
        void pushNode(FbxNode* node, u8 parent);
        void pushMesh(Node& dst, FbxNode* src, u8 parent);
        s32 pushMaterial(FbxSurfaceMaterial* src, FbxGeometryBase* geometry);

        const Char* getAttributeType(const FbxNodeAttribute* attr);
        //s32 findGeometry(const Char* name);
        bool findMesh(u16& meshStartIndex, u16& numMeshes, s32 geometryIndex);

        void calcSphere(Mesh& mesh);

        NodeVector nodes_;
        GeometryVector geometries_;
        MeshVector meshes_;
        MaterialVector materials_;
    };
}
#endif //INC_CONVERTER_H__
