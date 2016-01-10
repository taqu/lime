#ifndef INC_LRENDER_LOADEROBJ_H__
#define INC_LRENDER_LOADEROBJ_H__
/**
@file LoaderObj.h
@author t-sakai
@date 2015/09/14 create
*/
#include "Loader.h"
#include <lcore/liostream.h>
#include <lcore/Vector.h>
#include "../core/Buffer.h"
#include "../core/Triangle.h"
#include "../shape/Mesh.h"
#include "../shape/MeshInstance.h"
#include "../bsdf/BSDF.h"
#include "../texture/Texture.h"

namespace lrender
{
    class Color3;

    //------------------------------------------------------
    //---
    //--- LoaderObj
    //---
    //------------------------------------------------------
    class LoaderObj : public Loader
    {
    public:
        typedef lcore::vector_arena<MeshInstance::pointer> MeshInstanceVector;

        LoaderObj(bool adjustGeometryNormal);
        virtual ~LoaderObj();

        /**
        */
        virtual s32 loadFromFile(Shape::ShapeVector& shapes, const Char* filename);
    private:
        struct Vertex
        {
            s32 position_;
            s32 normal_;
            s32 texcoord_;
        };

        typedef lcore::vector_arena<Char> String;
        typedef lcore::vector_arena<Vector2, lcore::vector_arena_static_inc_size<1024> > Vector2Vector;
        typedef lcore::vector_arena<Vector3, lcore::vector_arena_static_inc_size<1024> > Vector3Vector;
        typedef lcore::vector_arena<Vector4, lcore::vector_arena_static_inc_size<1024> > Vector4Vector;
        typedef lcore::vector_arena<Triangle, lcore::vector_arena_static_inc_size<1024> > TriangleVector;
        typedef lcore::vector_arena<Vertex, lcore::vector_arena_static_inc_size<1024> > VertexVector;

        typedef lcore::vector_arena<BSDF::pointer> BSDFVector;

        enum Code
        {
            Code_None,
            Code_Position,
            Code_Normal,
            Code_Texcoord,
            Code_Face,
            Code_MtlLib,
            Code_UseMtl,
            Code_Object,
            Code_Group,
            Code_Smooth,
            Code_Comment,
            Code_EOF,
        };

        //------------------------------------------------------
        //---
        //--- LoaderMtl
        //---
        //------------------------------------------------------
        class LoaderMtl
        {
        public:
            enum CodeMtl
            {
                CodeMtl_None,
                CodeMtl_NewMtl,
                CodeMtl_Model,
                CodeMtl_Dissolve,
                CodeMtl_SpecularExponent,
                CodeMtl_RefracttionIndex,
                CodeMtl_Ambient,
                CodeMtl_Diffuse,
                CodeMtl_Specular,
                CodeMtl_Transmission,
                CodeMtl_Emission,
                CodeMtl_MapDiffuse,
                CodeMtl_MapBump,
                CodeMtl_Illumination,
                CodeMtl_EOF,
            };

            LoaderMtl();
            ~LoaderMtl();

            s32 loadFromFile(BSDFVector& bsdfs, const Char* directory, const Char* filename);

            void skip();
            bool getLine();
            void split();
            CodeMtl decodeLine();
            void load(f32& f);
            void load(Vector3& v);
            void load(Color3& rgb);

            Texture::pointer loadTexture(const Char* filename);
            Texture::pointer loadAsConstant();
            Texture::pointer loadAsImage();

            bool readLine(BSDFVector& brdfs);

            const Char* directory_;
            lcore::ifstream file_;
            String line_;
            lcore::vector_arena<Char*> elements_;
        };

        void skip();
        bool getLine();
        void split();
        Code decodeLine();
        s32 getFaceElements(f32 felems[4]);
        void getFaceVertex(Char* str, Vertex& v);

        bool readLine();

        void pushMesh();

        void generateNormals(BufferVector3& normals, const BufferTriangle& triangles, const BufferVector3& positions);
        void adjustGeometryNormals(BufferTriangle& triangles, const BufferVector3& normals, const BufferVector3& positions);

        MeshInstance::pointer& getLastMeshInstance()
        {
            return reinterpret_cast<MeshInstance::pointer&>((*shapes_)[shapes_->size()-1]);
        }

        bool adjustGeometryNormals_;
        Char* directory_;
        lcore::ifstream file_;
        String line_;
        lcore::vector_arena<Char*> elements_;

        Vector3Vector positions_;
        Vector3Vector normals_;
        Vector2Vector texcoords_;

        VertexVector vertices_;

        s32 lastNumTriangles_;
        Mesh::pointer mesh_;
        Shape::ShapeVector* shapes_;
        BSDFVector bsdfs_;
    };
}

#endif //INC_LRENDER_LOADEROBJ_H__
