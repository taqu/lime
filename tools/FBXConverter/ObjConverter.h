#ifndef INC_LOAD_LOADEROBJ_H__
#define INC_LOAD_LOADEROBJ_H__
#include "load.h"
#include <lmath/lmath.h>
#include "load_material.h"
#include "load_texture.h"

#include <fstream>
#include <vector>
#include <map>
#include <string>

namespace lcore
{
    class istream;
}

namespace load
{
    class Geometry;

    class LoaderObj
    {
    public:
        LoaderObj();
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

        typedef std::vector<lmath::Vector3> Vector3Vector;
        typedef std::vector<lmath::Vector2> Vector2Vector;
        typedef std::vector<U16Vector4> U16Vector4Vector;
        typedef std::vector<U16Vector2> U16Vector2Vector;
        typedef std::vector<u16> U16Vector;
        typedef std::vector<s32> S32Vector;
        typedef std::map<s32, s32> S32Map;
        typedef std::map<std::string, s32> NameToMaterialMap;
        typedef std::map<std::string, s32> NameToTextureMap;

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
            CodeMtl_MapDiffuse,
            CodeMtl_MapBump,
            CodeMtl_Illumination,
            CodeMtl_EOF,
        };

        struct Vertex
        {
            s32 position_;
            s32 texcoord_;
            s32 normal_;
            s16 material_;
            s16 type_;
        };

        void skip(std::ifstream& input);

        bool getLine(std::ifstream& input);

        void split();

        void getFElems(f32 felems[4]);
        void getFaceVertex(Char* str, Vertex& v);

        Code decodeLine(std::ifstream& input);
        bool readLine(std::ifstream& input);

        bool loadMtl(const Char* filepath);
        CodeMtl decodeLineMtl(std::ifstream& input);
        bool readLineMtl(std::ifstream& input);

        void getInt(s32& dst);

        void getFloat(f32& dst);
        void getFloat3(lmath::Vector3& dst);
        s32 addMaterial(const std::string& name);
        s32 getMaterial(const std::string& name);

        void loadTexture(const Char* name);
        s32 getTexture(const Char* name);

        void setVertexProperty(Geometry& geometry);

        void push_back_scale(Vector3Vector& v, const lmath::Vector3& p, f32 scale);
        void push_back(U16Vector4Vector& v, const lmath::Vector3& n);
        void push_back(U16Vector2Vector& v, const lmath::Vector2& t);

        Char* directory_;
        std::vector<Char> line_;
        std::vector<Char*> elems_;

        s32 currentMaterialId_;
        Vector3Vector positions_;
        Vector3Vector normals_;
        Vector2Vector texcoords_;

        typedef std::vector<Vertex> VertexVector;
        VertexVector vertices_;

        std::string currentMaterial_;
        NameToMaterialMap materials_;
        std::vector<Material> materialVector_;

        NameToTextureMap textures_;
        std::vector<Texture> textureVector_;
    };
}
#endif //INC_LOAD_LOADEROBJ_H__
