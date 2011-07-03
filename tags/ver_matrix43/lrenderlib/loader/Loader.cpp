/**
@file Loader.cpp
@author t-sakai
@date 2010/01/07
*/
#include "Loader.h"

#include <assimp.hpp>
#include <aiScene.h>
#include <aiPostProcess.h>

#include <lcore/lcore.h>
#include <lcore/FixedArray.h>
#include <lmath/Color.h>

#include <lgraphics/api/RenderState.h>
#include <lgraphics/api/Texture.h>
#include <lgraphics/scene/Material.h>


#include "../geometry/GeometryBuffer.h"
#include "../geometry/Geometry.h"
#include "../geometry/Mesh.h"
#include "../geometry/Face.h"
#include "../geometry/Vertex.h"

namespace lrender
{
    using namespace lgraphics;
    using namespace lmath;

    namespace
    {
        inline void subst(lmath::Vector4& vec, const aiColor4D& color)
        {
            vec._x = color.r;
            vec._y = color.g;
            vec._z = color.b;
            vec._w = color.a;
        }

        inline void subst(lmath::Vector3& vec, const aiColor4D& color)
        {
            vec._x = color.r;
            vec._y = color.g;
            vec._z = color.b;
        }

        void copyBuffer(char* dst, const char* src, int count, int size, int stride)
        {
            for(int i=0; i<count; ++i){
                memcpy(dst, src, size);
                src += size;
                dst += stride;
            }
        }

        void copyColor(char* dst, const aiColor4D* src, int count, int stride)
        {
            for(int i=0; i<count; ++i){
                Color *color = (Color*)dst;

                u8 a = static_cast<u8>(src->a*255.0f + 0.5f);
                u8 r = static_cast<u8>(src->r*255.0f + 0.5f);
                u8 g = static_cast<u8>(src->g*255.0f + 0.5f);
                u8 b = static_cast<u8>(src->b*255.0f + 0.5f);
                color->set(a, r, g, b);

                ++src;
                dst += stride;
            }
        }

        void copyUV(char* dst, const aiVector3D* src, int count, int stride)
        {
            for(int i=0; i<count; ++i){
                lmath::Vector2 *uv = (lmath::Vector2*)dst;

                uv->set(src->x, src->y);

                ++src;
                dst += stride;
            }
        }

        const char* getShadingName(int shading)
        {
            switch(shading)
            {
            case aiShadingMode_Flat:
                return "flat";
                break;

            case aiShadingMode_Gouraud:
                return "gourand";
                break;

            case aiShadingMode_Phong:
                return "phong";
                break;

            case aiShadingMode_Blinn:
                return "blinn";
                break;

            case aiShadingMode_Toon:
                return "toon";
                break;

            case aiShadingMode_OrenNayar:
                LASSERT(false && "Shading: not supported");
                break;

            case aiShadingMode_Minnaert:
                LASSERT(false && "Shading: not supported");
                break;

            case aiShadingMode_CookTorrance:
                LASSERT(false && "Shading: not supported");
                break;

            case aiShadingMode_NoShading:
                LASSERT(false && "Shading: not supported");
                break;

            case aiShadingMode_Fresnel:
                LASSERT(false && "Shading: not supported");
                break;

            default:
                LASSERT(false && "Shading: not supported");
                break;
            };
            return NULL;
        }


        // AssetImporterのMeshからGeometryに変換
        void convertMesh(aiMesh* mesh, lrender::Geometry& geom)
        {
            static const int vec3Size = sizeof(lmath::Vector3);
            static const int vec2Size = sizeof(lmath::Vector2);
            static const int colorSize = sizeof(Color);

            u32 numElement =0;
            numElement += mesh->GetNumColorChannels();
            numElement += mesh->GetNumUVChannels();
            numElement += (mesh->HasNormals())? 1 : 0;
            numElement += (mesh->HasPositions())? 1 : 0;
            numElement += (mesh->HasTangentsAndBitangents())? 2 : 0;

            u32 vertexSize = 0;
            vertexSize += mesh->GetNumColorChannels() * colorSize;
            vertexSize += mesh->GetNumUVChannels() * vec2Size;
            vertexSize += (mesh->HasNormals())? vec3Size : 0;
            vertexSize += (mesh->HasPositions())? vec3Size : 0;
            vertexSize += (mesh->HasTangentsAndBitangents())? 2 * vec3Size : 0;

            VertexBufferUP vb(vertexSize, mesh->mNumVertices);
            char *buffer = NULL;
            vb.lock((void**)&buffer);

            VertexDeclCreator declCreator(numElement);
            u16 stream = 0;
            u16 offset = 0;

            LASSERT(mesh->HasPositions());
            //if(mesh->HasPositions()){
                declCreator.add(stream, offset, lgraphics::DeclType_Float3, lgraphics::DeclUsage_Position, 0);

                copyBuffer(buffer+offset, (const char*)mesh->mVertices, mesh->mNumVertices, vec3Size, vertexSize);

                offset += vec3Size;
            //}

            LASSERT(mesh->HasNormals());
            //if(mesh->HasNormals()){
                declCreator.add(stream, offset, lgraphics::DeclType_Float3, lgraphics::DeclUsage_Normal, 0);

                copyBuffer(buffer+offset, (const char*)mesh->mNormals, mesh->mNumVertices, vec3Size, vertexSize);

                offset += vec3Size;
            //}

            if(mesh->HasTangentsAndBitangents()){
                declCreator.add(stream, offset, lgraphics::DeclType_Float3, lgraphics::DeclUsage_Tangent, 0);

                copyBuffer(buffer+offset, (const char*)mesh->mTangents, mesh->mNumVertices, vec3Size, vertexSize);

                offset += vec3Size;


                declCreator.add(stream, offset, lgraphics::DeclType_Float3, lgraphics::DeclUsage_Binormal, 0);

                copyBuffer(buffer+offset, (const char*)mesh->mBitangents, mesh->mNumVertices, vec3Size, vertexSize);

                offset += vec3Size;
            }

            for(u32 i=0; i<mesh->GetNumColorChannels(); ++i){
                declCreator.add(stream, offset, lgraphics::DeclType_Color, lgraphics::DeclUsage_Tangent, i);

                copyColor(buffer+offset, mesh->mColors[i], mesh->mNumVertices, vertexSize);

                offset += colorSize;
            }

            for(u32 i=0; i<mesh->GetNumUVChannels(); ++i){
                declCreator.add(stream, offset, lgraphics::DeclType_Float2, lgraphics::DeclUsage_Texcoord, i);

                copyUV(buffer+offset, mesh->mTextureCoords[i], mesh->mNumVertices, vertexSize);

                offset += vec2Size;
            }

            VertexDeclaration decl;
            declCreator.end(decl);

            lgraphics::PrimitiveType primType;

            switch(mesh->mPrimitiveTypes)
            {
            case aiPrimitiveType_TRIANGLE:
                primType = lgraphics::Primitive_TriangleList;
                break;

            default:
                LASSERT(false);
            }

            s32 materialIndex = static_cast<s32>(mesh->mMaterialIndex);

            IndexBufferUP ib;
            if(mesh->HasFaces()){
                IndexBufferUP temp(mesh->mNumFaces);

                u16 *index = NULL;
                temp.lock((void**)&index);
                for(u32 i=0; i<mesh->mNumFaces; ++i){
                    aiFace &face = mesh->mFaces[i];
                    index[0] = face.mIndices[0];
                    index[1] = face.mIndices[1];
                    index[2] = face.mIndices[2];
                    index += 3;
                }
                temp.unlock();
                ib.swap(temp);
            }

            lrender::GeometryBuffer::pointer geomBuffer( LIME_NEW lrender::GeometryBuffer(primType, decl, vb, ib) );
            lrender::Geometry temp(geomBuffer, mesh->mNumFaces, materialIndex);

            geom.swap(temp);
        }


        void convertMaterial(aiMaterial* aiMat, Material& material)
        {
            aiReturn result;


            aiString name;
            result = aiMat->Get(AI_MATKEY_NAME, name);

            aiColor4D diffuse(0.0f, 0.0f, 0.0f, 1.0f);
            result = aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);

            aiColor4D specular(0.0f, 0.0f, 0.0f, 1.0f);
            result = aiMat->Get(AI_MATKEY_COLOR_SPECULAR, specular);

            aiColor4D ambient(0.0f, 0.0f, 0.0f, 1.0f);
            result = aiMat->Get(AI_MATKEY_COLOR_AMBIENT, ambient);

            aiColor4D emissive(0.0f, 0.0f, 0.0f, 1.0f);
            result = aiMat->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);

            aiColor4D transparent(0.0f, 0.0f, 0.0f, 1.0f);
            result = aiMat->Get(AI_MATKEY_COLOR_TRANSPARENT, transparent);

            int wireframe = 0;
            result = aiMat->Get(AI_MATKEY_ENABLE_WIREFRAME, wireframe);

            int twosided = 0;
            result = aiMat->Get(AI_MATKEY_TWOSIDED, twosided);

            int shading = -1;
            result = aiMat->Get(AI_MATKEY_SHADING_MODEL, shading);

            int blendfunc = -1;
            result = aiMat->Get(AI_MATKEY_BLEND_FUNC, blendfunc);

            float opacity = 1.0f;
            result = aiMat->Get(AI_MATKEY_OPACITY, opacity);

            float shininess = 0.0f;
            result = aiMat->Get(AI_MATKEY_SHININESS, shininess);

            float shininess_strength = 1.0f;
            result = aiMat->Get(AI_MATKEY_SHININESS_STRENGTH, shininess_strength);

            float refraction = 1.0f;
            result = aiMat->Get(AI_MATKEY_REFRACTI, refraction);

            //Create RenderState
            RenderState::begin();
            if(twosided){
                RenderState::setCullMode(lgraphics::CullMode_None);
            }else{
                RenderState::setCullMode(lgraphics::CullMode_CCW);
                //RenderState::setCullMode(lgraphics::CullMode_None);
            }

            if(lmath::isEqual(opacity, 1.0f)){
                RenderState::setAlphaBlendEnable(false);

                if(false == transparent.IsBlack()){
                    RenderState::setAlphaTest(true);
                }else{
                    RenderState::setAlphaTest(false);
                }

            }else{
                switch(blendfunc)
                {
                case aiBlendMode_Default:
                    RenderState::setAlphaBlendEnable(true);
                    RenderState::setAlphaBlendSrc(lgraphics::Blend_SrcAlpha);
                    RenderState::setAlphaBlendDst(lgraphics::Blend_InvDestAlpha);
                    break;

                case aiBlendMode_Additive:
                    RenderState::setAlphaBlendEnable(true);
                    RenderState::setAlphaBlendSrc(lgraphics::Blend_SrcAlpha);
                    RenderState::setAlphaBlendDst(lgraphics::Blend_DestAlpha);
                    break;

                default:
                    RenderState::setAlphaBlendEnable(false);
                    break;
                }
            }

            RenderState state;
            RenderState::end(state);

            //Textures

            Material temp;

            temp.name_.assign( &(name.data[0]) );
            temp.shading_.assign( getShadingName(shading) );

            subst(temp.diffuse_, diffuse);
            subst(temp.specular_, specular);
            subst(temp.ambient_, ambient);
            subst(temp.emissive_, emissive);
            subst(temp.transparent_, transparent);

            temp.opacity_ = opacity;
            temp.shininess_ = shininess;
            temp.shininessStrength_ = shininess_strength;
            temp.refraction_ = refraction;

            temp.setState(state);

            material.swap(temp);
        }
    }

    class Loader::Impl
    {
    public:
        Impl();
        ~Impl();

        bool readFile(const char* filename);

        bool readGeometry();
        bool readMaterial();

        Mesh* releaseMesh()
        {
            Mesh *mesh = mesh_;
            mesh_ = NULL;
            return mesh;
        }

        Assimp::Importer importer_;
        const aiScene* scene_;

        Mesh* mesh_;
    };

    Loader::Impl::Impl()
        :scene_(NULL)
        ,mesh_(NULL)
    {
    }

    Loader::Impl::~Impl()
    {
        LIME_DELETE(mesh_);
    }

    //---------------------------------------------------
    // ファイルロード
    bool Loader::Impl::readFile(const char* filename)
    {
        LASSERT(filename != NULL);

        //シーンロード
        scene_ = importer_.ReadFile(
            filename,
            aiProcess_ConvertToLeftHanded //左座標系化
            //| aiProcess_GenNormals
            | aiProcess_GenSmoothNormals  //頂点法線の平滑化
            | aiProcess_ImproveCacheLocality//
            //| aiProcess_CalcTangentSpace
            | aiProcess_SortByPType
            //| aiProcess_JoinIdenticalVertices
            //| aiProcessPreset_TargetRealtime_MaxQuality
            | aiProcess_Triangulate); //三角形リスト化

        if(scene_ == NULL){
            LogInfo( importer_.GetErrorString() );
            return false;
        }

        u32 numGeometries = (scene_->HasMeshes())? scene_->mNumMeshes : 0;
        u32 numMaterials = (scene_->HasMaterials())? scene_->mNumMaterials : 0;

        mesh_ = LIME_NEW Mesh(numGeometries, numMaterials);
        readGeometry();
        readMaterial();
        return true;
    }

    bool Loader::Impl::readGeometry()
    {
        LASSERT(mesh_ != NULL);

        u32 numGeometries = mesh_->getNumGeometries();

        for(u32 i=0; i<numGeometries; ++i){
            aiMesh *mesh = scene_->mMeshes[i];
            convertMesh(mesh, mesh_->getGeometry(i));
        }
        return true;
    }

    bool Loader::Impl::readMaterial()
    {
        u32 numMaterials = scene_->mNumMaterials;

        for(u32 i=0; i<numMaterials; ++i){
            aiMaterial *material = scene_->mMaterials[i];
            convertMaterial(material, mesh_->getMaterial(i));
        }
        return true;
    }


    //--------------------------------------------------------
    //---
    //--- Loader
    //---
    //--------------------------------------------------------
    Loader::Loader()
        :impl_(LIME_NEW Impl)
    {
    }

    Loader::~Loader()
    {
        LIME_DELETE(impl_);
    }

    bool Loader::readFile(const char* filename)
    {
        return impl_->readFile(filename);
    }


    Mesh* Loader::releaseMesh()
    {
        return impl_->releaseMesh();
    }
}
