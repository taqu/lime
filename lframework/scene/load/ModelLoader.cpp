/**
@file ModelLoader.cpp
@author t-sakai
@date 2014/12/09 create
*/
#include "ModelLoader.h"

#include <lgraphics/api/InputLayoutRef.h>
#include <lgraphics/api/VertexBufferRef.h>
#include <lgraphics/api/IndexBufferRef.h>
#include <lgraphics/api/TextureRef.h>
#include <lgraphics/io11/IODDS.h>
#include <lgraphics/io11/IOPNG.h>
#include <lgraphics/io11/IOBMP.h>

#include "../file/File.h"

#include "load_geometry.h"
#include "load_joint.h"
#include "load_material.h"
#include "load_mesh.h"
#include "load_node.h"
#include "load_texture.h"

#include "../SystemBase.h"

#include "../file/FileSystem.h"

#include "../render/InputLayoutManager.h"
#include "../render/Object.h"
#include "../render/AnimObject.h"
#include "../render/Geometry.h"
#include "../render/Material.h"
#include "../render/Mesh.h"
#include "../render/Node.h"

namespace lscene
{
namespace lload
{
    ModelLoader::ModelLoader()
        :file_(NULL)
        ,directoryPathLength_(0)
        ,textureAddress_(lgraphics::TexAddress_Clamp)
        ,reserveGeometryData_(false)
        ,convertRefractiveIndex_(true)
        ,openForceOSFileSystem_(false)
    {
        directoryPath_[0] = '\0';
    }

    ModelLoader::~ModelLoader()
    {
    }

    //---------------------------------------------
    // ディレクトリパスセット
    void ModelLoader::setDirectoryPath(const Char* filepath)
    {
        LASSERT(NULL != filepath);
        s32 len = lcore::strlen(filepath);
        if(MaxPathLength<len){
            len = MaxPathLength;
        }
        directoryPathLength_ = lcore::extractDirectoryPath(directoryPath_, filepath, len);
    }

    //---------------------------------------------
    // オブジェクトロード
    bool ModelLoader::load(lrender::Object& obj, lscene::lfile::SharedFile* file)
    {
        LASSERT(NULL != file);
        file_ = file;

        if(!load(header_)){
            return false;
        }

        u32 numGeometries = header_.elems_[Elem_Geometry].number_;
        u32 numMeshes = header_.elems_[Elem_Mesh].number_;
        u32 numMaterials = header_.elems_[Elem_Material].number_;
        u32 numNodes = header_.elems_[Elem_Node].number_;
        u32 numTextures = header_.elems_[Elem_Texture].number_;

        bool ret = obj.create(
            numGeometries,
            numMeshes,
            numMaterials,
            numNodes,
            numTextures);

        if(!ret){
            return false;
        }

        ret = loadInternal(obj.getContent());
        {//境界球
            lmath::Sphere sphere;
            sphere.zero();
            for(u32 i=0; i<numMeshes; ++i){
                sphere.combine(sphere, obj.getMesh(i).getSphere());
            }
            obj.setSphere(sphere);
        }
        return ret;
    }

    //---------------------------------------------
    // オブジェクトロード
    bool ModelLoader::load(lrender::AnimObject& obj, lscene::lfile::SharedFile* file)
    {
        LASSERT(NULL != file);
        file_ = file;

        if(!load(header_)){
            return false;
        }

        u32 numGeometries = header_.elems_[Elem_Geometry].number_;
        u32 numMeshes = header_.elems_[Elem_Mesh].number_;
        u32 numMaterials = header_.elems_[Elem_Material].number_;
        u32 numNodes = header_.elems_[Elem_Node].number_;
        //u32 numJoints = header_.elems_[Elem_Joint].number_;
        u32 numTextures = header_.elems_[Elem_Texture].number_;

        bool ret = obj.create(
            numGeometries,
            numMeshes,
            numMaterials,
            numNodes,
            numTextures);

        if(!ret){
            return false;
        }

        ret = loadInternal(obj.getContent());

        {//境界球
            lmath::Sphere sphere;
            sphere.zero();
            for(u32 i=0; i<numMeshes; ++i){
                sphere.combine(sphere, obj.getMesh(i).getSphere());
            }
            obj.setSphere(sphere);
        }

        //ノードに行列セット
        //if(ret){

        //    for(u32 i=0; i<numMeshes; ++i){
        //        obj.getMesh(i).setNumSkinningMatrices(numJoints);
        //        obj.getMesh(i).setSkinningMatrices(obj.getSkinningMatricesAligned16());
        //    }
        //}
        obj.setSkeleton(skeleton_);
        return ret;
    }

    //---------------------------------------------
    // オブジェクトロード
    bool ModelLoader::loadInternal(lrender::ObjectContent& obj)
    {
        u32 numGeometries = header_.elems_[Elem_Geometry].number_;
        u32 numMeshes = header_.elems_[Elem_Mesh].number_;
        u32 numMaterials = header_.elems_[Elem_Material].number_;
        u32 numNodes = header_.elems_[Elem_Node].number_;
        u32 numJoints = header_.elems_[Elem_Joint].number_;
        u32 numTextures = header_.elems_[Elem_Texture].number_;


        {//ジオメトリロード
            for(u32 i=0; i<numGeometries; ++i){
                if(false == load(obj.getGeometry(i))){
                    return false;
                }
            }
        }

        {//マテリアルロード
            for(u32 i=0; i<numMaterials; ++i){
                if(false == load(obj.getMaterial(i))){
                    return false;
                }
            }
        }


        {//メッシュロード
            for(u32 i=0; i<numMeshes; ++i){
                if(false == load(obj, obj.getMesh(i))){
                    return false;
                }
            }
        }

        {//ノードロード
            for(u32 i=0; i<numNodes; ++i){
                if(false == load(obj, obj.getNode(i))){
                    return false;
                }
            }
        }

        {//スケルトンロード
            skeleton_ = loadSkeleton(numJoints);
        }

        {//テクスチャロード
            lload::Texture loadTexture;
            lrender::Texture2D texture;
            for(u32 i=0; i<numTextures; ++i){
                file_->read(&loadTexture);

                if(false == load(texture, loadTexture)){
                    continue;
                }
                obj.setTexture(i, texture);

            }

            //マテリアルに参照セット
            //for(u32 i=0; i<numMaterials; ++i){
            //    lrender::Material& material = obj.getMaterial(i);

            //    for(u32 j=0; j<lrender::Material::Tex_Num; ++j){

            //        if(0<=material.textureIDs_[j] && material.textureIDs_[j]<numTextures){
            //            s16 id = material.textureIDs_[j];
            //            if(obj.getTexture(id).valid()){
            //                material.textures_[j] = &(obj.getTexture(id));
            //            }else{
            //                material.textures_[j] = NULL;
            //            }
            //        }
            //    }
            //}
        }
        return true;
    }

    //---------------------------------------------
    // ヘッダロード
    bool ModelLoader::load(Header& header)
    {
        return (LOAD_TRUE == file_->read(header));
    }

    //---------------------------------------------
    // ジオメトリロード
    bool ModelLoader::load(lrender::Geometry& geometry)
    {
        lload::Geometry tmp;
        if( LOAD_TRUE != file_->read(tmp) ){
            return false;
        }

        lgraphics::InputLayoutRef inputLayout;
        createInputLayout(tmp.vflag_, inputLayout);
        if(false == inputLayout.valid()){
            return false;
        }

        u32 size = tmp.vsize_*tmp.numVertices_;
        u8* vertices = reinterpret_cast<u8*>( LSCENE_MALLOC(size) );
        if( LOAD_TRUE != file_->read(vertices, size) ){
            LSCENE_FREE(vertices);
            return false;
        }

        lgraphics::VertexBufferRef vb = lgraphics::VertexBuffer::create(
            size,
            lgraphics::Usage_Default,
            lgraphics::CPUAccessFlag_None,
            lgraphics::ResourceMisc_None,
            0,
            vertices);

        //インデックスバッファ作成
        size = sizeof(u16) * tmp.numIndices_;
        u16* indices = reinterpret_cast<u16*>( LSCENE_MALLOC(size) );

        if( LOAD_TRUE != file_->read(indices, size) ){
            LSCENE_FREE(indices);
            return false;
        }

        lgraphics::IndexBufferRef ib = lgraphics::IndexBuffer::create(
            size,
            lgraphics::Usage_Default,
            lgraphics::CPUAccessFlag_None,
            lgraphics::ResourceMisc_None,
            0,
            indices);

        if(!reserveGeometryData_){
            LSCENE_FREE(vertices);
            LSCENE_FREE(indices);
        }

        geometry.create(
            tmp.vflag_,
            tmp.vsize_,
            tmp.numVertices_,
            vertices,
            tmp.numIndices_,
            indices,
            inputLayout, vb, ib);
        return true;
    }

    //---------------------------------------------
    // メッシュロード
    bool ModelLoader::load(lrender::ObjectContent& obj, lrender::Mesh& mesh)
    {
        lload::Mesh tmp;
        if( LOAD_TRUE != file_->read(tmp) ){
            return false;
        }
        mesh.create(
            lgraphics::Primitive_TriangleList,
            tmp.indexOffset_,
            tmp.numIndices_,
            &obj.getGeometry(tmp.geometry_),
            &obj.getMaterial(tmp.material_),
            tmp.sphere_);

        return true;
    }

    //---------------------------------------------
    // マテリアルロード
    bool ModelLoader::load(lrender::Material& material)
    {
        lload::Material tmp;
        if(LOAD_TRUE != file_->read(tmp)){
            return false;
        }

        material.flags_ = tmp.flags_;
        material.diffuse_ = tmp.diffuse_;
        material.specular_ = tmp.specular_;
        material.ambient_ = tmp.ambient_;
        material.shadow_ = tmp.shadow_;
        material.textureIDs_[0] = tmp.texColor_;
        material.textureIDs_[1] = tmp.texNormal_;

        if((material.flags_ & Material::Flag_RefractiveIndex) && convertRefractiveIndex_){
            material.ambient_.w_ = lcore::calcFresnelTerm(material.ambient_.w_);
        }
        return true;
    }

    //---------------------------------------------
    // ノードロード
    bool ModelLoader::load(lrender::ObjectContent& obj, lrender::Node& node)
    {
        lload::Node tmp;

        if(LOAD_TRUE != file_->read(tmp)){
            return false;
        }

        node.index_ = tmp.index_;
        node.parentIndex_ = tmp.parentIndex_;
        node.childStartIndex_ = tmp.childStartIndex_;
        node.numChildren_ = tmp.numChildren_;
        node.translation_ = tmp.translation_;
        //node.rotation_ = tmp.rotation_;
        node.scale_ = tmp.scale_;
        node.type_ = tmp.type_;
        node.rotationOrder_ = tmp.rotationOrder_;
        node.meshStartIndex_ = tmp.meshStartIndex_;
        node.numMeshes_ = tmp.numMeshes_;

        node.calcRotation(tmp.rotation_);
        node.world_.identity();

        if(node.meshStartIndex_ == lload::InvalidNode){
            node.meshes_ = NULL;
        }else{
            node.meshes_ = &obj.getMesh(node.meshStartIndex_);
        }

        return true;
    }

    //---------------------------------------------
    lanim::Skeleton* ModelLoader::loadSkeleton(u32 numJoints)
    {
        if(numJoints<=0){
            return NULL;
        }

        lanim::Skeleton* skeleton = LSCENE_NEW lanim::Skeleton(numJoints);
        lload::Joint tjoint;
        lanim::Name name;
        for(u32 i=0; i<numJoints; ++i){
            if(LOAD_TRUE != file_->read(tjoint)){
                LSCENE_DELETE(skeleton);
                return NULL;
            }
            lanim::Joint& joint = skeleton->getJoint(i);
            joint.setParentIndex(tjoint.parent_);
            joint.setSubjectTo(tjoint.subjectTo_);
            joint.setType(tjoint.type_);
            joint.setFlag(tjoint.flag_);

            lmath::Matrix34 invInitialMatrix = tjoint.initialMatrix_;
            invInitialMatrix.invert();
            joint.setInitialMatrix(tjoint.initialMatrix_);
            joint.setInvInitialMatrix(invInitialMatrix);
            skeleton->setJointName(i, tjoint.name_, lload::MaxNameLength);
        }
        return skeleton;
    }

    //---------------------------------------------
    // テクスチャロード
    bool ModelLoader::load(lrender::Texture2D& texture, const lload::Texture& loadTexture)
    {
        u32 len = lcore::strlen(loadTexture.name_);
        const Char* ext = lcore::rFindChr(loadTexture.name_, '.', len);
        if(NULL == ext){
            return false;
        }

        FileType type = FileType_DDS;
        if(0==lcore::strncmp(ext, ".dds", 4)){
            type = FileType_DDS;
        }else if(0==lcore::strncmp(ext, ".png", 4)){
            type = FileType_PNG;
        }else if(0==lcore::strncmp(ext, ".bmp", 4)){
            type = FileType_BMP;
        }

        lcore::strncpy(directoryPath_+directoryPathLength_, MaxFileNameSize, loadTexture.name_, len);

        lscene::lfile::FileSystemBase* fileSystem = SystemInstance::getInstance().getFileSystem();
        lscene::lfile::SharedFile texfile = (openForceOSFileSystem_)? fileSystem->openForceOSFileSystem(directoryPath_) : fileSystem->open(directoryPath_);
        if(!texfile.is_open()){
            return false;
        }
        return lload::load(
            texture.texture_,
            texture.sampler_,
            texture.srv_,
            texfile,
            type,
            lgraphics::Usage_Immutable,
            lgraphics::TexFilter_MinMagMipLinear,
            textureAddress_,
            (loadTexture.type_ == TexType_Albedo));
    }

    //---------------------------------------------
    // 頂点レイアウト作成
    void ModelLoader::createInputLayout(u32 vflag, lgraphics::InputLayoutRef& inputLayout)
    {
        lrender::Layout layoutType = lrender::Layout_P;
        switch(vflag)
        {
        case (VElem_Position):
            layoutType = lrender::Layout_P;
            break;

        case (VElem_Position|VElem_Normal):
            layoutType = lrender::Layout_PN;
            break;

        case (VElem_Position|VElem_Texcoord):
            layoutType = lrender::Layout_PU;
            break;

        case (VElem_Position|VElem_Color):
            layoutType = lrender::Layout_PC;
            break;

        case (VElem_Position|VElem_Normal|VElem_Tangent|VElem_Binormal):
            layoutType = lrender::Layout_PNTB;
            break;

        case (VElem_Position|VElem_Normal|VElem_Texcoord):
            layoutType = lrender::Layout_PNU;
            break;

        case (VElem_Position|VElem_Normal|VElem_Color|VElem_Texcoord):
            layoutType = lrender::Layout_PNCU;
            break;

        case (VElem_Position|VElem_Normal|VElem_Tangent|VElem_Binormal|VElem_Texcoord):
            layoutType = lrender::Layout_PNTBU;
            break;

        case (VElem_Position|VElem_Normal|VElem_Texcoord|VElem_Bone|VElem_BoneWeight):
            layoutType = lrender::Layout_PNUBone;
            break;

        default:
            LASSERT(false);
            break;
        }

        inputLayout = lrender::InputLayoutManager::getInstance().get(layoutType);
    }

    //
    void ModelLoader::getTextureNameTable(u32 numTextures, lload::Texture* textures)
    {
        LASSERT(NULL != file_);

        file_->seekg(0);
        load(header_);

        file_->seekg(header_.elems_[Elem_Texture].offset_);
        LASSERT(numTextures == header_.elems_[Elem_Texture].number_);

        for(u32 i=0; i<numTextures; ++i){
            file_->read(textures[i]);
        }
    }

    //
    bool ModelLoader::save(lrender::Object& obj, lload::Texture* textures, lanim::Skeleton::pointer& skeleton, const Char* filepath)
    {
        lcore::ofstream os(filepath, lcore::ios::binary);
        if(!os.is_open()){
            return false;
        }

        lload::Header loadHeader;
        loadHeader.major_ = 1;
        loadHeader.minor_ = 0;
        u32 offset = sizeof(lload::Header);

        lload::Geometry* loadGeometries = LIME_NEW lload::Geometry[obj.getNumGeometries()];
        loadHeader.elems_[lload::Elem_Geometry].number_ = obj.getNumGeometries();
        loadHeader.elems_[lload::Elem_Geometry].offset_ = offset;
        for(u32 i=0; i<obj.getNumGeometries(); ++i){
            lrender::Geometry& geometry = obj.getGeometry(i);
            loadGeometries[i].vflag_ = geometry.getVFlag();
            loadGeometries[i].vsize_ = geometry.getVSize();
            loadGeometries[i].numVertices_ = geometry.getNumVertices();
            loadGeometries[i].numIndices_ = geometry.getNumIndices();

            offset += sizeof(lload::Geometry);
            offset += geometry.getVSize() * geometry.getNumVertices();
            offset += sizeof(u16) * geometry.getNumIndices();
        }

        lload::Material* loadMaterials = LIME_NEW lload::Material[obj.getNumMaterials()];
        loadHeader.elems_[lload::Elem_Material].number_ = obj.getNumMaterials();
        loadHeader.elems_[lload::Elem_Material].offset_ = offset;
        for(u32 i=0; i<obj.getNumMaterials(); ++i){
            lrender::Material& material = obj.getMaterial(i);
            loadMaterials[i].flags_ = material.flags_;
            loadMaterials[i].diffuse_ = material.diffuse_;
            loadMaterials[i].specular_ = material.specular_;
            loadMaterials[i].ambient_ = material.ambient_;
            loadMaterials[i].shadow_ = material.shadow_;
            loadMaterials[i].texColor_ = material.textureIDs_[lrender::Tex_Albedo];
            loadMaterials[i].texNormal_ = material.textureIDs_[lrender::Tex_Normal];

            offset += sizeof(lload::Material);
        }

        lload::Mesh* loadMeshes = LIME_NEW lload::Mesh[obj.getNumMeshes()];
        loadHeader.elems_[lload::Elem_Mesh].number_ = obj.getNumMeshes();
        loadHeader.elems_[lload::Elem_Mesh].offset_ = offset;
        for(u32 i=0; i<obj.getNumMeshes(); ++i){
            lrender::Mesh& mesh = obj.getMesh(i);

            loadMeshes[i].geometry_ = static_cast<s16>(mesh.getGeometry() - &obj.getGeometry(0));
            loadMeshes[i].material_ = static_cast<s16>(mesh.getMaterial() - &obj.getMaterial(0));
            loadMeshes[i].indexOffset_ = mesh.getIndexOffset();
            loadMeshes[i].numIndices_ = mesh.getNumIndices();
            loadMeshes[i].sphere_ = mesh.getSphere();

            offset += sizeof(lload::Mesh);
        }

        lload::Node* loadNodes = LIME_NEW lload::Node[obj.getNumNodes()];
        loadHeader.elems_[lload::Elem_Node].number_ = obj.getNumNodes();
        loadHeader.elems_[lload::Elem_Node].offset_ = offset;
        for(u32 i=0; i<obj.getNumNodes(); ++i){
            lrender::Node& node = obj.getNode(i);
            loadNodes[i].index_ = node.index_;
            loadNodes[i].parentIndex_ = node.parentIndex_;
            loadNodes[i].childStartIndex_ = node.childStartIndex_;
            loadNodes[i].numChildren_ = node.numChildren_;
            loadNodes[i].translation_.set(node.translation_.x_, node.translation_.y_, node.translation_.z_);
            loadNodes[i].rotation_.set(node.rotation_.x_, node.rotation_.y_, node.rotation_.z_);
            loadNodes[i].scale_.set(node.scale_.x_, node.scale_.y_, node.scale_.z_);

            loadNodes[i].type_ = node.type_;
            loadNodes[i].rotationOrder_ = node.rotationOrder_;
            loadNodes[i].meshStartIndex_ = node.meshStartIndex_;
            loadNodes[i].numMeshes_ = node.numMeshes_;

            offset += sizeof(lload::Node);
        }

        lload::Joint* loadJoints = NULL;
        if(NULL == skeleton){
            loadHeader.elems_[lload::Elem_Joint].number_ = 0;
            loadHeader.elems_[lload::Elem_Joint].offset_ = offset;
        }else{
            loadJoints = LIME_NEW lload::Joint[skeleton->getNumJoints()];
            loadHeader.elems_[lload::Elem_Joint].number_ = skeleton->getNumJoints();
            loadHeader.elems_[lload::Elem_Joint].offset_ = offset;

            for(s32 i=0; i<skeleton->getNumJoints(); ++i){
                lanim::Joint& joint = skeleton->getJoint(i);
                loadJoints[i].parent_ = joint.getParentIndex();
                loadJoints[i].subjectTo_ = joint.getSubjectTo();
                loadJoints[i].type_ = joint.getType();
                loadJoints[i].flag_ = joint.getFlag();
                loadJoints[i].initialMatrix_ = joint.getInitialMatrix();
                lcore::memcpy(loadJoints[i].name_, skeleton->getJointName(i).c_str(), lload::MaxNameSize);
                offset += sizeof(lload::Joint);
            }
        }
        loadHeader.elems_[lload::Elem_Texture].number_ = obj.getNumTextures();
        loadHeader.elems_[lload::Elem_Texture].offset_ = offset;


        lcore::io::write(os, loadHeader);
        for(u32 i=0; i<obj.getNumGeometries(); ++i){
            lrender::Geometry& geometry = obj.getGeometry(i);

            lcore::io::write(os, loadGeometries[i]);
            lcore::io::write(os, geometry.getVertices(), geometry.getVSize()*geometry.getNumVertices());
            lcore::io::write(os, geometry.getIndices(), sizeof(u16)*geometry.getNumIndices());
        }

        lcore::io::write(os, loadMaterials, sizeof(lload::Material)*obj.getNumMaterials());
        lcore::io::write(os, loadMeshes, sizeof(lload::Mesh)*obj.getNumMeshes());
        lcore::io::write(os, loadNodes, sizeof(lload::Node)*obj.getNumNodes());
        if(NULL != loadJoints){
            lcore::io::write(os, loadJoints, sizeof(lload::Joint)*skeleton->getNumJoints());
        }

        lcore::io::write(os, textures, sizeof(lload::Texture)*obj.getNumTextures());

        os.close();

        LIME_DELETE_ARRAY(loadJoints);
        LIME_DELETE_ARRAY(loadNodes);
        LIME_DELETE_ARRAY(loadMeshes);
        LIME_DELETE_ARRAY(loadMaterials);
        LIME_DELETE_ARRAY(loadGeometries);
        return true;
    }
}
}
