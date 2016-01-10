#ifndef INC_LOAD_LOADEROBJ_H__
#define INC_LOAD_LOADEROBJ_H__

#include <map>

#include <lcore/liostream.h>
#include <lcore/String.h>
#include <lcore/Vector.h>
#include <lcore/HashMap.h>
#include <lmath/lmath.h>
#include <lframework/scene/load/load.h>
#include <lframework/scene/load/load_material.h>
#include <lframework/scene/load/load_texture.h>

namespace lcore
{
    class istream;
}

namespace lscene
{
namespace lload
{
    class Geometry;

    class LoaderObj
    {
    public:
        struct Vertex
        {
            s32 position_;
            s32 texcoord_;
            s32 normal_;
            s16 type_;
            s16 material_;
        };

        struct Triangle
        {
            s32 indices_[3];
        };

        struct WorkMesh
        {
            s16 geometry_;
            s16 material_;
            s32 indexOffset_;
            s32 numIndices_;
        };

        LoaderObj(bool generateNormals=true);
        ~LoaderObj();

        bool load(const Char* filepath);
        void save(const Char* filepath, f32 scale);
    private:
        LoaderObj(const LoaderObj&);
        LoaderObj& operator=(const LoaderObj&);

        enum VType
        {
            VType_P =0,
            VType_PU,
            VType_PN,
            VType_PNU,
            VType_Num,
        };

        struct U16Vector2
        {
            u16 x_;
            u16 y_;
        };

        struct U16Vector4
        {
            u16 x_;
            u16 y_;
            u16 z_;
            u16 w_;
        };

        typedef lcore::vector_arena<Char> String;
        typedef lcore::vector_arena<lmath::Vector2, lcore::vector_arena_static_inc_size<1024> > Vector2Vector;
        typedef lcore::vector_arena<lmath::Vector3, lcore::vector_arena_static_inc_size<1024> > Vector3Vector;
        typedef lcore::vector_arena<lmath::Vector4, lcore::vector_arena_static_inc_size<1024> > Vector4Vector;
        typedef lcore::vector_arena<U16Vector2, lcore::vector_arena_static_inc_size<1024> > U16Vector2Vector;
        typedef lcore::vector_arena<U16Vector4, lcore::vector_arena_static_inc_size<1024> > U16Vector4Vector;
        typedef lcore::vector_arena<u16, lcore::vector_arena_static_inc_size<1024> > U16Vector;
        typedef lcore::vector_arena<s32, lcore::vector_arena_static_inc_size<1024> > S32Vector;

        typedef lcore::vector_arena<Triangle, lcore::vector_arena_static_inc_size<1024> > TriangleVector;
        typedef lcore::vector_arena<Vertex, lcore::vector_arena_static_inc_size<1024> > VertexVector;

        typedef lcore::vector_arena<lload::Texture> TextureVector;
        typedef lcore::vector_arena<WorkMesh> MeshVector;

        typedef std::map<s32, s32> S32Map;

        //typedef lcore::HopscotchHashMap<lcore::DynamicString, s32> NameToMaterialMap;
        //typedef lcore::HopscotchHashMap<lcore::DynamicString, s32> NameToTextureMap;

        struct WorkMaterial
        {
            static const s32 MaxNameSize = 256;
            Char name_[MaxNameSize];
            Material material_;

            void clear();
            void setName(const Char* name);
        };

        typedef lcore::vector_arena<WorkMaterial> MaterialVector;

        struct MaterialPack
        {
            MaterialVector materials_;
            TextureVector textures_;

            s32 findMaterialIndex(const Char* name) const;
            s32 findTextureIndex(const Char* name) const;
        };

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

            s32 loadFromFile(MaterialPack& materialPack, const Char* directory, const Char* filename);

            void skip();
            bool getLine();
            void split();
            CodeMtl decodeLine();
            void load(f32& f);
            void load(lmath::Vector3& v);
            void load(lmath::Vector4& v);

            Texture loadTexture(const Char* filename);

            bool readLine(MaterialPack& materialPack);

            const Char* directory_;
            lcore::ifstream file_;
            String line_;
            lcore::vector_arena<Char*> elements_;
            WorkMaterial* material_;
        };

        void skip();
        bool getLine();
        void split();
        Code decodeLine();
        s32 getFaceElements(f32 felems[4]);
        void getFaceVertex(Char* str, Vertex& v);

         bool readLine();

        void pushMesh();

        void setVertexProperty(Geometry& geometry);

        void push_back_scale(Vector3Vector& v, const lmath::Vector3& p, f32 scale);
        void push_back(U16Vector4Vector& v, const lmath::Vector3& n);
        void push_back(U16Vector2Vector& v, const lmath::Vector2& t);

        bool generateNormals_;
        Char* directory_;
        lcore::ifstream file_;
        String line_;
        lcore::vector_arena<Char*> elements_;

        Vector3Vector positions_;
        Vector3Vector normals_;
        Vector2Vector texcoords_;

        VertexVector vertices_;
        s32 lastNumTriangles_;

        MeshVector meshes_;

        s32 currentMaterial_;
        MaterialPack materialPack_;
    };
}
}
#endif //INC_LOAD_LOADEROBJ_H__
