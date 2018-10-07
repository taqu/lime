/**
@file ModelLoader.cpp
@author t-sakai
@date 2016/11/23 create
*/
#include "resource/ModelLoader.h"

#include <lcore/File.h>
#include <lgraphics/InputLayoutRef.h>
#include <lgraphics/VertexBufferRef.h>
#include <lgraphics/IndexBufferRef.h>
#include <lgraphics/TextureRef.h>
#include <lgraphics/io/IODDS.h>
#include <lgraphics/io/IOPNG.h>
#include <lgraphics/io/IOBMP.h>

#include "System.h"

#include "resource/load_geometry.h"
#include "resource/load_joint.h"
#include "resource/load_material.h"
#include "resource/load_mesh.h"
#include "resource/load_node.h"
#include "resource/load_texture.h"

#include "render/Model.h"
#include "render/Geometry.h"
#include "render/Material.h"
#include "render/Mesh.h"
#include "render/Node.h"
#include "animation/Skeleton.h"

#include "resource/Resources.h"
#include "resource/InputLayoutFactory.h"
#include "resource/ResourceTexture2D.h"

namespace lfw
{
    ModelLoader::ModelLoader()
        :fileSystem_(NULL)
        ,directoryPathLength_(0)
        ,path_(NULL)
        ,reserveGeometryData_(false)
        ,convertRefractiveIndex_(true)
    {
    }

    ModelLoader::ModelLoader(lcore::FileSystem* fileSystem)
        :fileSystem_(fileSystem)
        ,directoryPathLength_(0)
        ,path_(NULL)
        ,reserveGeometryData_(false)
        ,convertRefractiveIndex_(true)
    {
    }

    ModelLoader::~ModelLoader()
    {
        LDELETE_ARRAY(path_);
    }

    void ModelLoader::set(lcore::FileSystem* fileSystem)
    {
        fileSystem_ = fileSystem;
    }

    //---------------------------------------------
    // オブジェクトロード
    void ModelLoader::loadModel(Model*& model, Skeleton*& skeleton, s32 setID, const Char* filepath)
    {
        LASSERT(NULL != filepath);

        clearTextures();

        model = NULL;

        lcore::FileProxy* file = NULL;
        if(NULL != fileSystem_){
            file = fileSystem_->openFile(filepath);
        } else{
            lcore::LHANDLE handle = CreateFile(
                filepath,
                GENERIC_READ,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL);
            file = LNEW lcore::FileProxyOSRaw(handle);
        }
        if(NULL == file){
            return;
        }
        fileStream_.reset(file);

        //ディレクトリパスセット
        setDirectoryPath(filepath);

        if(!load(header_)){
            clearFileStream(file);
            return;
        }

        u32 numGeometries = header_.elems_[Elem_Geometry].number_;
        u32 numMeshes = header_.elems_[Elem_Mesh].number_;
        u32 numMaterials = header_.elems_[Elem_Material].number_;
        u32 numNodes = header_.elems_[Elem_Node].number_;
        u32 numTextures = header_.elems_[Elem_Texture].number_;

        model = LNEW Model;
        if(!model->create(
            numGeometries,
            numMeshes,
            numMaterials,
            numNodes,
            numTextures))
        {
            LDELETE(model);
            clearFileStream(file);
            return;
        }

        if(!loadInternal(*model)){
            LDELETE(model);
            clearFileStream(file);
            return;
        }

        skeleton = NULL;
        loadSkeleton(skeleton);

        if(!loadTextures(setID, *model)){
            LDELETE(model);
            clearFileStream(file);
            return;
        }

        //境界球
        calcBoundingSphere(*model);

        clearFileStream(file);
    }

    //---------------------------------------------
    void ModelLoader::setDirectoryPath(const Char* path)
    {
        s32 len = lcore::strlen_s32(path);
        if(directoryPathLength_<len){
            LDELETE_ARRAY(path_);
            directoryPathLength_ = len;
            path_ = LNEW Char[directoryPathLength_+MaxNameSize]; //ファイル名のサイズだけ余分に確保
        }
        directoryPathLength_ = lcore::Path::extractDirectoryPath(path_, len, path);
    }

    void ModelLoader::clearFileStream(lcore::FileProxy* file)
    {
        fileStream_.reset(NULL);
        if(NULL != fileSystem_){
            fileSystem_->closeFile(file);
        } else{
            LDELETE(file);
        }
    }

    //---------------------------------------------
    // オブジェクトロード
    bool ModelLoader::loadInternal(Model& model)
    {
        s32 numGeometries = static_cast<s32>(header_.elems_[Elem_Geometry].number_);
        s32 numMeshes = static_cast<s32>(header_.elems_[Elem_Mesh].number_);
        s32 numMaterials = static_cast<s32>(header_.elems_[Elem_Material].number_);
        s32 numNodes = static_cast<s32>(header_.elems_[Elem_Node].number_);

        {//ジオメトリロード
            for(s32 i=0; i<numGeometries; ++i){
                if(false == load(model.getGeometry(i))){
                    return false;
                }
            }
        }

        {//マテリアルロード
            for(s32 i=0; i<numMaterials; ++i){
                if(false == load(model.getMaterial(i))){
                    return false;
                }
            }
        }


        {//メッシュロード
            for(s32 i=0; i<numMeshes; ++i){
                if(false == load(model, model.getMesh(i))){
                    return false;
                }
            }
        }

        {//ノードロード
            for(s32 i=0; i<numNodes; ++i){
                if(false == load(model, model.getNode(i))){
                    return false;
                }
            }
        }
        return true;
    }

    bool ModelLoader::loadSkeleton(Skeleton*& skeleton)
    {
        u32 numJoints = header_.elems_[Elem_Joint].number_;
        if(numJoints<=0){
            return true;
        }
        skeleton = LNEW Skeleton(numJoints);
        LoadJoint tjoint;
        Name name;
        for(u32 i=0; i<numJoints; ++i){
            if(!fileStream_.read(tjoint)){
                LDELETE(skeleton);
                return false;
            }
            Joint& joint = skeleton->getJoint(i);
            joint.setParentIndex(tjoint.parent_);
            joint.setSubjectTo(tjoint.subjectTo_);
            joint.setType(tjoint.type_);
            joint.setFlag(tjoint.flag_);

            lmath::Matrix34 invInitialMatrix = tjoint.initialMatrix_;
            invInitialMatrix.invert();
            joint.setInitialMatrix(tjoint.initialMatrix_);
            joint.setInvInitialMatrix(invInitialMatrix);
            skeleton->setJointName(i, MaxNameLength, tjoint.name_);
        }
        return true;
    }

    bool ModelLoader::loadTextures(s32 setID, Model& model)
    {
        //テクスチャロード
        LoadTexture loadTexture;
        Texture2D texture;
        for(s32 i=0; i<model.getNumTextures(); ++i){
            fileStream_.read(loadTexture);
            if(!load(setID, texture, loadTexture)){
                continue;
            }
            model.setTexture(i, texture);
        }
        return true;
    }

    void ModelLoader::calcBoundingSphere(Model& model)
    {
        lmath::Sphere sphere;
        sphere.zero();
        for(s32 i=0; i<model.getNumMeshes(); ++i){
            sphere.combine(sphere, model.getMesh(i).getSphere());
        }
        model.setSphere(sphere);
    }

    //---------------------------------------------
    // ヘッダロード
    bool ModelLoader::load(LoadHeader& header)
    {
        return fileStream_.read(header);
    }

    //---------------------------------------------
    // ジオメトリロード
    bool ModelLoader::load(Geometry& geometry)
    {
        LoadGeometry tmp;
        if(!fileStream_.read(tmp)){
            return false;
        }

        lgfx::InputLayoutRef inputLayout;
        createInputLayout(tmp.vflag_, inputLayout);
        if(!inputLayout.valid()){
            return false;
        }

        s32 size = tmp.vsize_*tmp.numVertices_;
        u8* vertices = static_cast<u8*>(LMALLOC(size));
        if(!fileStream_.read(size, vertices)){
            LFREE(vertices);
            return false;
        }

        lgfx::VertexBufferRef vb = lgfx::VertexBuffer::create(
            size,
            lgfx::Usage_Default,
            lgfx::CPUAccessFlag_None,
            lgfx::ResourceMisc_None,
            0,
            vertices);

        //インデックスバッファ作成
        size = sizeof(u16) * tmp.numIndices_;
        u16* indices = static_cast<u16*>(LMALLOC(size));

        if(!fileStream_.read(size, indices)){
            LFREE(indices);
            LFREE(vertices);
            return false;
        }

        lgfx::IndexBufferRef ib = lgfx::IndexBuffer::create(
            size,
            lgfx::Usage_Default,
            lgfx::CPUAccessFlag_None,
            lgfx::ResourceMisc_None,
            0,
            indices);

        if(!reserveGeometryData_){
            LFREE(indices);
            LFREE(vertices);
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
    bool ModelLoader::load(Model& model, Mesh& mesh)
    {
        LoadMesh tmp;
        if(!fileStream_.read(tmp)){
            return false;
        }
        mesh.create(
            lgfx::Primitive_TriangleList,
            tmp.indexOffset_,
            tmp.numIndices_,
            &model.getGeometry(tmp.geometry_),
            &model.getMaterial(tmp.material_),
            tmp.sphere_);

        return true;
    }

    //---------------------------------------------
    // マテリアルロード
    bool ModelLoader::load(Material& material)
    {
        LoadMaterial tmp;
        if(!fileStream_.read(tmp)){
            return false;
        }

        material.flags_ = tmp.flags_;
        material.diffuse_ = tmp.diffuse_;
        material.specular_ = tmp.specular_;
        material.ambient_ = tmp.ambient_;
        material.shadow_ = tmp.shadow_;
        material.blendState_ = lgfx::BlendState::create(
            FALSE,
            tmp.blendEnable_,
            static_cast<lgfx::BlendType>(tmp.srcBlend_),
            static_cast<lgfx::BlendType>(tmp.dstBlend_),
            static_cast<lgfx::BlendOp>(tmp.blendOp_),
            lgfx::Blend_SrcAlpha,
            lgfx::Blend_DestAlpha,
            lgfx::BlendOp_Add,
            lgfx::ColorWrite_All);
        for(s32 i=0; i<TextureType_Num; ++i){
            material.textureIDs_[i] = tmp.textureIDs_[i];
        }

        if((material.flags_ & MaterialFlag_RefractiveIndex) && convertRefractiveIndex_){
            material.ambient_.w_ = lcore::calcFresnelTerm(material.ambient_.w_);
        }
        return true;
    }

    //---------------------------------------------
    // ノードロード
    bool ModelLoader::load(Model& model, Node& node)
    {
        LoadNode tmp;
        if(!fileStream_.read(tmp)){
            return false;
        }

        node.index_ = tmp.index_;
        node.parentIndex_ = tmp.parentIndex_;
        node.childStartIndex_ = tmp.childStartIndex_;
        node.numChildren_ = tmp.numChildren_;
        node.translation_ = lmath::Vector4::construct(tmp.translation_);
        //node.rotation_ = tmp.rotation_;
        node.scale_ = tmp.scale_;
        node.type_ = tmp.type_;
        node.rotationOrder_ = tmp.rotationOrder_;
        node.meshStartIndex_ = tmp.meshStartIndex_;
        node.numMeshes_ = tmp.numMeshes_;

        node.calcEuler(tmp.rotation_);
        node.world0_.identity();
        node.world1_.identity();

        if(node.meshStartIndex_ == InvalidNode){
            node.meshes_ = NULL;
        }else{
            node.meshes_ = &model.getMesh(node.meshStartIndex_);
        }

        return true;
    }

    //---------------------------------------------
    // テクスチャロード
    bool ModelLoader::load(s32 setID, Texture2D& texture, const LoadTexture& loadTexture)
    {
        s32 len = lcore::strlen_s32(loadTexture.name_);
        lcore::strncpy(path_+directoryPathLength_, MaxNameSize, loadTexture.name_, len);
        TextureParameter texParam = TextureParameter::SRGBSampler;
        texParam.filterType_ = loadTexture.filterType_;
        texParam.addressUVW_ = loadTexture.addressUVW_;
        texParam.compFunc_ = loadTexture.compFunc_;
        texParam.borderColor_ = loadTexture.borderColor_;

        Resources& resources = System::getResources();
        ResourceTexture2D::pointer resTex;
        if(fileSystem_){
            resTex = resources.load(setID, path_, ResourceType_Texture2D, texParam).get_reinterpret_cast<ResourceTexture2D>();
        }else{
            lcore::File file(path_, lcore::ios::in);
            if(!file.is_open()){
                return false;
            }
            s64 size = file.size();
            u8* buffer = LNEW u8[size];
            file.read(size, buffer);
            resTex = ResourceTexture2D::load(path_, size, buffer, texParam);
            LDELETE_ARRAY(buffer);
        }
        
        if(NULL == resTex){
            return false;
        }
        textures_.push_back(resTex);
        texture.texture_ = resTex->get();
        texture.sampler_ = resTex->getSampler();
        texture.srv_ = resTex->getShaderResourceView();
        return true;
    }

    //---------------------------------------------
    // 頂点レイアウト作成
    void ModelLoader::createInputLayout(u32 vflag, lgfx::InputLayoutRef& inputLayout)
    {
        Resources& resources = System::getResources();
        InputLayoutFactory& inputLayoutFactory = resources.getInputLayoutFactory();

        lfw::InputLayoutType layoutType = lfw::InputLayout_P;
        switch(vflag)
        {
        case (VElem_Position):
            layoutType = lfw::InputLayout_P;
            break;

        case (VElem_Position|VElem_Normal):
            layoutType = lfw::InputLayout_PN;
            break;

        case (VElem_Position|VElem_Texcoord):
            layoutType = lfw::InputLayout_PU;
            break;

        case (VElem_Position|VElem_Color):
            layoutType = lfw::InputLayout_PC;
            break;

        case (VElem_Position|VElem_Normal|VElem_Tangent|VElem_Binormal):
            layoutType = lfw::InputLayout_PNTB;
            break;

        case (VElem_Position|VElem_Normal|VElem_Texcoord):
            layoutType = lfw::InputLayout_PNU;
            break;

        case (VElem_Position|VElem_Normal|VElem_Color|VElem_Texcoord):
            layoutType = lfw::InputLayout_PNCU;
            break;

        case (VElem_Position|VElem_Normal|VElem_Tangent|VElem_Binormal|VElem_Texcoord):
            layoutType = lfw::InputLayout_PNTBU;
            break;

        case (VElem_Position|VElem_Normal|VElem_Texcoord|VElem_Bone|VElem_BoneWeight):
            layoutType = lfw::InputLayout_PNUBone;
            break;

        case (VElem_Position|VElem_Normal|VElem_Tangent|VElem_Binormal|VElem_Texcoord|VElem_Bone|VElem_BoneWeight):
            layoutType = lfw::InputLayout_PNTUBone;
            break;

        default:
            LASSERT(false);
            break;
        }

        inputLayout = inputLayoutFactory.get(layoutType);
    }

    //
    LoadTexture* ModelLoader::loadTextures(const Char* filepath)
    {
        LASSERT(NULL != filepath);
        lcore::FileProxy* file = NULL;
        if(NULL != fileSystem_){
            file = fileSystem_->openFile(filepath);
        } else{
            lcore::LHANDLE handle = CreateFile(
                filepath,
                GENERIC_READ,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL);
            file = LNEW lcore::FileProxyOSRaw(handle);
        }
        if(NULL == file){
            return NULL;
        }
        fileStream_.reset(file);

        if(!load(header_)){
            clearFileStream(file);
            return NULL;
        }

        u32 offsetTextures = header_.elems_[Elem_Texture].offset_;
        u32 numTextures = header_.elems_[Elem_Texture].number_;

        if(numTextures<=0){
            return NULL;
        }
        LoadTexture* textures = LNEW LoadTexture[numTextures];

        fileStream_.seekg(offsetTextures, lcore::ios::beg);
        fileStream_.read(sizeof(LoadTexture)*numTextures, textures);
        clearFileStream(file);
        return textures;
    }

    //
    bool ModelLoader::save(const Char* filepath, Model& model, LoadTexture* textures, Skeleton* skeleton)
    {
        lcore::File os(filepath, lcore::ios::out);
        if(!os.is_open()){
            return false;
        }

        LoadHeader loadHeader;
        loadHeader.major_ = 1;
        loadHeader.minor_ = 0;
        u32 offset = sizeof(LoadHeader);

        LoadGeometry* loadGeometries = LNEW LoadGeometry[model.getNumGeometries()];
        loadHeader.elems_[Elem_Geometry].number_ = model.getNumGeometries();
        loadHeader.elems_[Elem_Geometry].offset_ = offset;
        for(s32 i=0; i<model.getNumGeometries(); ++i){
            Geometry& geometry = model.getGeometry(i);
            loadGeometries[i].vflag_ = geometry.getVFlag();
            loadGeometries[i].vsize_ = geometry.getVSize();
            loadGeometries[i].numVertices_ = geometry.getNumVertices();
            loadGeometries[i].numIndices_ = geometry.getNumIndices();

            offset += sizeof(LoadGeometry);
            offset += geometry.getVSize() * geometry.getNumVertices();
            offset += sizeof(u16) * geometry.getNumIndices();
        }

        LoadMaterial* loadMaterials = LNEW LoadMaterial[model.getNumMaterials()];
        loadHeader.elems_[Elem_Material].number_ = model.getNumMaterials();
        loadHeader.elems_[Elem_Material].offset_ = offset;
        for(s32 i=0; i<model.getNumMaterials(); ++i){
            Material& material = model.getMaterial(i);
            loadMaterials[i].flags_ = material.flags_;
            loadMaterials[i].diffuse_ = material.diffuse_;
            loadMaterials[i].specular_ = material.specular_;
            loadMaterials[i].ambient_ = material.ambient_;
            loadMaterials[i].shadow_ = material.shadow_;
            lgfx::BlendStateDesc blendStateDesc;
            if(material.blendState_.getDesc(blendStateDesc)){
                loadMaterials[i].blendEnable_ = static_cast<u8>(blendStateDesc.RenderTarget[0].BlendEnable);
                loadMaterials[i].srcBlend_ = static_cast<u8>(blendStateDesc.RenderTarget[0].SrcBlend);
                loadMaterials[i].dstBlend_ = static_cast<u8>(blendStateDesc.RenderTarget[0].DestBlend);
                loadMaterials[i].blendOp_ = static_cast<u8>(blendStateDesc.RenderTarget[0].BlendOp);

            } else{
                loadMaterials[i].blendEnable_ = FALSE;
                loadMaterials[i].srcBlend_ = lgfx::Blend_SrcAlpha;
                loadMaterials[i].dstBlend_ = lgfx::Blend_InvSrcAlpha;
                loadMaterials[i].blendOp_ = lgfx::BlendOp_Add;
            }
            for(s32 j=0; j<TextureType_Num; ++j){
                loadMaterials[i].textureIDs_[j] = material.textureIDs_[j];
            }

            offset += sizeof(LoadMaterial);
        }

        LoadMesh* loadMeshes = LNEW LoadMesh[model.getNumMeshes()];
        loadHeader.elems_[Elem_Mesh].number_ = model.getNumMeshes();
        loadHeader.elems_[Elem_Mesh].offset_ = offset;
        for(s32 i=0; i<model.getNumMeshes(); ++i){
            Mesh& mesh = model.getMesh(i);

            loadMeshes[i].geometry_ = static_cast<s16>(mesh.getGeometry() - &model.getGeometry(0));
            loadMeshes[i].material_ = static_cast<s16>(mesh.getMaterial() - &model.getMaterial(0));
            loadMeshes[i].indexOffset_ = mesh.getIndexOffset();
            loadMeshes[i].numIndices_ = mesh.getNumIndices();
            loadMeshes[i].sphere_ = mesh.getSphere();

            offset += sizeof(LoadMesh);
        }

        LoadNode* loadNodes = LNEW LoadNode[model.getNumNodes()];
        loadHeader.elems_[Elem_Node].number_ = model.getNumNodes();
        loadHeader.elems_[Elem_Node].offset_ = offset;
        for(s32 i=0; i<model.getNumNodes(); ++i){
            Node& node = model.getNode(i);
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

            offset += sizeof(LoadNode);
        }

        LoadJoint* loadJoints = NULL;
        if(NULL == skeleton){
            loadHeader.elems_[Elem_Joint].number_ = 0;
            loadHeader.elems_[Elem_Joint].offset_ = offset;
        }else{
            loadJoints = LNEW LoadJoint[skeleton->getNumJoints()];
            loadHeader.elems_[Elem_Joint].number_ = skeleton->getNumJoints();
            loadHeader.elems_[Elem_Joint].offset_ = offset;

            for(s32 i=0; i<skeleton->getNumJoints(); ++i){
                Joint& joint = skeleton->getJoint(i);
                loadJoints[i].parent_ = joint.getParentIndex();
                loadJoints[i].subjectTo_ = joint.getSubjectTo();
                loadJoints[i].type_ = joint.getType();
                loadJoints[i].flag_ = joint.getFlag();
                loadJoints[i].initialMatrix_ = joint.getInitialMatrix();
                lcore::memcpy(loadJoints[i].name_, skeleton->getJointName(i).c_str(), MaxNameSize);
                offset += sizeof(LoadJoint);
            }
        }
        loadHeader.elems_[Elem_Texture].number_ = model.getNumTextures();
        loadHeader.elems_[Elem_Texture].offset_ = offset;


        os.write(loadHeader);
        for(s32 i=0; i<model.getNumGeometries(); ++i){
            Geometry& geometry = model.getGeometry(i);

            os.write(loadGeometries[i]);
            os.write(geometry.getVSize()*geometry.getNumVertices(), geometry.getVertices());
            os.write(sizeof(u16)*geometry.getNumIndices(), geometry.getIndices());
        }

        os.write(sizeof(LoadMaterial)*model.getNumMaterials(), loadMaterials);
        os.write(sizeof(LoadMesh)*model.getNumMeshes(), loadMeshes);
        os.write(sizeof(LoadNode)*model.getNumNodes(), loadNodes);
        if(NULL != loadJoints){
            os.write(sizeof(LoadJoint)*skeleton->getNumJoints(), loadJoints);
        }

        if(NULL != textures){
            os.write(sizeof(LoadTexture)*model.getNumTextures(), textures);
        }

        os.close();

        LDELETE_ARRAY(loadJoints);
        LDELETE_ARRAY(loadNodes);
        LDELETE_ARRAY(loadMeshes);
        LDELETE_ARRAY(loadMaterials);
        LDELETE_ARRAY(loadGeometries);
        return true;
    }
}
